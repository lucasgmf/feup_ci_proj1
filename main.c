#include <stdio.h>
#include <unistd.h>

#include "ModbusApp.h"
// #define IN_BUFF_SIZE 100

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 502

int main() {
    int socketfd = connectToServer(SERVER_IP, SERVER_PORT);
    if (socketfd < 0) {
        printf("Error connecting to server\n");
        return -1;
    }

    uint16_t readQuantity = 10;
    uint16_t readAddress = 0;

    uint8_t* buffer = NULL;
    int bufferLen = 0;

    // uint8_t val[2] = {0x00, 0x00};

    while (1) {
        buffer = readHoldingRegisters(socketfd, readAddress, readQuantity, &bufferLen);
        if (buffer == NULL) {
            printf("Error reading holding registers\n");
            return -1;
        }
        if (buffer[0] & 0x80) {
            printf("Error code: %d\n", buffer[1]);
            break;
        }
        printf("Final buffer: ");
        printPacket(buffer, 5);

        free(buffer);
        sleep(1);
    }
    disconnectFromServer(socketfd);
    return 0;
    // int socketfd = connectToServer(SERVER_IP, SERVER_PORT);

    // if (socketfd < 0) {
    //     printf("[Main] - Error connecting to server\n");
    //     return -1;
    // } else {
    //     printf("[Main] - Connected to server\n");
    // }

    // uint16_t readQuantity = 10;
    // uint16_t readAddress = 0;

    // uint8_t* data = NULL;

    // int dataLength = 0;

    // while (1) {
    //     printf("[Main] - Reading holding registers begin with starting address: %d, quantity: %d\n", readAddress, readQuantity);
    //     data = readHoldingRegisters(socketfd, readAddress, readQuantity, &dataLength);
    //     printf("[Main] - data: %p\n", data);
    //     if (data == NULL) {
    //         printf("[Main] - Error reading holding registers\n");
    //         return -1;
    //     }
    //     printf("[Main] - Buffer length: %d\n", dataLength);

    //     printf("[Main] - Data: ");
    //     for (int i = 0; i < 10; i++) {
    //         printf("%d ", data[i]);
    //     }
    //     printf("\n");
    //     printf("Ending reading holding registers\n\n\n\n");

    //     sleep(1);
    // }
    // return 0;
}