#ifndef _MODBUS_APP_H_
#define _MODBUS_APP_H_

#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "ModbusTCP.h"

#define MODBUS_ADDRESS_MIN 0x0000       // 0
#define MODBUS_ADDRESS_MAX 0xFFFF       // 65535
#define MODBUS_REG_QUANTITY_MIN 0x0001  // 1
#define MODBUS_REG_QUANTITY_MAX 0x007D  // 125

#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0

int connectToServer(char* ip, int port);
int disconnectFromServer(int socketfd);
int readHoldingRegisters(int socketfd, uint16_t id, uint16_t startingAddress,
                         uint16_t quantity, uint8_t* dataToRead);

int writeMultipleRegisters(int socketfd, uint16_t id, uint16_t startingAddress,
                           uint16_t quantity, uint16_t* data);
#endif  // _MODBUS_APP_H_