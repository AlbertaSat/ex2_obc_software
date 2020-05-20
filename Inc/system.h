#ifndef __SYSTEM_H
#define __SYSTEM_H
#include <FreeRTOS.h>
#include <csp/csp.h>

#include "queue.h"
#include "services.h"

#define USE_LOCALHOST  // Define for local development, add other options when
                       // available

#define TM_TC_BUFF_SIZE 256
#define NORMAL_TICKS_TO_WAIT 1
#define NORMAL_SERVICE_PRIO 5
#define NORMAL_QUEUE_LEN 3
#define NORMAL_QUEUE_SIZE 256

// Define all the services that the module implements
typedef struct {
  xQueueHandle verification_app_queue;
  xQueueHandle hk_app_queue;
  xQueueHandle test_app_queue;
} service_queues_t;

#endif
