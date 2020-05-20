#include <FreeRTOS.h>
#include <stdio.h>

#include "system.h"
#include "task.h"

extern service_queues_t service_queues;

static void test_app(void *parameters) {
  csp_packet_t packet;
  for (;;) {
    if (xQueueReceive(service_queues.test_app_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      printf("TEST SERVICE RX: %.*s, ID: %d\n", packet.length, (char*) packet.data,
             packet.id);
    }
  }
}

static void hk_app(void *parameters) {
  csp_packet_t packet;
  for (;;) {
    if (xQueueReceive(service_queues.hk_app_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      printf("HOUSEKEEPING SERVICE RX: %.*s, ID: %d\n", packet.length, (char*) packet.data,
             packet.id);
    }
  }
}

SAT_returnState start_service_handlers() {
  /**
   * Create the queues & tasks for each service implemented by this module
   */
  if (!(service_queues.verification_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE VERIFICATION APP QUEUE");
    return SATR_ERROR;
  };

  if (!(service_queues.hk_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE HK APP QUEUE");
    return SATR_ERROR;
  };

  if (!(service_queues.test_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE TEST APP QUEUE");
    return SATR_ERROR;
  };

  xTaskCreate((TaskFunction_t)test_app, "test app", 2048, NULL,
              NORMAL_SERVICE_PRIO, NULL);

  xTaskCreate((TaskFunction_t)hk_app, "hk app", 2048, NULL, NORMAL_SERVICE_PRIO,
              NULL);

  return SATR_OK;
}
