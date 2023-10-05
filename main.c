#include <stdio.h>
#include <unistd.h>

#include "ModbusApp.h"
// #define IN_BUFF_SIZE 100

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 502

int main() {
    // int main(int argc, char* argv[]) {
    // if (argc < 4) {
    //     printf("usage: progname <IP> <PORT> slave|client\n");
    //     exit(1);
    // }

    // if (strcmp(argv[3], "slave") == 0) {
    //     printf("Slave\n");
    // } else if (strcmp(argv[3], "client") == 0) {
    //     printf("Client\n");
    // } else {
    //     printf("Invalid argument\n");
    //     exit(1);
    // }
    // int errorCode;
    // char *userInput[IN_BUFF_SIZE] = {'\0'};

    // printf("Input data: ");
    // fgets(*userInput, sizeof(userInput), stdin);

    // errorCode = writeMultipleRegisters(SERVER_IP, SERVER_PORT, 0, sizeof(userInput), (uint8_t *)userInput);
    // if (errorCode < 0) {
    //     printf("[Client][WMR] - %d\n", errorCode);
    // }

    int socketfd = connectToServer(SERVER_IP, SERVER_PORT);
    if (socketfd < 0) {
        printf("[Main] - Error connecting to server\n");
        return -1;
    } else {
        printf("[Main] - Connected to server\n");
    }

    uint16_t readQuantity = 10;
    uint16_t readAddress = 0;

    uint16_t* data = NULL;

    int dataLength = 0;

    while (1) {
        printf("[Main] - Reading holding registers begin with starting address: %d, quantity: %d\n", readAddress, readQuantity);
        data = readHoldingRegisters(socketfd, readAddress, readQuantity, &dataLength);
        printf("[Main] - data: %p\n", data);
        if (data == NULL) {
            printf("[Main] - Error reading holding registers\n");
            return -1;
        }
        printf("[Main] - Buffer length: %d\n", dataLength);

        printf("[Main] - Data: ");
        for (int i = 0; i < 10; i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
        printf("Ending reading holding registers\n\n\n\n");

        sleep(1);
    }
    return 0;
}