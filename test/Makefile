# makefile for testing obc software

INC=$(addsuffix / ,$(addprefix -I,$(shell find ../ -name 'include' -type d -not -path "../Debug/*")))
INC+=$(addsuffix / ,$(addprefix -I,$(shell find ../ -name 'inc' -type d)))
INC += -I../main/
INC += -I../
CC=gcc -std=c99
SRC=$(shell find test_ex2_obc_software -name "*.c")

LIB=-Wl,--start-group
LIB+=-lm
LIB+=-lc

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
	$(CC) $(OBJS_FILES) $(CFLAGS) main.o $(LIB) -Lcgreen/ -lcgreen -Wl,--end-group -o main

test: all
	./main

lib:  $(OBJS_FILES)
	ar -rsc file_delivery_app.a $(OBJS_FILES)

clean:
	find . -type f -name '*.o' -exec rm {} \;
	rm $(MAIN)
