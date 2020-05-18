#ifndef __MY_MODULE_H
#define __MY_MODULE_H
#include "services.h"
// gcc Src/*.c Src/my_module/*.c -c -I Inc/ -I ../upsat-ecss-services/services/ -I Src/ -I Src/my_module -I ../ex2_on_board_computer/Source/include/ -I ../ex2_on_board_computer/Project/ -I ../ex2_on_board_computer/libcsp/include/ -I ../ex2_on_board_computer/Source/portable/GCC/POSIX/ -I ../ex2_on_board_computer/libcsp/build/include/ -m32 -lpthread -std=c99 -lrt && ar -rsc client_server.a *.o^C

SAT_returnState start_service_handlers();

#endif
