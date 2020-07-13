#ifndef HOUSEKEEPING_SERVICE_H
#define HOUSEKEEPING_SERVICE_H

#include <csp/csp.h>
#include "services.h"

/* Housekeeping service address & port*/

#define HK_PARAMETERS_REPORT 25
//#define TM_HK_PARAMETERS_REPORT 21

#define HK_PR_ERR -1
#define HK_PR_OK 0

extern unsigned int count;

// extern CSP_DEFINE_TASK(hk_app);
SAT_returnState hk_service_app(csp_packet_t* pkt);

static SAT_returnState hk_parameter_report(csp_packet_t *packet);

/*hk data sample*/
typedef enum {
  EPS_CURRENT_VAL,
  EPS_CURRENT_STATE,
  EPS_VOLTAGE_VAL,
  EPS_VOLTAGE_STATE,
  EPS_TEMPERATURE_VAL,
  EPS_TEMPERATURE_STATE,
  EPS_ALERT,
  EPS_SIZE
} data_sample;

#endif /* HOUSEKEEPING_SERVICE_H */
