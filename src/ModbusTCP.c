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

int tcpCreateSocket() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    // printf("[TCPcreate] - Socket: %d\n", socketfd);
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
    printf("[TCP] - socketfd: %d, pduLen: %d, id: %d\n", socketfd, pduLen, id);
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

    MBAP[0] = (uint8_t)(id >> 8) & 0xFF;            // Transaction identifier (high byte)
    MBAP[1] = (uint8_t)id & 0xFF;                   // Transaction identifier (low byte)
    MBAP[2] = (uint8_t)(0x00 >> 8) & 0xFF;          // Protocol identifier (high byte) - 0x00 = Modbus
    MBAP[3] = (uint8_t)0x00 & 0xFF;                 // Protocol identifier (low byte) - 0x00 = Modbus
    MBAP[4] = (uint8_t)((pduLen + 1) >> 8) & 0xFF;  // Message length (high byte)
    MBAP[5] = (uint8_t)(pduLen + 1) & 0xFF;         // Message length (low byte)
    MBAP[6] = (uint8_t)UNIT_ID;                     // Unit identifier - Slave ID

    // pduLen += 1;
    // pduLen = pduLen - 1;  // PDU length - unit identifier

    printf("[TCP] - MBAP: ");
    for (int i = 0; i < MBAP_SIZE; i++) {
        printf("%02X ", MBAP[i]);
    }
    for (int i = 0; i < pduLen; i++) {
        printf("%02X ", pdu[i]);
    }
    printf("\n");

    // (MBAPDU = MBAP + PDU)
    // Writing MBAP header to socket

    int sizePacket = MBAP_SIZE + pduLen;
    // printf("[TCP] - sizePacket: %d\n", sizePacket);

    uint8_t *packet = (uint8_t *)malloc(sizePacket * sizeof(packet));
    memcpy(packet, MBAP, MBAP_SIZE);
    memcpy(packet + MBAP_SIZE, pdu, pduLen);

    printf("[TCP] - packet: ");
    for (int i = 0; i < sizePacket; i++) {
        printf("%02X ", packet[i]);
    }
    printf("\n");

    int bytesSent = sendModbusPacket(socketfd, packet, sizePacket);
    printf("[TCP] - bytesSent: %d\n", bytesSent);

    if (bytesSent < 0) {
        printf("[TCP] - Error sending MBAPDU\n");
        return -1;
    }
    // free(packet);

    int bytesRecieved = recieveModbusPacket(socketfd, packet, sizePacket);
    if (bytesRecieved < 0) {
        printf("[TCP] - Error recieving MBAPDU from server\n");
        return -1;
    }
    printf("[TCP] - packet: ");
    for (int i = 0; i < sizePacket; i++) {
        printf("%02X ", packet[i]);
    }
    printf("[TCP] - Size of data: %d\n", (packet[4] >> 8) + packet[5] - 1);
    printf("[TCP] - bytesRecieved: %d\n", bytesRecieved);

    

    return 1;
}

int sendModbusPacket(int socketfd, u_int8_t *packet, int responseLen) {
    printf("[TCP] - socketfd: %d,responseLen: %d\n", socketfd, responseLen);
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
        // printf("[TCP] - bytesSent: %d, k: %d\n", bytesSent, k);
        if (k < 0) {
            printf("[TCP] - Error sending response\n");
            return -1;
        }
        printf("[TCP] - Sent %d bytes of data\n", k - 7);
        bytesSent += k;
    }
}

int recieveModbusPacket(int socketfd, u_int8_t *packet, int sizePacket) {
    // TODO check parameters
    int bytesRecieved = recv(socketfd, packet, sizePacket, 0);
    if (bytesRecieved < 0) {
        printf("[TCP] - Error recieving response\n");
        return -1;
    }
    printf("[TCP] - Recieved %d bytes of data\n", bytesRecieved);
    return bytesRecieved;
}