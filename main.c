#include <stdio.h>
#include <unistd.h>

#include "ModbusApp.h"
// #define IN_BUFF_SIZE 100

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 502

void decimalPacketprint(void* packet, int packetLen) {
    
    printf("Dec: ");
    for (int i = 0; i < packetLen; i += 2) {
        printf("%d ", ((uint8_t*)packet)[i] << 8 | ((uint8_t*)packet)[i + 1]);
    }
    printf("\n");
}

int main() {
    int socketfd = connectToServer(SERVER_IP, SERVER_PORT);
    if (socketfd < 0) {
        printf("Error connecting to server\n");
        return -1;
    }

    uint16_t readQuantity = 10;
    uint16_t readAddress = 0;

    // uint16_t writeAddress = 7;
    // uint16_t writeValue = 0;
    // uint16_t writeQuantity = 1;

    uint8_t* buffer = NULL;
    int bufferLen = 0;

    // uint8_t val[2] = {0x00, 0x00};

    while (1) {
        // printf("----------------------------------------------\n");
        printf("Read Holding Registers request\n");
        buffer = readHoldingRegisters(socketfd, readAddress, readQuantity, &bufferLen);
        if (buffer == NULL) {
            printf("Error reading holding registers\n");
            return -1;
        }
        if (buffer[0] & 0x80) {
            printf("Error code: %d\n", buffer[1]);
            break;
        }
        // printPacket(buffer, bufferLen);
        // decimalPacketprint(buffer + 2, bufferLen - 2);

        free(buffer);

        // printf("\nWrite Single Register request\n");
        // printf("address: %d, value: %d\n", writeAddress, writeValue);

        // buffer = writeMultipleRegisters(socketfd, writeAddress, writeQuantity, &bufferLen, &writeValue);
        // if (buffer == NULL) {
        //     printf("Write Single Register failed\n");
        //     break;
        // }
        // if (buffer[0] & 0x80) {
        //     printf("Error code: %d\n", buffer[1]);
        //     break;
        // }
        // // printf("Final buffer: ");
        // printPacket(buffer, bufferLen);
        // free(buffer);

        // writeValue = (writeValue + 1) % 0xFFFF;

        sleep(1);
    }
    disconnectFromServer(socketfd);
    return 0;
}