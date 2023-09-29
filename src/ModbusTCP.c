#include "ModbusTCP.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 502

#define MBAP_SIZE 7

#define UNIT_ID 0x01  // Slave ID - //! Verify if this is correct

//? Note that the function is also recieving the server response
int sendModbusReq(char *ip, unsigned int port, uint8_t *APDUreq, uint16_t APDUreqLen, uint8_t *response, uint16_t responseLen) {
    int fd;  // File descriptor
    struct sockaddr_in server;

    // Create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("[TCP] - Error creating socket\n");
        return -1;
    } else {
        printf("[TCP] - Socket created\n");
    }

    // Prepare sockaddr_in structure
    // memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htons(port);
    inet_aton(ip, &server.sin_addr);

    // Connect to server
    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("[TCP] - Error connecting to server\n");
        close(fd);
        return -1;
    } else {
        printf("Connected to server\n");
    }

    // Building MBAPDU header (MBAP = Modbus Application Protocol)
    unsigned char MBAP[MBAP_SIZE];  // not initialized with 0's
    uint16_t TI = 1;                //! maybe not best solution

    MBAP[0] = (uint8_t)(TI >> 8);
    MBAP[1] = (uint8_t)(TI & 0xFF);
    MBAP[2] = 0x00;                        // Protocol identifier (high byte) - 0x00 = Modbus
    MBAP[3] = 0x00;                        // Protocol identifier (low byte) - 0x00 = Modbus
    MBAP[4] = (uint8_t)(APDUreqLen >> 8);  // Message length (high byte)
    // MBAP[4] = (uint8_t)((APDUreqLen + 1) >> 8);  // Message length (high byte)
    MBAP[5] = (uint8_t)(APDUreqLen & 0xFF);  // Message length (low byte)
    // MBAP[5] = (uint8_t)((APDUreqLen + 1) & 0xFF);  // Message length (low byte)
    MBAP[6] = UNIT_ID;  // Unit identifier

    printf("[TCP] - MBAP: ");
    for (int i = 0; i < MBAP_SIZE; i++) {
        printf("%02X ", MBAP[i]);
    }
    printf("\n");

    //? (MBAPDU = MBAP + PDU)

    // Writing MBAPDU header to socket
    if (write(fd, MBAP, MBAP_SIZE) < 0) {
        printf("[TCP] - Error sending MBAPDU header to socket\n");
        return -1;
    }

    // Writing APDU request to socket
    if (write(fd, APDUreq, APDUreqLen) < 0) {
        printf("[TCP] - Error sending APDU request to socket\n");
        return -1;
    }

    printf("[TCP] Message sent to Slave:\n");
    for (int i = 0; i < MBAP_SIZE; i++) {
        // printf("%02X ", MBAP[i]);
        printf("%.2X ", MBAP[i]);
    }
    for (int i = 0; i < APDUreqLen; i++) {
        // printf("%02X ", APDUreq[i]);
        printf("%.2X ", APDUreq[i]);
    }

    int len = (MBAP[4] << 8) + MBAP[5];  // recover Lenght field

    // Reading response from socket
    if (read(fd, response, len - 1) < 0) {  //! why -1?
        printf("[TCP] - Error reading response from socket\n");
        return -1;
    }

    printf("\n[TCP] Message received from Slave:");
    for (int i = 0; i < len - 1; i++) {
        // printf("%02X ", response[i]);
        printf("%.2X ", response[i]);
    }

    close(fd);
    return 0;
}
