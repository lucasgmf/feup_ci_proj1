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

int tcpCreateSocket() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0) {
        printf("[TCP] - Error creating socket\n");
        return -1;
    } else {
        printf("[TCP] - Socket created\n");
    }

    return socketfd;
}

int tcpConnect(int socketfd, char *ip, int port) {
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    // if (inet_aton(ip, &server.sin_addr) == 0) {
    //     printf("[TCP] - Error: Invalid IP address\n");
    //     return -1;
    // }
    if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        printf("[TCP] - Error: Connection to the server failed\n");
        return -1;
    }

    return 1;
}

int tcpDisconnect(int socketfd) {
    return close(socketfd);
}

int tcpSendMBAP(int socketfd, uint8_t *pdu, int pduLen, int id) {
    if (socketfd < 0) {
        printf("[TCP] - Error: Invalid socket file descriptor\n");
        return -1;
    }
    if (pduLen < 0) {
        printf("[TCP] - Error: Invalid PDU length\n");
        return -1;
    }
    if (id < 0) {
        printf("[TCP] - Error: Invalid ID\n");
        return -1;
    }

    // Build MBAPDU header (MBAP = Modbus Application Protocol)
    // unsigned char MBAP[MBAP_SIZE];
    uint8_t MBAP[MBAP_SIZE];

    MBAP[0] = (uint8_t)(id >> 8) & 0xFF;      // Transaction identifier (high byte)
    MBAP[1] = (uint8_t)id & 0xFF;             // Transaction identifier (low byte)
    MBAP[2] = (uint8_t)(0x00 >> 8) & 0xFF;    // Protocol identifier (high byte) - 0x00 = Modbus
    MBAP[3] = (uint8_t)0x00 & 0xFF;           // Protocol identifier (low byte) - 0x00 = Modbus
    MBAP[4] = (uint8_t)(pduLen >> 8) & 0xFF;  // Message length (high byte)
    MBAP[5] = (uint8_t)pduLen & 0xFF;         // Message length (low byte)
    MBAP[6] = (uint8_t)UNIT_ID;               // Unit identifier - Slave ID

    pduLen = pduLen - 1;  // PDU length - unit identifier

    printf("[TCP] - MBAP: ");
    for (int i = 0; i < sizeof(MBAP_SIZE); i++) {
        printf("%02X ", MBAP[i]);
    }
    for (int i = 0; i < pduLen; i++) {
        printf("%02X ", pdu[i]);
    }
    printf("\n");

    // (MBAPDU = MBAP + PDU)
    // Writing MBAP header to socket
    if (write(socketfd, MBAP, MBAP_SIZE) < 0) {
        printf("[TCP] - Error sending MBAP header to socket\n");
        return -1;
    }
    // Writing PDU to socket
    if (write(socketfd, pdu, pduLen) < 0) {
        printf("[TCP] - Error sending PDU to socket\n");
        return -1;
    }

    // uint8_t *MBAPDU = (uint8_t *)malloc((MBAP_SIZE + pduLen) * sizeof(*MBAPDU));
    // if (MBAPDU == NULL) {
    //     printf("[TCP] - Error: Failed to allocate memory\n");
    //     return -1;
    // }
    // memcpy(MBAPDU, MBAP, MBAP_SIZE);
    // memcpy(MBAPDU + MBAP_SIZE, pdu, pduLen);

    // printf("[TCP] - MBAPDU: ");
    // for (int i = 0; i < totalLen; i++) {
    //     printf("%02X ", MBAPDU[i]);
    // }
    // printf("\n");

    // int n = 0, sent = 0;

    // while (n < totalLen) {
    //     n = send(socketfd, MBAPDU + sent, totalLen - sent, 0);
    //     printf("[TCP] - Sent %d bytes\n", n);

    //     if (n < 0) {
    //         printf("[TCP] - Error sending MBAP header to socket\n");
    //         return -1;
    //     }
    //     sent += n;
    // }

    // printf("[TCP] - Finished sending packet with %d bytes\n", sent);
    // free(MBAPDU);
    // return ((MBAP[4] << 8) + MBAP[5]);  // recover Lenght field

    return MBAP_SIZE + pduLen;
}

uint8_t *tcpRecieveMBAP(int socketfd, uint8_t *response, int responseLen) {
    if (socketfd < 0) {
        printf("[TCP] - Error: Invalid socket file descriptor\n");
        return NULL;
    }
    if (responseLen < 0) {
        printf("[TCP] - Error: Invalid response length\n");
        return NULL;
    }

    printf("[TCP] - Starting reciving response\n");

    if (read(socketfd, response, responseLen) > 0) {
        printf("[TCP] - Reading data\n");
        // printf("[TCP] - Error reading response from socket\n");
        // return NULL;
    }

    printf("\n[TCP] - Message received from Slave:");
    for (int i = 0; i < responseLen; i++) {
        printf("%.2X ", response[i]);
    }
    printf("\n");

    return response;
}
