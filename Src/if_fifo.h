#ifndef __IF_FIFO_H
#define __IF_FIFO_H

void fifo_rx(void *parameters);
int csp_fifo_tx(csp_iface_t *ifc, csp_packet_t *packet, uint32_t timeout);
int rx_channel, tx_channel;

#endif
