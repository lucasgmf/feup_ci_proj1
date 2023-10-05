#include <stdio.h>
#include <unistd.h>

#include "include/ModbusApp.h"
#define IN_BUFF_SIZE 100

int main(int argc, char *argv[]) {
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
    int errorCode;
    char *userInput[IN_BUFF_SIZE] = {'\0'};

    printf("Input data: ");
    fgets(*userInput, sizeof(userInput), stdin);

    errorCode = writeMultipleRegisters(SERVER_IP, SERVER_PORT, 0, sizeof(userInput), (uint8_t *)userInput);
    if (errorCode < 0) {
        printf("[Client][WMR] - %d\n", errorCode);
    }

    return 0;
}