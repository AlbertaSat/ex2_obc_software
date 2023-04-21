/*
 * Copyright 2001-2010 Georges Menie (www.menie.org)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* this code needs standard functions memcpy() and memset()
   and input/output functions _inbyte() and _outbyte().

   the prototypes of the input/output functions are:
     int _inbyte(unsigned short timeout); // msec timeout
     void _outbyte(int c);

 */

#include <base_64.h>
#include "xmodem.h"
#include "northern_spirit_io.h"
#include <string.h>
#include <redposix.h>
#include "logger/logger.h"

int _inbyte(void *dat, size_t len, unsigned short timeout) {
    memset(dat, 0, len);
    if (NS_expectResponse(dat, len, timeout) == NS_OK) {
        return 0;
    } else {
        return -1;
    }
}

void _outbyte(void *c, size_t len) { NS_sendOnly((uint8_t *) c, len); }

unsigned short crc16_ccitt(const void *buf, int len) {
    register int counter;
    register unsigned short crc = 0;
    const char *p = (const char *)buf;
    for (counter = 0; counter < len; counter++)
        crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ *p++) & 0x00FF];
    return crc;
}

static int check(int crc, const char *buf, int sz) {
    if (crc) {
        unsigned short crc = crc16_ccitt(buf, sz);
        unsigned short tcrc = (buf[sz] << 8) + buf[sz + 1];
        if (crc == tcrc)
            return 1;
    } else {
        int i;
        unsigned char cks = 0;
        for (i = 0; i < sz; ++i) {
            cks += buf[i];
        }
        if (cks == buf[sz])
            return 1;
    }

    return 0;
}

static void flushinput(void) { NS_resetQueue(); }

int xmodemReceive(const unsigned char *dest) {
    char xbuff[133]; /* 128 for XModem 1k + 3 head chars + 2 crc + nul */
    int bufsz, crc = 0;
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    int len = 0;
    char c;
    int retry, retrans = MAXRETRANS;
    int fd = red_open((const char *) dest, RED_O_WRONLY | RED_O_CREAT);
    char ack = ACK;
    char nak = NAK;
    char can = CAN;
    if (fd < 0) {
        return -1;
    }
    for (;;) {
        for (retry = 0; retry < 16; ++retry) {
            if (trychar)
                _outbyte(&trychar, 1);
            _inbyte(&c, 1, DLY_1S);
            if ((c << 1) >= 0) {
                switch (c) {
                case SOH:
                    bufsz = 128;
                    goto start_recv;
                case STX:
                    bufsz = 1024;
                    goto start_recv;
                case EOT:
                    flushinput();
                    _outbyte(&ack, 1);
                    red_close(fd);
                    return len; /* normal end */
                case CAN:
                    _inbyte(&c, 1, DLY_1S);
                    if (c == CAN) {
                        flushinput();
                        _outbyte(&ack, 1);
                        red_close(fd);
                        return -1; /* canceled by remote */
                    }
                    break;
                default:
                    break;
                }
            }
        }
        if (trychar == 'C') {
            trychar = NAK;
            continue;
        }
        flushinput();
        _outbyte(&can, 1);
        _outbyte(&can, 1);
        _outbyte(&can, 1);
        return -2; /* sync error */

    start_recv:
        if (trychar == 'C')
            crc = 1;
        trychar = 0;
        int buflen = (bufsz + (crc ? 1 : 0) + 3);
        xbuff[0] = c;
        _inbyte(&(xbuff[1]), buflen, 100);

        if (xbuff[1] == (unsigned char)(~xbuff[2]) &&
            (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno - 1) && check(crc, &xbuff[3], bufsz)) {
            if (xbuff[1] == packetno) {
                size_t binary_size;
                void *data = base64_decode(&xbuff[3], bufsz, &binary_size);
                red_write(fd, data, binary_size);
                vPortFree(data);
                len += bufsz;
                ++packetno;
                retrans = MAXRETRANS + 1;
            }
            if (--retrans <= 0) {
                flushinput();
                _outbyte(&can, 1);
                _outbyte(&can, 1);
                _outbyte(&can, 1);
                red_close(fd);
                return -3; /* too many retry error */
            }
            _outbyte(&ack, 1);
            continue;
        }

        flushinput();
        _outbyte(&nak, 1);
    }
}

int xmodemTransmit(int32_t filedes, uint64_t filesz) {
#ifdef TRANSMIT_XMODEM_1K
    unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
#else
    unsigned char xbuff[134]; /* 128 for XModem + 3 head chars + 2 crc + nul */
#endif

    int bufsz, crc = -1;
    unsigned char packetno = 1;
    int i, len = 0;
    int retry;
    char c = 0;
    char ack = ACK;
    char can = CAN;
    char eot = EOT;
    for (;;) {
        for (retry = 0; retry < 16; ++retry) {
            _inbyte(&c, 1, DLY_1S);
            if ((c << 1) >= 0) {
                switch (c) {
                case 'C':
                    crc = 1;
                    goto start_trans;
                case NAK:
                    crc = 0;
                    goto start_trans;
                case CAN:
                    _inbyte(&c, 1, DLY_1S);
                    if (c == CAN) {
                        _outbyte(&ack, 1);
                        flushinput();
                        return -1; /* canceled by remote */
                    }
                    break;
                default:
                    break;
                }
            }
        }
        _outbyte(&can, 1);
        _outbyte(&can, 1);
        _outbyte(&can, 1);
        flushinput();
        return -2; /* no sync */

        for (;;) {
        start_trans:
#ifdef TRANSMIT_XMODEM_1K
            xbuff[0] = STX;
            bufsz = 1024;
#else
            xbuff[0] = SOH;
            bufsz = 128;
#endif
            xbuff[1] = packetno;
            xbuff[2] = ~packetno;
            c = filesz - len;

            if (c > bufsz)
                c = bufsz;
            if (c > 0) {
                // Transmit next xmodem packet
                memset(&xbuff[3], '=', bufsz);
                int32_t bytes_read = red_read(filedes, &xbuff[3], 96);
                len += 96;
                if (bytes_read == -1) {
                    goto trans_error;
                } else if (bytes_read == 0) {
                    goto done_trans;
                } else {
                    size_t encoded_len;
                    char *temp = base64_encode(&xbuff[3], bytes_read, &encoded_len);
                    if (!temp) {
                        goto trans_error;
                    }
                    if (encoded_len < c) {
                        c = encoded_len;
                    }
                    memcpy(&xbuff[3], temp, encoded_len);
                    vPortFree(temp);
                }
                if (crc) {
                    unsigned short ccrc = crc16_ccitt(&xbuff[3], bufsz);
                    xbuff[bufsz + 3] = (ccrc >> 8) & 0xFF;
                    xbuff[bufsz + 4] = ccrc & 0xFF;
                } else {
                    unsigned char ccks = 0;
                    for (i = 3; i < bufsz + 3; ++i) {
                        ccks += xbuff[i];
                    }
                    xbuff[bufsz + 3] = ccks;
                }
                for (retry = 0; retry < MAXRETRANS; ++retry) {
                    int transmitlen = bufsz + 4 + (crc ? 1 : 0);
                    _outbyte(xbuff, transmitlen);
                    _inbyte(&c, 1, DLY_1S);
                    if ((int)c >= 0) {
                        switch (c) {
                        case ACK:
                            ++packetno;
                            // len += bufsz;
                            goto start_trans;
                        case CAN:
                            _inbyte(&c, 1, DLY_1S);
                            if (c == CAN) {
                                _outbyte(&ack, 1);
                                flushinput();
                                return -1; /* canceled by remote */
                            }
                            break;
                        case NAK:
                        default:
                            break;
                        }
                    }
                }
            trans_error:
                _outbyte(&can, 1);
                _outbyte(&can, 1);
                _outbyte(&can, 1);
                flushinput();
                return -4; /* xmit error */
            } else {
            done_trans:
                // Reached end of transmission
                _outbyte(&eot, 1);
                _inbyte(&c, 1, DLY_1S);
                flushinput();
                return (c == ACK) ? len : -5;
            }
        }
    }
}
