# Makefile for testing obc software	
# Last Edited by Ben Fisher (bsfisher@ualberta.ca) on August 17, 2022

# -------------------------- USER SPECIFIC MANUALLY SET VARIABLES ----------------------------
# Set to location of cgreen directory ie.(/home/usr/.../cgreen)
LCGREEN = /home/usr/cgreen


#------------- Global Variables -------------------
SUT =
SRC =
INC =

CC=gcc -std=c99

LIB=-Wl,--start-group
LIB+=-lm
LIB+=-lc


# ------------ Globally Shared Files ---------------

INC += -I$(LCGREEN)/include/
INC += -I../include/
INC += -I../ex2_system/include/
INC += -I../main/
INC += -I../reliance_edge/include/

SRC += ./test_ex2_obc_software/fake-freeRTOS.c

# For Leop and Logger Testing
INC += -I./test_ex2_obc_software/ex2_system/include/


# ---------- Test Dependant Files ------------------
#Logger Testing
SUT += ./../../ex2_obc_software/ex2_system/source/logger/logger.c
SRC += ./test_ex2_obc_software/ex2_system/source/logger/test_logger.c
	

#ADCS_Handler Testing
#SUT += ../../ex2_obc_software/ex2_hal/adcs/equipment_handler/src/adcs_handler.c
#SRC += ./test_ex2_obc_software/ex2_hal/adcs/equipment_handler/src/test_adcs_handler.c
#INC += -I./test_ex2_obc_software/ex2_hal/adcs/equipment_handler/inc/
#INC += -I../ex2_hal/adcs/equipment_handler/inc/

#Leop Testing
SUT += ../../ex2_obc_software/ex2_system/source/leop/leop.c
SRC += ./test_ex2_obc_software/ex2_system/source/leop/test_leop.c
INC += -I../ex2_hal/athena/equipment_handler/include/
INC += -I../F021_API/


#entrypoint
MAIN = main


CFLAGS=$(INC)


#---------------------------Build---------------------------
OBJS_FILES = $(patsubst %.c, %.o, $(SRC))


all: $(MAIN)

.PHONY: clean lib


$(MAIN): $(OBJS_FILES) main.o $(SUT)
	$(CC) $(OBJS_FILES) $(CFLAGS) main.o $(LIB) -L$(LCGREEN)/build/src -lcgreen -Wl,--end-group -o main

test: all
	LD_LIBRARY_PATH="$(LCGREEN)/build/src" ./main

lib:  $(OBJS_FILES)
	ar -rsc file_delivery_app.a $(OBJS_FILES)

clean:
	find . -type f -name '*.o' -exec rm {} \;
	rm $(MAIN)
