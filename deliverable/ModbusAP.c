#include "ModbusAP.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ModbusTCP.h"

// #define MAX_APDU 256
#define FC_WMR 16        // function code for write multiple registers
#define FC_RMR 3         // function code for read multiple registers
#define MAX_WR_REGS 123  // maximum number of registers that can be written
#define MAX_RD_REGS 125  // maximum number of registers that can be read

// #define DEBUG

#ifdef DEBUG

#define PRINT(...) printf(__VA_ARGS__)

#else

#define PRINT(...)

#endif

int connectToServer(char* ip, int port) {
    int socketfd = tcpCreateSocket();
    if (socketfd < 0) {
        PRINT("[App] - Error creating socket\n");
        return -1;
    }
    tcpConnect(socketfd, ip, port);

    return socketfd;
}

int disconnectFromServer(int socketfd) { return tcpDisconnect(socketfd); }

int readHoldingRegisters(int socketfd, uint16_t id, uint16_t startingAddress,
                         uint16_t quantity, uint16_t* dataToRead) {
    if (socketfd < 0) {
        PRINT("[App][RHR] - Error: Invalid socket\n");
        return -1;
    }
    if (quantity < MODBUS_REG_QUANTITY_MIN ||
        quantity > MODBUS_REG_QUANTITY_MAX) {
        PRINT("[App][RHR] - Error: Invalid number of registers to read\n");
        return -1;
    }
    if (startingAddress < MODBUS_ADDRESS_MIN ||
        startingAddress > MODBUS_ADDRESS_MAX) {
        PRINT("[App][RHR] - Error: Invalid starting address\n");
        return -1;
    }
    if (startingAddress + quantity > MODBUS_ADDRESS_MAX) {
        PRINT(
            "[App][RHR] - Error: Number of registers to read exceeds "
            "maximum\n");
        return -1;
    }
    if (dataToRead == NULL) {
        PRINT("[App][RHR] - Error: Invalid data pointer\n");
        return -1;
    }

    int apduLen = 5;

    uint8_t* apdu = (uint8_t*)malloc(apduLen * sizeof(apdu));
    if (apdu == NULL) {
        PRINT("[App][RHR] - Error: Failed to allocate memory\n");
        return -1;
    }

    apdu[0] = (uint8_t)(FC_RMR);                  // function code
    apdu[1] = (uint8_t)(startingAddress >> 8);    // start address (high byte)
    apdu[2] = (uint8_t)(startingAddress & 0xFF);  // start address (low byte)
    apdu[3] = (uint8_t)(quantity >> 8);    // number of registers (high byte)
    apdu[4] = (uint8_t)(quantity & 0xFF);  // number of registers (low byte)

    // PRINT("[App][RHR] - apdu: ");
    // printPacket(apdu, apduLen);
    int bytesSent = sendModbusRequest(socketfd, id, apdu, apduLen);
    if (bytesSent < 0) {
        PRINT("[App][RHR] - Error sending request\n");
        return -1;
    }

    int bytesReceived = receiveModbusPacket(socketfd, id, apdu, apduLen);
    if (bytesReceived < 0) {
        PRINT("[App][RHR] - Error receiving response\n");
        return -1;
    }

    // * print data received
    for (int i = 0; i < quantity; i++)
        dataToRead[i] = (apdu[2 + i * 2] << 8) + apdu[3 + i * 2];

    if (apdu[0] & 0x80) {
        PRINT("checkForException: Exception code: %d\n", apdu[1]);
        return apdu[1];
    }

    return 0;
}

int writeMultipleRegisters(int socketfd, uint16_t id, uint16_t startingAddress,
                           uint16_t quantity, uint16_t* data) {
    if (socketfd < 0) {
        PRINT("[App][WMR] - Error: Invalid socket\n");
        return -1;
    }
    if (quantity < MODBUS_REG_QUANTITY_MIN ||
        quantity > MODBUS_REG_QUANTITY_MAX) {
        PRINT("[App][WMR] - Error: Invalid number of registers to read\n");
        return -1;
    }
    if (startingAddress < MODBUS_ADDRESS_MIN ||
        startingAddress > MODBUS_ADDRESS_MAX) {
        PRINT("[App][WMR] - Error: Invalid starting address\n");
        return -1;
    }
    if (startingAddress + quantity > MODBUS_ADDRESS_MAX) {
        PRINT(
            "[App][WMR] - Error: Number of registers to read exceeds "
            "maximum\n");
        return -1;
    }

    int apduLen = quantity * 2 + 6;

    uint8_t* apdu = (uint8_t*)malloc(apduLen * sizeof(apdu));
    if (apdu == NULL) {
        PRINT("[App][WMR] - Error: Failed to allocate memory\n");
        return -1;
    }

    apdu[0] = (uint8_t)(FC_WMR);                  // function code
    apdu[1] = (uint8_t)(startingAddress >> 8);    // start address (high byte)
    apdu[2] = (uint8_t)(startingAddress & 0xFF);  // start address (low byte)
    apdu[3] = (uint8_t)(quantity >> 8);    // number of registers (high byte)
    apdu[4] = (uint8_t)(quantity & 0xFF);  // number of registers (low byte)
    apdu[5] = (uint8_t)(quantity * 2);     // number of bytes to write

    for (int i = 0; i < quantity; i++) {
        apdu[6 + i * 2] = (data[i] >> 8);    // (high byte)
        apdu[7 + i * 2] = (data[i] & 0xFF);  // (low byte)
    }

    if (sendModbusRequest(socketfd, id, apdu, apduLen) < 0) {
        PRINT("[App][WMR] - Error sending request\n");
        return -1;
    }

    if (receiveModbusPacket(socketfd, id, apdu, apduLen) < 0) {
        PRINT("[App][WMR] - Error recieving response\n");
        return -1;
    }

    // * return data received
    // memcpy(data, apdu, apduLen);

    if (apdu[0] & 0x80) {
        PRINT("checkForException: Exception code: %d\n", apdu[1]);
        return apdu[1];
    }
    // *rlen = apduLen;
    return 0;
}

#undef PRINT
#undef DEBUG