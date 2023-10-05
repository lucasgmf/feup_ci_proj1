#include "ModbusApp.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ModbusTCP.h"

#define MBAP_SIZE 7

// #define MAX_APDU 256
#define FC_WMR 16        // function code for write multiple registers
#define FC_RMR 3         // function code for read multiple registers
#define MAX_WR_REGS 123  // maximum number of registers that can be written
#define MAX_RD_REGS 125  // maximum number of registers that can be read

int connectToServer(char* ip, int port) {
    int socketfd = tcpCreateSocket();
    if (socketfd < 0) {
        printf("[App] - Error creating socket\n");
        return -1;
    }

    tcpConnect(socketfd, ip, port);
    return 1;
}

int disconnectFromServer(int socketfd) {
    return tcpDisconnect(socketfd);
}

uint16_t* readHoldingRegisters(int socketfd, uint16_t startingAddress, uint16_t numberOfRegisters, int* registerLen) {
    if (socketfd < 0) {
        printf("[App][RHR] - Error: Invalid socket\n");
        return NULL;
    }
    if (numberOfRegisters < MODBUS_REG_QUANTITY_MIN || numberOfRegisters > MODBUS_REG_QUANTITY_MAX) {
        printf("[App][RHR] - Error: Invalid number of registers to read\n");
        return NULL;
    }
    if (startingAddress < MODBUS_ADDRESS_MIN || startingAddress > MODBUS_ADDRESS_MAX) {
        printf("[App][RHR] - Error: Invalid starting address\n");
        return NULL;
    }
    if (startingAddress + numberOfRegisters > MODBUS_ADDRESS_MAX) {
        printf("[App][RHR] - Error: Number of registers to read exceeds maximum\n");
        return NULL;
    }
    if (registerLen == NULL) {
        printf("[App][RHR] - Error: Invalid length pointer\n");
        return NULL;
    }

    // *registerLen = 5;  // 1 byte for function code + 2 bytes for starting address + 2 bytes for number of registers

    int pduLen = 5;

    uint8_t* pdu = (uint8_t*)malloc(pduLen);
    if (pdu == NULL) {
        printf("[App][RHR] - Error: Failed to allocate memory\n");
        return NULL;
    }

    pdu[0] = (uint8_t)FC_RMR;                    // function code
    pdu[1] = (uint8_t)startingAddress >> 8;      // start address (high byte)
    pdu[2] = (uint8_t)startingAddress & 0xFF;    // start address (low byte)
    pdu[3] = (uint8_t)numberOfRegisters >> 8;    // number of registers (high byte)
    pdu[4] = (uint8_t)numberOfRegisters & 0xFF;  // number of registers (low byte)

    printf("[App][RHR] - pdu: ");
    for (int i = 0; i < pduLen; i++) {
        printf("%02X ", pdu[i]);
    }
    printf("\n");

    int id = 0x03;  // function code for read multiple registers

    if (tcpSendMBAP(socketfd, pdu, pduLen, id) < 0) {
        printf("[App][RHR] - Error: Failed to send MBAPDU\n");
        return NULL;
    }

    free(pdu);

    uint8_t* response = (uint8_t*)malloc(pduLen + MBAP_SIZE);
    if (response == NULL) {
        printf("[App][RHR] - Error: Failed to allocate memory\n");
        return NULL;
    }

    printf("[App][RHR] - Starting reciving response\n");
    response = tcpRecieveMBAP(socketfd, response, *registerLen);
    printf("[App][RHR] - Recieving response ended.\n");

    if (response == NULL) {
        printf("[App][RHR] - Error: Failed to recieve response\n");
        return NULL;
    }
    *registerLen = pduLen + MBAP_SIZE;

    return (uint16_t*)response;
}
