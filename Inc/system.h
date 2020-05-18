#ifndef __SYSTEM_H
#define __SYSTEM_H
#include <csp/csp.h>
#include <services.h>

#define TM_TC_BUFF_SIZE 250
#define NORMAL_TICKS_TO_WAIT 1
#define NORMAL_SERVICE_PRIO 5
#define NORMAL_QUEUE_LEN 3
#define NORMAL_QUEUE_SIZE (sizeof(csp_packet_t *))

// Define all the services that the module implements
typedef struct {
  xQueueHandle verification_app_queue;
  xQueueHandle hk_app_queue;
  xQueueHandle test_app_queue;
}service_queues_t;

SAT_returnState start_service_handlers()

#endif
