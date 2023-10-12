#include <stdio.h>
#include <unistd.h>

#include "ModbusAP.h"

#define REMOTEHOST "10.277.113.1"
#define LOCALHOST "127.0.0.1"
#define PORT 502

// void decimalPacketprint(void* packet, int packetLen) {

//     printf("Decimal: ");
//     for (int i = 0; i < packetLen; i += 2) {
//         printf("%d ", ((uint8_t*)packet)[i] << 8 | ((uint8_t*)packet)[i +
//         1]);
//     }
//     printf("\n");
// }

// #define DEBUG

#ifdef DEBUG

#define PRINT(...) printf(__VA_ARGS__)

#else

#define PRINT(...)

#endif

int main() {
    int socketfd = connectToServer(LOCALHOST, PORT);
    if (socketfd == -1) {
        PRINT("Error connecting to server\n");
        return -1;
    }
    uint16_t TransactionID = 53;

    uint16_t readQuantity = 5;
    uint16_t readAddress = 0;

    uint16_t dataToRead[10];

    uint16_t writeAddress = 1;
    uint16_t writeValue = 70;
    uint16_t writeQuantity = 1;

    int result;

    PRINT("Read Holding Registers request\n");
    result = readHoldingRegisters(socketfd, TransactionID, readAddress,
                                  readQuantity, dataToRead);
    if (result != 0) {
        PRINT("Error reading registers with error: %d\n", result);
        return -1;
    }

    PRINT("Read Holding Registers response\n");
    for (int i = 0; i < readQuantity; i++) {
        PRINT("dataToRead[%d]: %d\n", i, dataToRead[i]);
    }

    PRINT("\nWrite Single Register request\n");
    result = writeMultipleRegisters(socketfd, TransactionID, writeAddress,
                                    writeQuantity, &writeValue);

    if (result != 0) {
        PRINT("Error writing registers with error: %d\n", result);
        return -1;
    }
    disconnectFromServer(socketfd);
    return 0;
}

#undef PRINT
#undef DEBUG