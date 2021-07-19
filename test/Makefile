# makefile for testing obc software

INC=$(addsuffix / ,$(addprefix -I,$(shell find ../ -name 'include' -type d -not -path "../Debug/*")))
INC += -I../main/
CC=gcc -std=c99
SRC=$(shell find test_ex2_obc_software -name "*.c" -type f -not -path "../Debug/*")
LIB=-lm
STATIC_FILES=cgreen/libcgreen.so
CFLAGS=$(INC)
SHELL:=/bin/bash
#entrypoint
MAIN = main
SUT=$(shell ./sut_deps.py)

#---------------------------Build---------------------------
OBJS_FILES = $(patsubst %.c, %.o, $(SRC))

all: $(MAIN)

.PHONY: clean lib


$(MAIN): $(OBJS_FILES) main.o $(SUT)
	$(CC) $(OBJS_FILES) $(INC) main.o -Lcgreen/ -lcgreen $(LIB) -o main

test: all
	export LD_LIBRARY_PATH=$(pwd)cgreen/
	./main

lib:  $(OBJS_FILES)
	ar -rsc file_delivery_app.a $(OBJS_FILES)

clean: 
	find . -type f -name '*.o' -exec rm {} \;
	rm $(MAIN)

