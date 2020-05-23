#include "time_management_service.h"

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <stdio.h>

#include "service_utilities.h"

#undef __FILE_ID__
#define __FILE_ID__ 2

#if (SYSTEM_APP_ID == DEMO_APP_ID_)
#include "demo_hal.h"
#endif

SAT_returnState time_management_app(csp_packet_t *pkt) {
  uint8_t ser_subtype;
  struct time_utc temp_time;

  ser_subtype = (uint8_t)pkt->data[0];

  if (ser_subtype == TM_TIME_SET_IN_UTC) {
    printf("SET TIME\n");
    cnv8_32(&pkt->data[1], &temp_time.unix_timestamp);
    if (!TIMESTAMP_ISOK(temp_time.unix_timestamp)) {
      printf("bad timestamp format\n");
      return SATR_ERROR;
    }
    printf("ts: %d\n", temp_time.unix_timestamp);
    fflush(stdout);
    set_time_UTC(temp_time);
    // pkt->verification_state = SATR_OK;
  }
  // else if (ser_subtype == TM_TIME_SET_IN_UTC) {
  //   /*set time in utc mode*/
  //   temp_time.unix_timestamp = (uint32_t) pkt->data[1];
  //   TIMESTAMP_ISOK(temp_time.unix_timestamp)
  //   set_time_UTC(temp_time);
  //   pkt->verification_state = SATR_OK;
  // } else if (ser_subtype == TM_REPORT_TIME_IN_QB50) {
  //   tc_tm_pkt *time_rep_pkt = get_pkt(PKT_NORMAL);
  //   /*make the packet to send*/
  //   time_management_report_time_in_qb50(time_rep_pkt,
  //                                       (TC_TM_app_id)pkt->dest_id);
  //   pkt->verification_state = SATR_OK;
  //   if (!C_ASSERT(time_rep_pkt != NULL) == true) {
  //     return SATR_ERROR;
  //   }
  //   route_pkt(time_rep_pkt);
  // } else if (ser_subtype == TM_REPORT_TIME_IN_UTC) {
  //   tc_tm_pkt *time_rep_pkt = get_pkt(PKT_NORMAL);
  //   if (!C_ASSERT(time_rep_pkt != NULL) == true) {
  //     return SATR_ERROR;
  //   }
  //   time_management_report_time_in_utc(time_rep_pkt,
  //                                      (TC_TM_app_id)pkt->dest_id);
  //   pkt->verification_state = SATR_OK;
  //   route_pkt(time_rep_pkt);
  // } else if (ser_subtype == TM_TIME_REPORT_IN_UTC) {
  //   /* time report from a time_management_service implementor in UTC format
  //    * exists here,
  //    * user should implement his own code to handle the time report
  //    response*/
  //   /*set time in utc mode*/
  //   if (!(C_ASSERT(pkt->data[0] >= 1) && C_ASSERT(pkt->data[0] < 8)) == true)
  //   {
  //     return SATR_ERROR;
  //   } /*weekday1to7*/
  //   if (!(C_ASSERT(pkt->data[1] > 0) && C_ASSERT(pkt->data[1] < 32)) == true)
  //   {
  //     return SATR_ERROR;
  //   } /*day1to31*/
  //   if (!(C_ASSERT(pkt->data[2] > 0) && C_ASSERT(pkt->data[2] < 13)) == true)
  //   {
  //     return SATR_ERROR;
  //   } /*month1to12*/
  //   if (!(C_ASSERT(pkt->data[3] >= 15) && C_ASSERT(pkt->data[3] < 100)) ==
  //       true) {
  //     return SATR_ERROR;
  //   } /*assert if year before or equal 2015*/
  //   if (!(C_ASSERT(pkt->data[4] >= 0) && C_ASSERT(pkt->data[4] < 24)) ==
  //   true) {
  //     return SATR_ERROR;
  //   } /*hours0to23*/
  //   if (!(C_ASSERT(pkt->data[5] >= 0) && C_ASSERT(pkt->data[5] < 60)) ==
  //   true) {
  //     return SATR_ERROR;
  //   } /*minutes0to59*/
  //   if (!(C_ASSERT(pkt->data[6] >= 0) && C_ASSERT(pkt->data[6] < 60)) ==
  //   true) {
  //     return SATR_ERROR;
  //   } /*seconds0to59*/
  //   temp_time.weekday = pkt->data[0];
  //   temp_time.day = pkt->data[1];
  //   temp_time.month = pkt->data[2];
  //   temp_time.year = pkt->data[3];
  //   temp_time.hour = pkt->data[4];
  //   temp_time.min = pkt->data[5];
  //   temp_time.sec = pkt->data[6];
  //   set_time_UTC(temp_time);
  //   pkt->verification_state = SATR_OK;
  // } else if (ser_subtype == TM_TIME_REPORT_IN_QB50) {
  //   /*time report from a time_management_service implementor in QB50 format
  //    * exists here,
  //    * user should implement his own code to handle the time report
  //    response*/
  //   pkt->verification_state = SATR_ERROR;
  // }

  return SATR_OK;
}

// /**
//  * Reports time in QB50 epoch format (seconds from 2000)
//  * @param pkt
//  * @param dest_id
//  * @return
//  */
// SAT_returnState time_management_report_time_in_qb50(tc_tm_pkt *pkt,
//                                                     TC_TM_app_id dest_id) {
//   uint32_t qb_temp_secs = 0;
//   if (!C_ASSERT(pkt != NULL) == true) {
//     return SATR_ERROR;
//   }
//   get_time_QB50(&qb_temp_secs);
//   crt_pkt(pkt, SYSTEM_APP_ID, TM, TC_ACK_NO, TC_TIME_MANAGEMENT_SERVICE,
//           TM_TIME_REPORT_IN_QB50, dest_id);
//   cnv32_8(qb_temp_secs, pkt->data);
//   pkt->len = 4;
//   return SATR_OK;
// }
//
// /**
//  * Reports the time in UTC format.
//  * @param pkt
//  * @param dest_id
//  * @return
//  */
// SAT_returnState time_management_report_time_in_utc(tc_tm_pkt *pkt,
//                                                    TC_TM_app_id dest_id) {
//   struct time_utc temp_time;
//   if (!C_ASSERT(pkt != NULL) == true) {
//     return SATR_ERROR;
//   }
//   get_time_UTC(&temp_time);
//   time_management_crt_pkt_TM(pkt, TM_TIME_REPORT_IN_UTC, dest_id);
//   pkt->data[0] = temp_time.weekday;
//   pkt->data[1] = temp_time.day;
//   pkt->data[2] = temp_time.month;
//   pkt->data[3] = temp_time.year;
//   pkt->data[4] = temp_time.hour;
//   pkt->data[5] = temp_time.min;
//   pkt->data[6] = temp_time.sec;
//   pkt->len = 7;
//
//   return SATR_OK;
// }
//
// /**
//  * Requests time in UTC format from a time_management_service implementor.
//  * @param dest_id is the on-board time_management service implementor to
//  request
//  * time from.
//  * @return
//  */
// SAT_returnState time_management_request_time_in_utc(TC_TM_app_id dest_id) {
//   tc_tm_pkt *time_req_pkt;
//   time_req_pkt = get_pkt(PKT_NORMAL);
//
//   if (!C_ASSERT(time_req_pkt != NULL) == true) {
//     return SATR_ERROR;
//   }
//   time_management_crt_pkt_TC(time_req_pkt, TM_REPORT_TIME_IN_UTC, dest_id);
//   route_pkt(time_req_pkt);
//   return SATR_OK;
// }
//
// /**
//  * Forces time update in UTC format to a time_management_service implementor.
//  * @param dest_id is the on-board time_management service implementor to
//  force
//  * time update on.
//  * @return
//  */
// SAT_returnState time_management_force_time_update(TC_TM_app_id dest_id) {
//   struct time_utc temp_time;
//   tc_tm_pkt *time_req_pkt;
//   time_req_pkt = get_pkt(PKT_NORMAL);
//
//   get_time_UTC(&temp_time);
//   if (!C_ASSERT(time_req_pkt != NULL) == true) {
//     return SATR_ERROR;
//   }
//
//   time_management_crt_pkt_TM(time_req_pkt, TM_TIME_SET_IN_UTC, dest_id);
//   (time_req_pkt)->data[0] = temp_time.weekday;
//   (time_req_pkt)->data[1] = temp_time.day;
//   (time_req_pkt)->data[2] = temp_time.month;
//   (time_req_pkt)->data[3] = temp_time.year;
//   (time_req_pkt)->data[4] = temp_time.hour;
//   (time_req_pkt)->data[5] = temp_time.min;
//   (time_req_pkt)->data[6] = temp_time.sec;
//   (time_req_pkt)->len = 7;
//
//   route_pkt(time_req_pkt);
//   return SATR_OK;
// }
//
// SAT_returnState time_management_crt_pkt_TC(tc_tm_pkt *pkt, uint8_t sid,
//                                            TC_TM_app_id dest_app_id) {
//   if (!C_ASSERT(dest_app_id < LAST_APP_ID) == true) {
//     return SATR_ERROR;
//   }
//   crt_pkt(pkt, dest_app_id, TC, TC_ACK_NO, TC_TIME_MANAGEMENT_SERVICE, sid,
//           SYSTEM_APP_ID);
//   pkt->len = 0;
//
//   return SATR_OK;
// }
//
// SAT_returnState time_management_crt_pkt_TM(tc_tm_pkt *pkt, uint8_t sid,
//                                            TC_TM_app_id dest_app_id) {
//   if (!C_ASSERT(dest_app_id < LAST_APP_ID) == true) {
//     return SATR_ERROR;
//   }
//   crt_pkt(pkt, SYSTEM_APP_ID, TM, TC_ACK_NO, TC_TIME_MANAGEMENT_SERVICE, sid,
//           dest_app_id);
//   pkt->len = 0;
//
//   return SATR_OK;
// }
//
// /**
//  * converts a day's moment (Hour, Minutes, Seconds) in this day's seconds.
//  * @param moment
//  * @param daysecs
//  * @return
//  */
// SAT_returnState cnv_utc_to_secs(struct time_utc *moment,
//                                 uint32_t *daysecs) {  // 1 day = 86400 secs
//
//   if (!C_ASSERT(moment->hour >= 0 && moment->hour <= 24) == true) {
//     return SATR_ERROR;
//   }
//   if (!C_ASSERT(moment->min >= 0 && moment->min <= 60) == true) {
//     return SATR_ERROR;
//   }
//   if (!C_ASSERT(moment->sec >= 0 && moment->sec <= 60) == true) {
//     return SATR_ERROR;
//   }
//   *daysecs = (moment->hour * 3600) + (moment->min * 60) + (moment->sec);
//
//   return SATR_OK;
// }
//
// void cnv_UTC_QB50(struct time_utc utc, uint32_t *qb) {
//   *qb = (UTC_QB50_YM[utc.year][utc.month] + UTC_QB50_D[utc.day] +
//          UTC_QB50_H[utc.hour] + (utc.min * 60) + utc.sec) -
//         UTC_QB50_D[1];
// }
//
// void set_time_QB50(uint32_t qb) { /*no general meaning(?)*/
// }
//
void set_time_UTC(struct time_utc utc) { HAL_sys_setTime(utc.unix_timestamp); }
//
// /**
//  * Fills the destination uint32_t pointer with
//  * QB50 epoch (seconds from 2000)
//  * @param qb
//  */
// void get_time_QB50(uint32_t *qb) {
//   struct time_utc utc;
//   HAL_sys_getTime(&utc.hour, &utc.min, &utc.sec);
//   HAL_sys_getDate(&utc.weekday, &utc.month, &utc.day, &utc.year);
//   cnv_UTC_QB50(utc, qb);
// }
//
// /**
//  * Returns time in QB50 epoch (seconds from 2000)
//  * @return
//  */
// uint32_t return_time_QB50() {
//   struct time_utc utc;
//   uint32_t qb_secs;
//   HAL_sys_getTime(&utc.hour, &utc.min, &utc.sec);
//   HAL_sys_getDate(&utc.weekday, &utc.month, &utc.day, &utc.year);
//   cnv_UTC_QB50(utc, &qb_secs);
//   return qb_secs;
// }
//
// void get_time_UTC(struct time_utc *utc) {
//   HAL_sys_getTime(&utc->hour, &utc->min, &utc->sec);
//   HAL_sys_getDate(&utc->weekday, &utc->month, &utc->day, &utc->year);
// }
