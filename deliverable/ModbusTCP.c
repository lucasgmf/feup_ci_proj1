#include "ModbusTCP.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MBAP_SIZE 7
#define UNIT_ID 53

#define TIMEOUT_SECONDS 2
#define TIMEOUT_MICROSECONDS 0

// #define DEBUG

#ifdef DEBUG

#define PRINT(...) printf(__VA_ARGS__)

#else

#define PRINT(...)

#endif

void printPacket(uint8_t *packet, int packetLen) {
    for (int i = 0; i < packetLen; i++) {
        PRINT("%02X ", packet[i]);
    }
    PRINT("\n");
}

int tcpCreateSocket() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        PRINT("[TCP] - Error creating socket\n");
        return -1;
    }

    //* set timeout
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SECONDS;
    timeout.tv_usec = TIMEOUT_MICROSECONDS;

    if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                   sizeof(timeout)) < 0) {
        return -2;
    }
    int optval = 1;

    if (setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &optval,
                   sizeof(optval)) < 0) {
        return -2;
    }

    return socketfd;
}

int tcpConnect(int socketfd, char *ip, int port) {
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    if (connect(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        PRINT("[TCP] - Error: Connection to the server failed\n");
        return -1;
    }
    return 1;
}

int tcpDisconnect(int socketfd) { return close(socketfd); }

int receivePacket(int socketfd, u_int8_t *packet, int sizePacket) {
    if (socketfd < 0) {
        PRINT("[TCP] - Error: Invalid socket file descriptor\n");
        return -1;
    }
    if (sizePacket < 0) {
        PRINT("[TCP] - Error: Invalid packet size\n");
        return -1;
    }

    int bytesReceived = 0;

    bytesReceived = recv(socketfd, packet, sizePacket, 0);
    if (bytesReceived < 0) {
        PRINT("[TCP] - Error recieving response\n");
        return -1;
    }
    return bytesReceived;
}

int receiveModbusPacket(int socketfd, uint8_t id, uint8_t *apdu, int apduLen) {
    uint8_t *mbapHeader = (uint8_t *)malloc(MBAP_SIZE * sizeof(mbapHeader));
    if (mbapHeader == NULL) {
        PRINT("[TCP][RMP] - Error: Failed to allocate memory\n");
        return -1;
    }

    int bytesReceived = receivePacket(socketfd, mbapHeader, MBAP_SIZE);
    if (bytesReceived < 0) {
        PRINT("[TCP][RMP] - Error recieving response\n");
        return -1;
    }
    uint16_t apduResponseLen = ((mbapHeader[4] << 8) | mbapHeader[5]) - 1;

    uint8_t *apduResponse = (uint8_t *)malloc(apduResponseLen);
    if (apduResponse == NULL) {
        PRINT("[TCP][RMP] - Error: Failed to allocate memory\n");
        return -1;
    }

    bytesReceived = receivePacket(socketfd, apduResponse, apduResponseLen);
    if (bytesReceived < 0) {
        PRINT("[TCP][RMP] - Error recieving response\n");
        return -1;
    }

    // * Copying apllication data response to apdu
    memcpy(apdu, apduResponse, apduResponseLen);
    // apduLen = apduResponseLen;
    // PRINT("rlen1: %d\n", *rlen);

    return 0;
}

int sendModbusPacket(int socketfd, u_int8_t *packet, int responseLen) {
    if (socketfd < 0) {
        PRINT("[TCP] - Error: Invalid socket file descriptor\n");
        return -1;
    }
    if (responseLen < 0) {
        PRINT("[TCP] - Error: Invalid response length\n");
        return -1;
    }
    int bytesSent = 0, k = 0;
    while (bytesSent < responseLen) {
        k = send(socketfd, packet + bytesSent, responseLen - bytesSent, 0);
        if (k < 0) {
            PRINT("[TCP] - Error sending response\n");
            return -1;
        }
        bytesSent += k;
    }
    return bytesSent;
}

int sendModbusRequest(int socketfd, uint16_t id, uint8_t *apdu, int apduLen) {
    uint8_t *mbapHeader = (uint8_t *)malloc(MBAP_SIZE * sizeof(mbapHeader));
    if (mbapHeader == NULL) {
        PRINT("[TCP][SMR] - Error: Failed to allocate memory\n");
        return -1;
    }

    // id++;
    mbapHeader[0] = (uint8_t)(id >> 8);               // Transaction Identifier
    mbapHeader[1] = (uint8_t)(id & 0xFF);             // Transaction Identifier
    mbapHeader[2] = (uint8_t)(0x00);                  // Protocol Identifier
    mbapHeader[3] = (uint8_t)(0x00);                  // Protocol Identifier
    mbapHeader[4] = (uint8_t)((apduLen + 1) >> 8);    // Length
    mbapHeader[5] = (uint8_t)((apduLen + 1) & 0xFF);  // Length
    mbapHeader[6] = (uint8_t)(UNIT_ID);               // Unit Identifier

    int pduLen = apduLen;

    uint8_t *packet = (uint8_t *)malloc((MBAP_SIZE + pduLen) * sizeof(packet));
    if (packet == NULL) {
        PRINT("[TCP][SMR] - Error: Failed to allocate memory\n");
        return -1;
    }

    memcpy(packet, mbapHeader, MBAP_SIZE);
    memcpy(packet + MBAP_SIZE, apdu, apduLen);

    int bytesSent = sendModbusPacket(socketfd, packet, MBAP_SIZE + pduLen);
    // PRINT("[TCP] - bytesSent: %d\n", bytesSent);

    if (bytesSent < 0) {
        PRINT("[TCP][SMR] - Error sending request\n");
        return -1;
    }

    // free(packet);

    return 0;
}

#undef PRINT
#undef DEBUG