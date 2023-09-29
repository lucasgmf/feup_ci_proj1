#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("usage: progname <IP> <PORT> slave|client\n");
        exit(1);
    }

    if (strcmp(argv[3], "slave") == 0) {
        printf("Slave\n");
    } else if (strcmp(argv[3], "client") == 0) {
        printf("Client\n");
    } else {
        printf("Invalid argument\n");
        exit(1);
    }
    
}