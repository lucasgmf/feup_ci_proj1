#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "ModbusTCP.h"

#define MODBUS_ADDRESS_MIN 0x0000       // 0
#define MODBUS_ADDRESS_MAX 0xFFFF       // 65535
#define MODBUS_REG_QUANTITY_MIN 0x0001  // 1
#define MODBUS_REG_QUANTITY_MAX 0x007D  // 125

#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0

void APDUprint(uint8_t* APDU, uint16_t APDUlen);

int writeMultipleRegisters(int socketfd, uint16_t startingAdress, uint16_t numberOfRegisters, int* rlen);
