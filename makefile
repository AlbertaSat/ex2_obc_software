PROJDIR = $(CURDIR)/../../
#---------------------------FOR FREE RTOS INTEGRATION---------------------------
#path to source includes
INCLUDE += -I$(PROJDIR)/Source/include
#path to compiler includes (portmacro.h)
INCLUDE += -I$(PROJDIR)/Source/portable/GCC/POSIX
#path to FreeRTOSconfig
INCLUDE += -I$(PROJDIR)/Project
#-------------------------------------------------------------------------------
#---------------------------FOR CSP INTEGRATION---------------------------------
#path to source includes
INCLUDE += -I$(PROJDIR)/libcsp/include/csp
INCLUDE += -I$(PROJDIR)/libcsp/include
INCLUDE += -I$(PROJDIR)/libcsp/build/include
INCLUDE += -I$(PROJDIR)/libcsp/src
#-------------------------------------------------------------------------------
#---------------------------------File Names------------------------------------
CFILES += $(CURDIR)/Platform/demo/hal/*.c 
CFILES += $(CURDIR)/Platform/demo/*.c
CFILES += $(CURDIR)/Services/*.c
CFILES += $(CURDIR)/ex2_demo_software/*.c

#-------------------------------------------------------------------------------
#---------------------------------Include Dirs----------------------------------
INCLUDE += -I$(CURDIR)
INCLUDE += -I$(CURDIR)/Platform/demo
INCLUDE += -I$(CURDIR)/Platform/demo/hal
INCLUDE += -I$(CURDIR)/Services
INCLUDE += -I$(CURDIR)/ex2_demo_software
INCLUDE += -I$(CURDIR)/../SatelliteSim/Source/include
INCLUDE += -I$(CURDIR)/../SatelliteSim/Project
INCLUDE += -I$(CURDIR)/../SatelliteSim/libcsp/include
INCLUDE += -I$(CURDIR)/../SatelliteSim/Source/portable/GCC/POSIX
INCLUDE += -I$(CURDIR)/../SatelliteSim/libcsp/build/include/
#-------------------------------------------------------------------------------

#entrypoint
MAIN = main

#---------------------------Compiler Warnings---------------------------
CWARNS += -W
CWARNS += -Wall
#CWARNS += -Werror
CWARNS += -Wextra
CWARNS += -Wformat
CWARNS += -Wmissing-braces
CWARNS += -Wno-cast-align
CWARNS += -Wparentheses
CWARNS += -Wshadow
CWARNS += -Wno-sign-compare
CWARNS += -Wswitch
CWARNS += -Wuninitialized
CWARNS += -Wunknown-pragmas
CWARNS += -Wunused-function
CWARNS += -Wunused-label
#CWARNS += -Wunused-parameter
CWARNS += -Wno-unused-parameter
CWARNS += -Wunused-value
CWARNS += -Wunused-variable
CWARNS += -Wmissing-prototypes

#---------------------------Libs---------------------------
LINKFLAGS = 
LIBS = -pthread
#for building independent of sat sim, archive files:
#STATIC_FILES += $(PROJDIR)/libcsp/build/libcsp.a
#STATIC_FILES += $(PROJDIR)/FreeRtos

#---------------------------Compiler flags---------------------------
CC = gcc 

#32 bit machines, leave this out if your machine is 64 bit
#CFLAGS += -m32
CFLAGS += -m64

CFLAGS += -g -UUSE_STDIO -D__GCC_POSIX__=1
CFLAGS += -pthread
CFLAGS += -DDEBUG=1

# MAX_NUMBER_OF_TASKS = max pthreads used in the POSIX port. 
# Default value is 64 (_POSIX_THREAD_THREADS_MAX), the minimum number required by POSIX.
CFLAGS += -DMAX_NUMBER_OF_TASKS=300
CFLAGS += $(INCLUDE) $(CWARNS)

#---------------------------Build---------------------------
OBJS_FILES = $(patsubst %.c, %.o, $(CFILES))

all: $(MAIN)

.PHONY: clean lib

$(MAIN): $(OBJS_FILES) $(STATIC_FILES)

lib:  $(OBJS_FILES)
	ar -rsc client_server.a $(OBJS_FILES)

clean: 
	rm -f *.o $(MAIN)


