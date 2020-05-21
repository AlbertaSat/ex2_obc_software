#ifndef __MY_MODULE_H
#define __MY_MODULE_H
#include <FreeRTOS.h>
#include <csp/csp.h>

#include "queue.h"
#include "services.h"

// gcc Src/*.c Src/my_module/*.c -c -I Inc/ -I ../upsat-ecss-services/services/
// -I Src/ -I Src/my_module -I ../ex2_on_board_computer/Source/include/ -I
// ../ex2_on_board_computer/Project/ -I ../ex2_on_board_computer/libcsp/include/
// -I ../ex2_on_board_computer/Source/portable/GCC/POSIX/ -I
// ../ex2_on_board_computer/libcsp/build/include/ -m32 -lpthread -std=c99 -lrt
// && ar -rsc client_server.a *.o^C
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
  xQueueHandle time_management_app_queue;
} service_queues_t;

SAT_returnState start_service_handlers();

#endif
