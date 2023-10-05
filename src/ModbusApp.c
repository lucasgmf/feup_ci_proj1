#include "../include/ModbusApp.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

//! Magia professor sabe muito
#define MAX_APDU 256
#define FC_WMR 16        // function code for write multiple registers
#define FC_RMR 3         // function code for read multiple registers
#define MAX_WR_REGS 123  // maximum number of registers that can be written
#define MAX_RD_REGS 125  // maximum number of registers that can be read

uint8_t* readHoldingRegisters(int socketfd, uint16_t startingAddress, uint16_t numberOfRegisters, int* rlen) {
    // check input parameters
    {
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
    }
    int len, sent, id;

    uint8_t* pdu = (uint8_t*)malloc(sizeof(uint8_t) * MAX_APDU);
    if (pdu == NULL) {
        printf("[App][RHR] - Error: Failed to allocate memory\n");
        return NULL;
    }

    pdu[0] = (uint8_t)FC_WMR;                      // function code
    pdu[1] = (uint8_t)(startingAddress >> 8);      // start address (high byte)
    pdu[2] = (uint8_t)(startingAddress & 0xFF);    // start address (low byte)
    pdu[3] = (uint8_t)(numberOfRegisters >> 8);    // number of registers (high byte)
    pdu[4] = (uint8_t)(numberOfRegisters & 0xFF);  // number of registers (low byte)
    pdu[5] = (uint8_t)(numberOfRegisters * 2);     // number of bytes // each register has 2 bytes // 16

    id = 0x03; // function code for read multiple registers
    sendModbusReq(SERVER_IP, SERVER_PORT, pdu, sizeof(pdu),);
}

// void APDUprint(uint8_t* APDU, uint16_t APDUlen) {
//     printf("[App] - APDU: ");
//     for (int i = 0; i < APDUlen; i++) {
//         printf("%02X ", APDU[i]);
//     }
//     printf("\n");
// }

// int writeMultipleRegisters(int socketfd, uint16_t startingAdress, uint16_t numberOfRegisters, int* rlen) {
// int fd;  // File descriptor
// struct sockaddr_in server;

// // Create socket
// fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// if (fd < 0) {
//     printf("[App] - Error creating socket\n");
//     return -1;
// } else {
//     printf("[App] - Socket created\n");
// }

// server.sin_family = AF_INET;
// server.sin_addr.s_addr = inet_addr(ip);
// server.sin_port = htons(port);

// // Connect to server
// if (connect(fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
//     printf("[App] - Error connecting to server\n");
//     close(fd);
//     return -1;
// } else {
//     printf("[App] - Connected to server\n");
// }

// uint8_t APDU[MAX_APDU];

// if (numberOfRegisters > MAX_WR_REGS) {
//     printf("[App][WMR] - Error: Number of registers to write exceeds maximum\n");
//     return -1;
// }

// if (numberOfRegisters < 1) {
//     printf("[App][WMR] - Error: Number of registers to write must be greater than 0\n");
//     return -1;
// }

// if (value == NULL) {
//     printf("[App][WMR] - Error: Invalid value\n");
//     return -1;
// }

// if (startRegister < 0) {
//     printf("[App][WMR] - Error: Invalid start register\n");
//     return -1;
// }

// if (startRegister + numberOfRegisters > 65535) {
//     printf("[App][WMR] - Error: Number of registers to write exceeds maximum\n");
//     return -1;
// }

// // Building MBAPDU header (MBAP = Modbus Application Protocol)
// APDU[0] = (uint8_t)FC_WMR;                      // function code
// APDU[1] = (uint8_t)(startRegister >> 8);        // start address (high byte)
// APDU[2] = (uint8_t)(startRegister & 0xFF);      // start address (low byte)
// APDU[3] = (uint8_t)(numberOfRegisters >> 8);    // number of registers (high byte)
// APDU[4] = (uint8_t)(numberOfRegisters & 0xFF);  // number of registers (low byte)
// APDU[5] = (uint8_t)(numberOfRegisters * 2);     // number of bytes // each register has 2 bytes // 16 bits

// // building the data section of the message
// for (int i = 0; i < numberOfRegisters; i++) {
//     APDU[6 + i * 2] = (uint8_t)(value[i] >> 8);    // value (high byte)
//     APDU[7 + i * 2] = (uint8_t)(value[i] & 0xFF);  // value (low byte)
// }

// uint16_t APDULen = 6 + numberOfRegisters * 2;  // APDU length
// APDUprint(APDU, APDULen);

// // Writing APDU request to socket
// if (sendModbusReq(SERVER_IP, SERVER_PORT, APDU, APDULen, APDU, APDULen) < 0) {
//     printf("[App][WMR] - Error: Failed to send Modbus request\n");
//     return -1;
// }

// if (APDU[0] & 0x80) {
//     printf("[App][WMR] - Error: Modbus exception error %d\n", APDU[1]);
//     return APDU[1];
// }
// return 1;
// }
