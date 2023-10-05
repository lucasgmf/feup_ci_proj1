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
    server.sin_port = htons(port);

    // server.sin_addr.s_addr = inet_addr(ip);
    if (inet_aton(ip, &server.sin_addr) == 0) {
        printf("[TCP] - Error: Invalid IP address\n");
        return -1;
    }
    return connect(socketfd, (struct sockaddr *)&server, sizeof(server));
}

int tcpDisconnect(int socketfd) {
    return close(socketfd);
}

int tcpSendMBAP(int socketfd, uint8_t *pdu, int pduLen, int id) {
    // printf("\n[INFO][TCP] - PDU: ");
    // for (int i = 0; i < pduLen; i++) {
    //     printf("%02X ", pdu[i]);
    // }
    // printf("\n\n\n");

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
    MBAP[6] = (uint8_t)UNIT_ID & 0xFF;        // Unit identifier - Slave ID

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

    printf("[TCP] - Message sent to Slave: ");
    for (int i = 0; i < MBAP_SIZE; i++) {
        printf("%.2X ", MBAP[i]);
    }
    for (int i = 0; i < pduLen; i++) {
        printf("%.2X ", pdu[i]);
    }
    printf("\n");

    return ((MBAP[4] << 8) + MBAP[5]);  // recover Lenght field
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

    // Reading response from socket
    // int bytes_read = 0;
    // int result;
    // while (bytes_read < responseLen) {
    //     printf("[TCP] - Bytes read: %d\n", bytes_read);
    //     printf("[TCP] - Response length: %d\n", responseLen);

    //     result = read(socketfd, response + bytes_read, responseLen - bytes_read);
    //     printf("[TCP] - Bytes read: %d\n", result);
    //     if (result < 0) {
    //         printf("[TCP] - Error reading response from socket\n");
    //         return NULL;
    //     }
    //     bytes_read += result;
    // }
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

// int sendModbusReq(int socketfd, uint8_t *APDUreq, uint16_t APDUreqLen, uint16_t id) {
//     // Connect to server
//     if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
//         printf("[TCP] - Error connecting to server\n");
//         close(fd);
//         return -1;
//     } else {
//         printf("[TCP] - Connected to server\n");
//     }

//     // Building MBAPDU header (MBAP = Modbus Application Protocol)
//     unsigned char MBAP[MBAP_SIZE];  // not initialized with 0's
//     uint16_t TI = 1;                //! maybe not best solution

//     MBAP[0] = (uint8_t)(TI >> 8);
//     MBAP[1] = (uint8_t)(TI & 0xFF);
//     MBAP[2] = 0x00;  // Protocol identifier (high byte) - 0x00 = Modbus
//     MBAP[3] = 0x00;  // Protocol identifier (low byte) - 0x00 = Modbus
//     // MBAP[4] = (uint8_t)(APDUreqLen >> 8);  // Message length (high byte)
//     MBAP[4] = (uint8_t)((APDUreqLen + 1) >> 8);  // Message length (high byte)
//     // MBAP[5] = (uint8_t)(APDUreqLen & 0xFF);  // Message length (low byte)
//     MBAP[5] = (uint8_t)((APDUreqLen + 1) & 0xFF);  // Message length (low byte)
//     MBAP[6] = UNIT_ID;                             // Unit identifier

//     printf("[TCP] - MBAP: ");
//     for (int i = 0; i < MBAP_SIZE; i++) {
//         printf("%02X ", MBAP[i]);
//     }
//     printf("\n");

//     // (MBAPDU = MBAP + PDU)
//     // Writing MBAPDU header to socket
//     if (write(fd, MBAP, MBAP_SIZE) < 0) {
//         printf("[TCP] - Error sending MBAPDU header to socket\n");
//         return -1;
//     }
//     // Writing APDU request to socket
//     if (write(fd, APDUreq, APDUreqLen) < 0) {
//         printf("[TCP] - Error sending APDU request to socket\n");
//         return -1;
//     }

//     printf("[TCP] - Message sent to Slave: ");
//     for (int i = 0; i < MBAP_SIZE; i++) {
//         // printf("%02X ", MBAP[i]);
//         printf("%.2X ", MBAP[i]);
//     }
//     for (int i = 0; i < APDUreqLen; i++) {
//         // printf("%02X ", APDUreq[i]);
//         printf("%.2X ", APDUreq[i]);
//     }

//     int len = (MBAP[4] << 8) + MBAP[5];  // recover Lenght field
//     printf("[TCP] - Response length: %d", len);
//     printf("\n[TCP] - Message successfully sent to Slave\n");
//     return 1;
// }

// int sendAPDUtoServer(int socketfd, uint8_t *APDUreq, uint16_t APDUreqLen) {
// }

// int recieveModbusReq(int socketfd, uint8_t *response, uint16_t responseLen) {
//     int fd;  // File descriptor
//     struct sockaddr_in server;

//     // Create socket
//     fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if (fd < 0) {
//         printf("[TCP] - Error creating socket\n");
//         return -1;
//     } else {
//         printf("[TCP] - Socket created\n");
//     }

//     MBAP[0] = (uint8_t)(TI >> 8);
//     MBAP[1] = (uint8_t)(TI & 0xFF);
//     MBAP[2] = 0x00;  // Protocol identifier (high byte) - 0x00 = Modbus
//     MBAP[3] = 0x00;  // Protocol identifier (low byte) - 0x00 = Modbus
//     // MBAP[4] = (uint8_t)(APDUreqLen >> 8);  // Message length (high byte)
//     MBAP[4] = (uint8_t)((responseLen + 1) >> 8);  // Message length (high byte)
//     // MBAP[5] = (uint8_t)(APDUreqLen & 0xFF);  // Message length (low byte)
//     MBAP[5] = (uint8_t)((responseLen + 1) & 0xFF);  // Message length (low byte)
//     MBAP[6] = UNIT_ID;
//     int len = (MBAP[4] << 8) + MBAP[5];

//     // Reading response from socket
//     int bytes_read = 0;
//     while (bytes_read < len - 1) {
//         int result = read(fd, response + bytes_read, len - 1 - bytes_read);
//         printf("[TCP] - Bytes read: %d\n", result);
//         if (result < 0) {
//             printf("[TCP] - Error reading response from socket\n");
//             return -1;
//         }
//         bytes_read += result;
//     }

//     printf("\n[TCP] - Message received from Slave:");
//     for (int i = 0; i < len - 1; i++) {
//         // printf("%02X ", response[i]);
//         printf("%.2X ", response[i]);
//     }

//     close(fd);
//     return 0;
// }

// uint8_t *createRegisters(uint16_t startingAddress, uint16_t numberOfRegisters, ) {
//     uint8_t *pdu = (uint8_t *)malloc(sizeof(uint8_t) * numberOfRegisters * 2);

//     if (pdu == NULL) {
//         printf("[App][createRegisters] - Error: Failed to allocate memory\n");
//         return NULL;
//     }

//     pdu[0] = (uint8_t) return registers;
// }