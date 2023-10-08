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
    }  // printf("[App] - Socket: %d\n", socketfd);
    tcpConnect(socketfd, ip, port);

    return socketfd;
}

int disconnectFromServer(int socketfd) {
    return tcpDisconnect(socketfd);
}

uint8_t* readHoldingRegisters(int socketfd, uint16_t startingAddress, uint16_t numberOfRegisters, int* registerLen) {
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

    int apduLen = 5;

    uint8_t* apdu = (uint8_t*)malloc(apduLen);
    if (apdu == NULL) {
        printf("[App][RHR] - Error: Failed to allocate memory\n");
        return NULL;
    }

    apdu[0] = (uint8_t)FC_RMR;                    // function code
    apdu[1] = (uint8_t)startingAddress >> 8;      // start address (high byte)
    apdu[2] = (uint8_t)startingAddress & 0xFF;    // start address (low byte)
    apdu[3] = (uint8_t)numberOfRegisters >> 8;    // number of registers (high byte)
    apdu[4] = (uint8_t)numberOfRegisters & 0xFF;  // number of registers (low byte)

    // printf("[App][RHR] - apdu to send: ");
    // printPacket(apdu, apduLen);

    int bytesRecieved = sendModbusRequest(socketfd, FC_RMR, apdu, apduLen);
    if (bytesRecieved < 0) {
        printf("[App][RHR] - Error sending request\n");
        return NULL;
    }
    // printf("[App][RHR] - recieved %d bytes\n", bytesRecieved);
    // printf("[App][RHR] - apdu recieved: ");
    // printPacket(apdu, apduLen);

    // free(apdu);
    *registerLen = apduLen;
    return apdu;
}

uint8_t* writeMultipleRegisters(int socketfd, uint16_t startingAddress, uint16_t numberOfRegisters, int* registerLen, uint16_t* data) {
    if (socketfd < 0) {
        printf("[App][WMR] - Error: Invalid socket\n");
        return NULL;
    }
    if (numberOfRegisters < MODBUS_REG_QUANTITY_MIN || numberOfRegisters > MODBUS_REG_QUANTITY_MAX) {
        printf("[App][WMR] - Error: Invalid number of registers to read\n");
        return NULL;
    }
    if (startingAddress < MODBUS_ADDRESS_MIN || startingAddress > MODBUS_ADDRESS_MAX) {
        printf("[App][WMR] - Error: Invalid starting address\n");
        return NULL;
    }
    if (startingAddress + numberOfRegisters > MODBUS_ADDRESS_MAX) {
        printf("[App][WMR] - Error: Number of registers to read exceeds maximum\n");
        return NULL;
    }
    if (registerLen == NULL) {
        printf("[App][WMR] - Error: Invalid length pointer\n");
        return NULL;
    }

    int apduLen = numberOfRegisters * 2 + 6;

    uint8_t* apdu = (uint8_t*)malloc(apduLen * sizeof(apdu));
    if (apdu == NULL) {
        printf("[App][WMR] - Error: Failed to allocate memory\n");
        return NULL;
    }

    apdu[0] = (uint8_t)FC_WMR;                    // function code
    apdu[1] = (uint8_t)startingAddress >> 8;      // start address (high byte)
    apdu[2] = (uint8_t)startingAddress & 0xFF;    // start address (low byte)
    apdu[3] = (uint8_t)numberOfRegisters >> 8;    // number of registers (high byte)
    apdu[4] = (uint8_t)numberOfRegisters & 0xFF;  // number of registers (low byte)
    apdu[5] = (uint8_t)numberOfRegisters * 2;     // number of bytes to write

    for (int i = 0; i < numberOfRegisters; i++) {
        apdu[6 + i * 2] = data[i] >> 8;    // (high byte)
        apdu[7 + i * 2] = data[i] & 0xFF;  // (low byte)
    }
    uint16_t id = FC_WMR;
    int bytesRecieved = sendModbusRequest(socketfd, id, apdu, apduLen);
    if (bytesRecieved < 0) {
        printf("[App][WMR] - Error sending request\n");
        return NULL;
    }
    // printf("[App][WMR] - recieved %d bytes\n", bytesRecieved);
    // printf("[App][WMR] - apdu recieved: ");
    printPacket(apdu, apduLen);

    *registerLen = apduLen;

    return apdu;
}
