#ifndef HOUSEKEEPING_SERVICE_H
#define HOUSEKEEPING_SERVICE_H

#include <csp/csp.h>

#include "services.h"

/* Housekeeping service address & port*/

#define HK_PARAMETERS_REPORT 25
//#define TM_HK_PARAMETERS_REPORT 21

#define HK_PR_ERR -1
#define HK_PR_OK 0

// HOUSEKEEPING SERVICE
#define TC_HOUSEKEEPING_SERVICE 9
typedef enum { TM_HK_PARAMETERS_REPORT = 0 } Housekeeping_Subtype;

SAT_returnState hk_service_app(csp_packet_t* pkt);
SAT_returnState start_housekeeping_service(void);
static SAT_returnState hk_parameter_report(csp_packet_t* packet);

/*hk data sample*/
typedef enum { EPS, ADCS, OBC, COMMS } hardware;

#endif /* HOUSEKEEPING_SERVICE_H */
