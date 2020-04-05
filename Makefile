# WORKING COMMAND: (This Makefile is a WIP)
#gcc src/telecommand/commands/* src/telecommand/packets/* src/driver_toolkit/* src/systems/ground_station/* src/telecommand/parser/parser.c src/telecommand/parser/interpreter/* src/main.c -c -I ./include -I ../SatelliteSim/Source/include/ -I ../SatelliteSim/Project/ -I ../SatelliteSim/Source/portable/GCC/POSIX/ -m32 -DDEBUG=1 -g -UUSE_STDIO -D__GCC_POSIX__=1 -pthread && ar -rsc telecommands.a *.o


# CC:= gcc
# CFLAGS := -I./include -I../SatelliteSim/Source/include/ -I../SatelliteSim/Project/ -I../SatelliteSim/Source/portable/GCC/POSIX/ -m32 -DDEBUG=1 -g -UUSE_STDIO -D__GCC_POSIX__=1 -pthread

# # MODULES = candy.o chips.o licorice.o cookies.o popcorn.o spinach.o

# MODULES := commands/commands \
# 	driver_toolkit/driver_toolkit \
# 	driver_toolkit/driver_toolkit_lpc \
# 	driver_toolkit/driver_toolkit_nanomind \
# 	drivers/gs_mocks \
# 	ground_station/ground_station \
# 	ground_station/ground_station_lpc_local \
# 	ground_station/ground_station_nanomind \
# 	ground_station/ground_station_uart \
# 	nanomind/script_daemon \
# 	nanomind/telecommand \
# 	nanomind/telecommand_prototype_manager \
# 	nanomind/telecommand_server \
# 	nanomind/telecommand_test \
# 	packets/packet_base \
# 	packes/telemetry_packet_base \
# 	parser/parser \
# 	parser/interpreter/script_expression \
# 	parser/interpreter/statement_expression \
# 	parser/interpreter/telecommand_expression

# SOURCES := src/$(MODULES).c
# OBJS := bin/$(MODULES).o

# all: telecommands.a
# %.o: %.c
# 	$(CC) $(CFLAGS) -c $^ -o $@

# telecommands.a: %.o
# 	ar -rsc $@ $^

# 	ar -rsc telecommands.a src/driver_toolkit_lpc.o src/gs_mocks.o src/telecommand_prototype_manager.o src/telecommand_expression.o src/ground_station_uart.o src/parser.o src/script_daemon.o src/telecommand.o src/driver_toolkit_nanomind.o src/telemetry_packet_base.o src/ground_station_nanomind.o src/packet_base.o src/driver_toolkit.o src/statement_expression.o src/telecommand_test.o src/ground_station_lpc_local.o src/script_expression.o src/ground_station_base.o src/telecommand_server.o

# ar -rsc telecommands.a src/driver_toolkit_lpc.o src/gs_mocks.o src/telecommand_prototype_manager.o src/telecommand_expression.o src/ground_station_uart.o src/parser.o src/script_daemon.o src/telecommand.o src/driver_toolkit_nanomind.o src/telemetry_packet_base.o src/ground_station_nanomind.o src/packet_base.o src/driver_toolkit.o src/statement_expression.o src/telecommand_test.o src/ground_station_lpc_local.o src/script_expression.o src/ground_station_base.o src/telecommand_server.o
CC=gcc
CFLAGS=-m32 -DDEBUG=1 -g -UUSE_STDIO -D__GCC_POSIX__=1 -pthread
INCLUDES=./include ../SatelliteSim/Source/include/ ../SatelliteSim/Project/ ../SatelliteSim/Source/portable/GCC/POSIX/
PROJECT=tele.a

MODULES := commands/commands \
	driver_toolkit/driver_toolkit \
	driver_toolkit/driver_toolkit_lpc \
	driver_toolkit/driver_toolkit_nanomind \
	drivers/gs_mocks \
	ground_station/ground_station \
	ground_station/ground_station_lpc_local \
	ground_station/ground_station_nanomind \
	ground_station/ground_station_uart \
	nanomind/script_daemon \
	nanomind/telecommand \
	nanomind/telecommand_prototype_manager \
	nanomind/telecommand_server \
	nanomind/telecommand_test \
	packets/packet_base \
	packes/telemetry_packet_base \
	parser/parser \
	parser/interpreter/script_expression \
	parser/interpreter/statement_expression \
	parser/interpreter/telecommand_expression

OBJECTS = src/$(MODULES).c


all: $(PROJECT)

.c.o:
	@echo -e "\033[32m -Building "$*"...\033[0m"
	$(CC) -c -I$(INCLUDES) $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	@echo -ne "\033[31m -Creating static library "$*"..."
	@(ar rcs $(PROJECT) $(OBJECTS))
	@echo -e "[DONE] \033[0m"

clean:
	@(rm -rf *.o)

mrproper: clean
	@(rm -rf $(EXEC))
