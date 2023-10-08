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
    printf("----------------------------------------------\n");
    printf("packet: ");
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
    // int transID = 0;

    uint8_t *mbap = (uint8_t *)malloc(MBAP_SIZE);
    if (mbap == NULL) {
        printf("[TCP][SMR] - Error: Failed to allocate memory\n");
        return -1;
    }

    // transID++;
    mbap[0] = (uint8_t)id >> 8;               // Transaction Identifier
    mbap[1] = (uint8_t)id & 0xFF;             // Transaction Identifier
    mbap[2] = (uint8_t)0x00;                  // Protocol Identifier
    mbap[3] = (uint8_t)0x00;                  // Protocol Identifier
    mbap[4] = (uint8_t)(apduLen + 1) >> 8;    // Length
    mbap[5] = (uint8_t)(apduLen + 1) & 0xFF;  // Length
    mbap[6] = (uint8_t)0x01;                  // Unit Identifier

    int pduLen = MBAP_SIZE + apduLen;
    uint8_t *pdu = (uint8_t *)malloc(pduLen);

    memcpy(pdu, mbap, MBAP_SIZE);
    memcpy(pdu + MBAP_SIZE, apdu, apduLen);

    // printf("[TCP][SMR] - Sending pdu: ");
    // printPacket(pdu, pduLen);

    int bytesSent = sendModbusPacket(socketfd, pdu, pduLen);
    if (bytesSent < 0) {
        printf("[TCP][SMR] - Error sending request\n");
        return -1;
    }

    // printf("[TCP][SMR] - Sent %d bytes of data\n", bytesSent);

    // TODO check recieved data
    uint8_t *mbapResponse = (uint8_t *)malloc(MBAP_SIZE);
    if (mbapResponse == NULL) {
        printf("[TCP][SMR] - Error: Failed to allocate memory\n");
        return -1;
    }
    int bytesRecieved = recieveModbusPacket(socketfd, mbapResponse, MBAP_SIZE);
    if (bytesRecieved < 0) {
        printf("[TCP][SMR] - Error recieving response\n");
        return -1;
    }
    // printf("[TCP][SMR] - Recieved %d bytes of data\n", bytesRecieved);
    // printf("[TCP][SMR] - recieving mbapResponse: ");
    // printPacket(mbapResponse, MBAP_SIZE);

    uint16_t apduResponseLen = ((mbapResponse[4] << 8) | mbapResponse[5]) - 1;
    // printf("[TCP][SMR] - apduResponseLen: %d\n", apduResponseLen);

    uint8_t *apduResponse = (uint8_t *)malloc(apduLen * sizeof(apduResponse));
    if (apduResponse == NULL) {
        printf("[TCP][SMR] - Error: Failed to allocate memory\n");
        return -1;
    }

    bytesRecieved = recieveModbusPacket(socketfd, apduResponse, apduResponseLen);
    if (bytesRecieved < 0) {
        printf("[TCP][SMR] - Error recieving response\n");
        return -1;
    }
    // printf("[TCP][SMR] - Recieved %d bytes of data\n", bytesRecieved);
    // printf("[TCP][SMR] - recieving apduResponse: ");
    // printPacket(apduResponse, apduResponseLen);

    memcpy(apdu, apduResponse, apduResponseLen);
    printPacket(apdu, apduResponseLen);
    return bytesRecieved;
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
        // printf("[TCP] - Sent %d bytes of data\n", k - 7);
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
// int tcpSendMBAP(int socketfd, uint8_t *pdu, int pduLen, int id) {
//     if (socketfd < 0) {
//         printf("[TCP] - Error: Invalid socket file descriptor\n");
//         return -1;
//     }
//     if (pduLen < 0) {
//         printf("[TCP] - Error: Invalid PDU length\n");
//         return -1;
//     }
//     if (id < 0) {
//         printf("[TCP] - Error: Invalid ID\n");
//         return -1;
//     }

//     uint8_t MBAP[MBAP_SIZE];

//     id = 0x03;

//     MBAP[0] = (uint8_t)(id >> 8) & 0xFF;            // Transaction identifier (high byte)
//     MBAP[1] = (uint8_t)id & 0xFF;                   // Transaction identifier (low byte)
//     MBAP[2] = (uint8_t)(0x00 >> 8) & 0xFF;          // Protocol identifier (high byte) - 0x00 = Modbus
//     MBAP[3] = (uint8_t)0x00 & 0xFF;                 // Protocol identifier (low byte) - 0x00 = Modbus
//     MBAP[4] = (uint8_t)((pduLen + 1) >> 8) & 0xFF;  // Message length (high byte)
//     MBAP[5] = (uint8_t)(pduLen + 1) & 0xFF;         // Message length (low byte)
//     // MBAP[6] = (uint8_t)UNIT_ID;                     // Unit identifier - Slave ID
//     MBAP[6] = (uint8_t)0x00;

//     printf("[TCP] - MBAP: ");
//     for (int i = 0; i < MBAP_SIZE; i++) {
//         printf("%02X ", MBAP[i]);
//     }
//     for (int i = 0; i < pduLen; i++) {
//         printf("%02X ", pdu[i]);
//     }
//     printf("\n");

//     // (MBAPDU = MBAP + PDU)
//     // Writing MBAP header to socket

//     int sizePacket = MBAP_SIZE + pduLen;

//     uint8_t *packet = (uint8_t *)malloc(sizePacket * sizeof(packet));
//     memcpy(packet, MBAP, MBAP_SIZE);
//     memcpy(packet + MBAP_SIZE, pdu, pduLen);

//     printf("1-----------------packetAEnviar: ");
//     for (int i = 0; i < sizePacket; i++) {
//         printf("%02X ", packet[i]);
//     }
//     printf("\n");

//     printf("[TCP] - packet: ");
//     for (int i = 0; i < sizePacket; i++) {
//         printf("%02X ", packet[i]);
//     }
//     printf("\n");

//     int bytesSent = sendModbusPacket(socketfd, packet, sizePacket);
//     printf("[TCP] - bytesSent: %d\n", bytesSent);

//     if (bytesSent < 0) {
//         printf("[TCP] - Error sending MBAPDU\n");
//         return -1;
//     }
//     free(packet);

//     uint8_t mbapHeader[MBAP_SIZE];
//     int bytesRecieved = recievePacket(socketfd, mbapHeader, MBAP_SIZE);

//     printf("XX-----------------packetRecebido: ");
//     for (int i = 0; i < MBAP_SIZE; i++) {
//         printf("%02X ", mbapHeader[i]);
//     }
//     printf("\n");

//     if (bytesRecieved < 0) {
//         printf("[TCP] - Error recieving MBAPDU from server\n");
//         return -1;
//     }

//     pduLen = ((mbapHeader[4] << 8) | mbapHeader[5]) - 1;

//     // recieve pdu
//     uint8_t *pdu2 = (uint8_t *)malloc(pduLen);
//     if (pdu == NULL) {
//         printf("[App][RHR] - Error: Failed to allocate memory\n");
//         return 0;
//     }

//     bytesRecieved = recievePacket(socketfd, pdu2, pduLen);
//     if (bytesRecieved < 0) {
//         printf("[TCP] - Error recieving PDU from server\n");
//         return -1;
//     }

//     printf("XX-----------------packetRecebido2: ");
//     for (int i = 0; i < pduLen; i++) {
//         printf("%02X ", pdu2[i]);
//     }
//     printf("\n");

//     pduLen = pduLen - 1;

//     printf("XXXXXXXXXXXXXXXXXXXXXX ");
//     for (int i = 0; i < pduLen + 1; i++) {
//         printf("%02X ", pdu2[i]);
//     }
//     printf("\n");

//     // if (bytesRecieved < 0) {
//     //     printf("[TCP] - Error recieving MBAPDU from server\n");
//     //     return -1;
//     // }

//     // printf("\n[TCP] - Size of data: %d\n", (packet[4] >> 8) + packet[5] - 1);
//     // printf("[TCP] - bytesRecieved: %d\n", bytesRecieved);

//     return 1;
// }
