#include "service_utilities.h"
#include "time_management_service.h"

#undef __FILE_ID__
#define __FILE_ID__ 5

struct _pkt_state {
    uint8_t seq_cnt[LAST_APP_ID];
};

static struct _pkt_state pkt_state;

// need to check endiannes
void cnv32_8(const uint32_t from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnv32 = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];
    to[2] = cnv.cnv8[2];
    to[3] = cnv.cnv8[3];
}

void cnv16_8(const uint16_t from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnv16[0] = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];

}

void cnv8_32(uint8_t *from, uint32_t *to) {

    union _cnv cnv;

    cnv.cnv8[3] = from[3];
    cnv.cnv8[2] = from[2];
    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnv32;

}

void cnv8_16LE(uint8_t *from, uint16_t *to) {

    union _cnv cnv;

    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnv16[0];
}

void cnv8_16(uint8_t *from, uint16_t *to) {

    union _cnv cnv;

    cnv.cnv8[1] = from[0];
    cnv.cnv8[0] = from[1];
    *to = cnv.cnv16[0];
}


void cnvF_8(const float from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnvF = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];
    to[2] = cnv.cnv8[2];
    to[3] = cnv.cnv8[3];
}

void cnv8_F(uint8_t *from, float *to) {

    union _cnv cnv;

    cnv.cnv8[3] = from[3];
    cnv.cnv8[2] = from[2];
    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnvF;

}

void cnvD_8(const double from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnvD = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];
    to[2] = cnv.cnv8[2];
    to[3] = cnv.cnv8[3];
    to[4] = cnv.cnv8[4];
    to[5] = cnv.cnv8[5];
    to[6] = cnv.cnv8[6];
    to[7] = cnv.cnv8[7];
}

void cnv8_D(uint8_t *from, double *to) {

    union _cnv cnv;

    cnv.cnv8[7] = from[7];
    cnv.cnv8[6] = from[6];
    cnv.cnv8[5] = from[5];
    cnv.cnv8[4] = from[4];
    cnv.cnv8[3] = from[3];
    cnv.cnv8[2] = from[2];
    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnvD;

}

// SAT_returnState checkSum(const uint8_t *data, const uint16_t size, uint8_t *res_crc) {
//
//     if(!C_ASSERT(data != NULL && size != 0) == true)                         { return SATR_ERROR; }
//     if(!C_ASSERT(size > TC_MIN_PKT_SIZE-3 && size < MAX_PKT_SIZE-2) == true) { return SATR_ERROR; }
//
//     *res_crc = 0;
//     for(int i=0; i<=size; i++){
//         *res_crc = *res_crc ^ data[i];
//     }
//
//     return SATR_OK;
// }
//
// /*Must check for endianess*/
// /*size: is the bytes of the buf*/
// SAT_returnState unpack_pkt(const uint8_t *buf, tc_tm_pkt *pkt, const uint16_t size) {
//
//     uint8_t tmp_crc[2];
//
//     uint8_t ver, dfield_hdr, ccsds_sec_hdr, tc_pus;
//
//     if(!C_ASSERT(buf != NULL && pkt != NULL && pkt->data != NULL) == true)  { return SATR_ERROR; }
//     if(!C_ASSERT(size > TC_MIN_PKT_SIZE && size < MAX_PKT_SIZE) == true)    { return SATR_ERROR; }
//
//     tmp_crc[0] = buf[size - 1];
//     checkSum(buf, size-2, &tmp_crc[1]); /* -2 for excluding the checksum bytes*/
//
//     ver = buf[0] >> 5;
//
//     pkt->type = (buf[0] >> 4) & 0x01;
//     dfield_hdr = (buf[0] >> 3) & 0x01;
//
//     pkt->app_id = (TC_TM_app_id)buf[1];
//
//     pkt->seq_flags = buf[2] >> 6;
//
//     cnv8_16((uint8_t*)&buf[2], &pkt->seq_count);
//     pkt->seq_count &= 0x3FFF;
//
//     cnv8_16((uint8_t*)&buf[4], &pkt->len);
//
//     ccsds_sec_hdr = buf[6] >> 7;
//
//     tc_pus = buf[6] >> 4;
//
//     pkt->ack = 0x07 & buf[6];
//
//     pkt->ser_type = buf[7];
//     pkt->ser_subtype = buf[8];
//     pkt->dest_id = (TC_TM_app_id)buf[9];
//
//     pkt->verification_state = SATR_PKT_INIT;
//
//     // if(!C_ASSERT(pkt->app_id < LAST_APP_ID &&
//     //              pkt->dest_id < LAST_APP_ID &&
//     //              pkt->app_id != pkt->dest_id) == true) {
//     //     pkt->verification_state = SATR_PKT_ILLEGAL_APPID;
//     //     return SATR_PKT_ILLEGAL_APPID;
//     // }
//     //
//     // if(!C_ASSERT(pkt->len == size - ECSS_HEADER_SIZE - 1) == true) {
//     //     pkt->verification_state = SATR_PKT_INV_LEN;
//     //     return SATR_PKT_INV_LEN;
//     // }
//     // pkt->len = pkt->len - ECSS_DATA_HEADER_SIZE - ECSS_CRC_SIZE + 1;
//     //
//     // if(!C_ASSERT(tmp_crc[0] == tmp_crc[1]) == true) {
//     //     pkt->verification_state = SATR_PKT_INC_CRC;
//     //     return SATR_PKT_INC_CRC;
//     // }
//     //
//     // if(!C_ASSERT(pkt->ser_type < MAX_SERVICES && pkt->ser_subtype < MAX_SUBTYPES && pkt->type <= TC) == true) {
//     //     pkt->verification_state = SATR_PKT_ILLEGAL_PKT_TP;
//     //     return SATR_PKT_ILLEGAL_PKT_TP;
//     // }
//     //
//     // if(!C_ASSERT(services_verification_TC_TM[pkt->ser_type][pkt->ser_subtype][pkt->type] == 1) == true) {
//     //     pkt->verification_state = SATR_PKT_ILLEGAL_PKT_TP;
//     //     SYSVIEW_PRINT("INV TP %u,%u,%u,%u,%u", pkt->type, pkt->app_id, pkt->dest_id, pkt->ser_type, pkt->ser_subtype);
//     //     return SATR_PKT_ILLEGAL_PKT_TP;
//     // }
//     //
//     // if(!C_ASSERT(ver == ECSS_VER_NUMBER) == true) {
//     //     pkt->verification_state = SATR_ERROR;
//     //     return SATR_ERROR;
//     // }
//     //
//     // if(!C_ASSERT(tc_pus == ECSS_PUS_VER) == true) {
//     //     pkt->verification_state = SATR_ERROR;
//     //     return SATR_ERROR;
//     // }
//     //
//     // if(!C_ASSERT(ccsds_sec_hdr == ECSS_SEC_HDR_FIELD_FLG) == true) {
//     //     pkt->verification_state = SATR_ERROR;
//     //     return SATR_ERROR;
//     // }
//     //
//     // if(!C_ASSERT(pkt->type == TC || pkt->type == TM) == true) {
//     //     pkt->verification_state = SATR_ERROR;
//     //     return SATR_ERROR;
//     // }
//     //
//     // if(!C_ASSERT(dfield_hdr == ECSS_DATA_FIELD_HDR_FLG) == true) {
//     //     pkt->verification_state = SATR_ERROR;
//     //     return SATR_ERROR;
//     // }
//     //
//     // if(!C_ASSERT(pkt->ack == TC_ACK_NO || pkt->ack == TC_ACK_ACC) == true) {
//     //     pkt->verification_state = SATR_ERROR;
//     //     return SATR_ERROR;
//     // }
//     //
//     // if(!C_ASSERT(pkt->seq_flags == TC_TM_SEQ_SPACKET) == true) {
//     //     pkt->verification_state = SATR_ERROR;
//     //     return SATR_ERROR;
//     // }
//
//     /*assertion for data size depanding on pkt type*/
//     //if(!C_ASSERT(pkt->len == pkt_size[app_id][type][subtype][generic] == true) {
//     //    pkt->verification_state = SATR_ERROR;
//     //    return SATR_ERROR;
//     //}
//
//     for(int i = 0; i < pkt->len; i++) {
//         pkt->data[i] = buf[ECSS_DATA_OFFSET+i];
//     }
//
//     return SATR_OK;
// }
//
//
// /*buf: buffer to store the data to be sent, pkt: the data to be stored in the buffer, size: size of the array*/
// SAT_returnState pack_pkt(uint8_t *buf, tc_tm_pkt *pkt, uint16_t *size) {
//
//     union _cnv cnv;
//     uint16_t buf_pointer;
//
//     if(!C_ASSERT(buf != NULL && pkt != NULL && pkt->data != NULL  && size != NULL) == true) { return SATR_ERROR; }
//     if(!C_ASSERT(pkt->type == TC || pkt->type == TM) == true)                               { return SATR_ERROR; }
//     if(!C_ASSERT(pkt->app_id < LAST_APP_ID &&
//                  pkt->dest_id < LAST_APP_ID &&
//                  pkt->app_id != pkt->dest_id) == true)                                      { return SATR_ERROR; }
//
//     cnv.cnv16[0] = pkt->app_id;
//
//     buf[0] = ( ECSS_VER_NUMBER << 5 | pkt->type << 4 | ECSS_DATA_FIELD_HDR_FLG << 3 | cnv.cnv8[1]);
//     buf[1] = cnv.cnv8[0];
//
//     /*if the pkt was created in OBC, it updates the counter*/
//     if(pkt->type == TC && pkt->dest_id == SYSTEM_APP_ID)      { pkt->seq_count = pkt_state.seq_cnt[pkt->app_id]++; }
//     else if(pkt->type == TM && pkt->app_id == SYSTEM_APP_ID)  { pkt->seq_count = pkt_state.seq_cnt[pkt->dest_id]++; }
//
//     pkt->seq_flags = TC_TM_SEQ_SPACKET;
//     cnv.cnv16[0] = pkt->seq_count;
//     buf[2] = (pkt->seq_flags << 6 | cnv.cnv8[1]);
//     buf[3] = cnv.cnv8[0];
//
//     /* TYPE = 0 TM, TYPE = 1 TC*/
//     if(pkt->type == TM) {
//         buf[6] = ECSS_PUS_VER << 4 ;
//     } else if(pkt->type == TC) {
//         buf[6] = ( ECSS_SEC_HDR_FIELD_FLG << 7 | ECSS_PUS_VER << 4 | pkt->ack);
//     }
//
//     buf[7] = pkt->ser_type;
//     buf[8] = pkt->ser_subtype;
//     buf[9] = pkt->dest_id; /*source or destination*/
//
//     buf_pointer = ECSS_DATA_OFFSET;
//
//     for(int i = 0; i < pkt->len; i++) {
//         buf[buf_pointer++] = pkt->data[i];
//     }
//
//     pkt->len += ECSS_DATA_HEADER_SIZE + ECSS_CRC_SIZE - 1;
//
//     /*check if this is correct*/
//     cnv.cnv16[0] = pkt->len;
//     buf[4] = cnv.cnv8[1];
//     buf[5] = cnv.cnv8[0];
//
//     /*added it for ecss conformity, checksum in the ecss is defined to have 16 bits, we only use 8*/
//     buf[buf_pointer++] = 0;
//     checkSum(buf, buf_pointer-2, &buf[buf_pointer]);
//     *size = buf_pointer + 1;
//
//     if(!C_ASSERT(*size > TC_MIN_PKT_SIZE && *size < MAX_PKT_SIZE) == true)       { return SATR_ERROR; }
//
//     return SATR_OK;
// }
//
// SAT_returnState crt_pkt(tc_tm_pkt *pkt, TC_TM_app_id app_id, uint8_t type, uint8_t ack, uint8_t ser_type, uint8_t ser_subtype, TC_TM_app_id dest_id) {
//
//     if(!C_ASSERT(pkt != NULL && pkt->data != NULL) == true)                 { return SATR_ERROR; }
//     if(!C_ASSERT(app_id < LAST_APP_ID && dest_id < LAST_APP_ID ) == true)   { return SATR_ERROR; }
//     if(!C_ASSERT(type == TC || type == TM) == true)                         { return SATR_ERROR; }
//     if(!C_ASSERT(ack == TC_ACK_NO || ack == TC_ACK_ACC) == true)            { return SATR_ERROR; }
//
//     pkt->type = type;
//     pkt->app_id = app_id;
//     pkt->dest_id = dest_id;
//     pkt->ser_type = ser_type;
//     pkt->ser_subtype = ser_subtype;
//     pkt->verification_state = SATR_PKT_INIT;
//
//     return SATR_OK;
// }

SAT_returnState sys_data_INIT() {
  for(uint8_t i = 0; i < LAST_APP_ID; i++) { pkt_state.seq_cnt[i] = 0; }
  return SATR_OK;
}

uint16_t
htons (uint16_t x)
{
#if (SYSTEM_ENDIANESS == SYS_LITTLE_ENDIAN)
  uint16_t ret = 0x0;
  ret = (x & 0xFF00) >> 8;
  ret |= (x & 0x00FF) << 8;
  return ret;
#else
  return x;
#endif
}

uint16_t
ntohs (uint16_t x)
{
  return htons(x);
}
