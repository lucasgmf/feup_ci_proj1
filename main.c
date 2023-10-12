#include <stdio.h>
#include <unistd.h>

#include "ModbusApp.h"

#define REMOTEHOST "10.277.113.1"
#define LOCALHOST "127.0.0.1"
#define PORT 502

// void decimalPacketprint(void* packet, int packetLen) {

//     printf("Decimal: ");
//     for (int i = 0; i < packetLen; i += 2) {
//         printf("%d ", ((uint8_t*)packet)[i] << 8 | ((uint8_t*)packet)[i + 1]);
//     }
//     printf("\n");
// }

#define DEBUG

#ifdef DEBUG

#define PRINT(...) printf(__VA_ARGS__)

#else

#define PRINT(...)

#endif

int checkForException(uint8_t* packet, int packetLen, int expectedLen) {
    if (packet == NULL) {
        PRINT("checkForException: packet is NULL\n");
        return -1;
    }

    if (packet[0] & 0x80) {
        PRINT("checkForException: Exception code: %d\n", packet[1]);
        return packet[1];
    }

    if (packetLen != expectedLen) {
        // incorrect response length
        PRINT("checkForException: packetLen != expectedLen\n");
        return -1;
    }
    return 0;
}

int main() {
    int socketfd = connectToServer(LOCALHOST, PORT);
    if (socketfd == -1) {
        PRINT("Error connecting to server\n");
        return -1;
    }
    uint16_t TransactionID = 1;

    uint16_t readQuantity = 5;
    uint16_t readAddress = 0;

    uint16_t dataToRead[10];

    uint16_t writeAddress = 5;
    uint16_t writeValue = 70;
    uint16_t writeQuantity = 1;

    // uint8_t* buffer = NULL;
    // int bufferLen = 0;

    // uint8_t val[2] = {0x00, 0x00};

    while (1) {
        PRINT("Read Holding Registers request\n");
        if (readHoldingRegisters(socketfd, TransactionID, readAddress, readQuantity, dataToRead) != 0) {
            PRINT("Error reading registers\n");
            break;
        }
        PRINT("Read Holding Registers response\n");
        for (int i = 0; i < readQuantity; i++) {
            PRINT("dataToRead[%d]: %d\n", i, dataToRead[i]);
        }

        PRINT("\nWrite Single Register request\n");
        writeMultipleRegisters(socketfd, TransactionID, writeAddress, writeQuantity, &writeValue);

        writeValue = (writeValue + 1) % 0xFFFF;

        sleep(1);
    }
    disconnectFromServer(socketfd);
    return 0;
}

#undef PRINT
#undef DEBUG