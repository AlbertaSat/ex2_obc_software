#include "FreeRTOS.h"
#include "stdio.h"
#include "telecommand/telecommand_handler.h"

static void telecommand_handler(void *parameters) {
  telecommand_t incoming_telecommand;
  for (;;) {
    if (xQueueReceive(telecommand_queue, (void *)&incoming_telecommand,
                      TELECOMMAND_TICKS_TO_WAIT) == pdPASS) {
      printf("RX: %s, ID: %d\n", (char *)incoming_telecommand.data,
             incoming_telecommand.id);
    }
  }
}

SAT_returnState start_service_handler() {
  telecommand_queue =
      xQueueCreate((unsigned portBASE_TYPE)TELECOMMAND_QUEUE_LEN,
                   (unsigned portBASE_TYPE)TELECOMMAND_QUEUE_SIZE);

  if (!telecommand_queue) {
    printf("FAILED TO CREATE QUEUE");
    return SATR_ERROR;
  }

  xTaskCreate((TaskFunction_t)telecommand_handler, "telecommand handler", 2048,
              NULL, TELECOMMAND_HANDLER_PRIO, &telecommand_handler_task);

  return SATR_OK;
}
