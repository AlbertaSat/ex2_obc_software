// #ifndef __TELECOMMAND_HANDLER__
// #define __TELECOMMAND_HANDLER__
//
// #include "FreeRTOS.h"
// #include <csp/csp.h>
// #include <task.h>
// #include "queue.h"
// #include "telecommand/server.h"
//
// /* TYPEDEFS */
// typedef struct telecommand_t telecommand_t;
//
// struct telecommand_t {
//   char data[250];
//   uint16_t id;
// };
//
// /* TELECOMMAND QUEUE CONFIGURATIONS */
// #define TELECOMMAND_TICKS_TO_WAIT 1
// #define TELECOMMAND_HANDLER_PRIO 5
// #define TELECOMMAND_QUEUE_LEN 16
// #define TELECOMMAND_QUEUE_SIZE (sizeof(telecommand_t *))
//
// /* RTOS HANDLES */
// TaskHandle_t telecommand_handler_task;
// xQueueHandle telecommand_queue;
//
// int start_telecommand_handler();
//
// #endif
