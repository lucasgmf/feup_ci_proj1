# -*- Makefile -*-
# Makefile to build the project
# NOTE: This file must not be changed.

# Parameters
CC = gcc

DEBUG_LEVEL=2

# _DEBUG is used to include internal logging of errors and general information. Levels go from 1 to 3, highest to lowest priority respectively
# _PRINT_PACKET_DATA is used to print the packet data that is received by RX
CFLAGS = -Wall -Wno-unknown-pragmas -Wno-implicit-function-declaration -Wno-unused-variable -g -D _DEBUG=$(DEBUG_LEVEL)

SRC = src
INCLUDE = include
BIN = bin
MONITORS = monitors

APP = main.c
BUILDEXTENS = exe

# MB Client monitor
MB_CLIENT_MONITOR = modpoll

# MB Slave monitor
MB_SLAVE_MONITOR = diagslave

# SerialPort
SERIALPORT = COM1

# Targets
.PHONY: all
all: $(BIN)/app.$(BUILDEXTENS)

$(BIN)/app.$(BUILDEXTENS): $(APP) $(SRC)/*.c
	$(CC) $(CFLAGS) -o $@ $^ -I$(INCLUDE) -lrt

.PHONY: slave
slave:
	sudo ./$(MONITORS)/$(MB_SLAVE_MONITOR).$(BUILDEXTENS)  -m tcp -p 502

.PHONY: client
client:
	sudo ./$(MONITORS)/$(MB_CLIENT_MONITOR).$(BUILDEXTENS) -m tcp 127.0.0.1 

.PHONY: run
run:
	./$(BIN)/app.$(BUILDEXTENS)

.PHONY: clean
clean:
	rm -f $(BIN)/*
	clear

.PHONY: tcptest
tcptest:
	$(CC) $(CFLAGS) -o $(BIN)/tcptest.$(BUILDEXTENS) $(SRC)/ModbusTCP/ModbusTCP.c -I$(INCLUDE) -lrt

