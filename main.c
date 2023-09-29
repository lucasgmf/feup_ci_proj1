#include <stdio.h>
#include <unistd.h>

#include "ModbusTCP.h"

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

    sendModbusReq("127.0.0.1", 502, NULL, 0, NULL, 0);
    return 0;
}