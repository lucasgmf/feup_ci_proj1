#include "ModbusTCP.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MBAP_SIZE 7
#define UNIT_ID 0x01  // Slave ID

// TODO definir limite de registros para escrever
// TODO escolher porta do servidor
// TODO escolher unit id
// TODO escolher registros a manipular

// TODO free memory

void printPacket(uint8_t *packet, int packetLen) {
    // printf("----------------------------------------------\n");
    // printf("packet: ");
    for (int i = 0; i < packetLen; i++) {
        printf("%02X ", packet[i]);
    }
    printf("\n");
}

int tcpCreateSocket() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        printf("[TCP] - Error creating socket\n");
        return -1;
    }

    return socketfd;
}

int tcpConnect(int socketfd, char *ip, int port) {
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        printf("[TCP] - Error: Connection to the server failed\n");
        return -1;
    }
    return 1;
}

int tcpDisconnect(int socketfd) {
    return close(socketfd);
}

int sendModbusRequest(int socketfd, uint16_t id, uint8_t *apdu, int apduLen) {
    uint8_t *mbap = (uint8_t *)malloc(MBAP_SIZE * sizeof(mbap));
    if (mbap == NULL) {
        printf("[TCP][SMR] - Error: Failed to allocate memory\n");
        return -1;
    }

    // printf("apduLen %d\n", apduLen);

    // id++;
    mbap[0] = (uint8_t)id >> 8;               // Transaction Identifier
    mbap[1] = (uint8_t)id & 0xFF;             // Transaction Identifier
    mbap[2] = (uint8_t)0x00;                  // Protocol Identifier
    mbap[3] = (uint8_t)0x00;                  // Protocol Identifier
    mbap[4] = (uint8_t)(apduLen + 1) >> 8;    // Length
    mbap[5] = (uint8_t)(apduLen + 1) & 0xFF;  // Length
    mbap[6] = (uint8_t)0x01;                  // Unit Identifier

    int pduLen = apduLen;

    uint8_t *packet = (uint8_t *)malloc((MBAP_SIZE + pduLen) * sizeof(packet));
    if (packet == NULL) {
        printf("[TCP][SMR] - Error: Failed to allocate memory\n");
        return -1;
    }

    // printf("pduLen: %d\n", pduLen);
    memcpy(packet, mbap, MBAP_SIZE);
    memcpy(packet + MBAP_SIZE, apdu, apduLen);

    printf("[TCP][SMR] - Sending pdu: ");
    printPacket(packet, pduLen + MBAP_SIZE);

    int bytesSent = sendModbusPacket(socketfd, packet, pduLen + MBAP_SIZE);
    if (bytesSent < 0) {
        printf("[TCP][SMR] - Error sending request\n");
        return -1;
    }

    // TODO check recieved data
    uint8_t *mbapResponse = (uint8_t *)malloc(MBAP_SIZE * sizeof(mbapResponse));
    if (mbapResponse == NULL) {
        printf("[TCP][SMR] - Error: Failed to allocate memory\n");
        return -1;
    }
    int bytesRecieved = recieveModbusPacket(socketfd, mbapResponse, MBAP_SIZE);
    if (bytesRecieved < 0) {
        printf("[TCP][SMR] - Error recieving response\n");
        return -1;
    }

    uint16_t apduResponseLen = ((mbapResponse[4] << 8) | mbapResponse[5]) - 1;

    // printf("[TCP][SMR] - apduResponseLen: %d\n", apduResponseLen);

    uint8_t *apduResponse = (uint8_t *)malloc(apduResponseLen);
    if (apduResponse == NULL) {
        printf("[TCP][SMR] - Error: Failed to allocate memory\n");
        return -1;
    }
    printf("[TCP][SMR] - mbapResponse recieved: ");
    printPacket(mbapResponse, MBAP_SIZE);

    bytesRecieved = recieveModbusPacket(socketfd, apduResponse, apduResponseLen);
    if (bytesRecieved < 0) {
        printf("[TCP][SMR] - Error recieving response\n");
        return -1;
    }

    memcpy(apdu, apduResponse, apduResponseLen);
    printf("[TCP][SMR] - apdu recieved: ");
    printPacket(apdu, apduResponseLen);

    // printf("[TCP][SMR] - apduResponseLen: %d\n", apduResponseLen);

    free(mbap);
    printf("[TCP][SMR] - apduResponseLen: %d\n", apduResponseLen);

    // memcpy(apduResponse, apdu, apduResponseLen);

    return 0;
}

int sendModbusPacket(int socketfd, u_int8_t *packet, int responseLen) {
    if (socketfd < 0) {
        printf("[TCP] - Error: Invalid socket file descriptor\n");
        return -1;
    }
    if (responseLen < 0) {
        printf("[TCP] - Error: Invalid response length\n");
        return -1;
    }
    int bytesSent = 0, k = 0;
    while (bytesSent < responseLen) {
        k = send(socketfd, packet + bytesSent, responseLen - bytesSent, 0);
        if (k < 0) {
            printf("[TCP] - Error sending response\n");
            return -1;
        }
        bytesSent += k;
    }
    return bytesSent;
}

uint8_t recieveModbusPacket(int socketfd, u_int8_t *packet, int sizePacket) {
    // TODO check parameters
    int bytesRecieved = 0;

    bytesRecieved = recv(socketfd, packet, sizePacket, 0);
    if (bytesRecieved < 0) {
        printf("[TCP] - Error recieving response\n");
        return -1;
    }
    return bytesRecieved;
}