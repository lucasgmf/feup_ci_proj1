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

// int main() {
//     int socketfd = connectToServer(LOCALHOST, PORT);
//     if (socketfd == -1) {
//         PRINT("Error connecting to server\n");
//         return -1;
//     }
//     uint16_t TransactionID = 53;

//     uint16_t readQuantity = 5;
//     uint16_t readAddress = 0;

//     uint16_t dataToRead[10];

//     uint16_t writeAddress = 1;
//     uint16_t writeValue = 70;
//     uint16_t writeQuantity = 1;

//     int result;

//     PRINT("Read Holding Registers request\n");
//     result = readHoldingRegisters(socketfd, TransactionID, readAddress,
//                                   readQuantity, dataToRead);
//     if (result != 0) {
//         PRINT("Error reading registers with error: %d\n", result);
//         return -1;
//     }

//     PRINT("Read Holding Registers response\n");
//     for (int i = 0; i < readQuantity; i++) {
//         PRINT("dataToRead[%d]: %d\n", i, dataToRead[i]);
//     }

//     PRINT("\nWrite Single Register request\n");
//     result = writeMultipleRegisters(socketfd, TransactionID, writeAddress,
//                                     writeQuantity, &writeValue);

//     if (result != 0) {
//         PRINT("Error writing registers with error: %d\n", result);
//         return -1;
//     }
//     disconnectFromServer(socketfd);
//     return 0;
// }

int main() {
    int socket = connectToServer(LOCALHOST, PORT);
    if (socket == -1) {
        PRINT("Error connecting to server\n");
        return -1;
    }

    int result;

    ////////////////////////////////* EX1 *///////////////////////////////////

    uint16_t transactionID = 1;
    uint16_t data[] = {0x41};
    uint16_t startingAddress = 121 - 1;  // register 121
    uint16_t quantity = 1;               // 1 register
    result = writeMultipleRegisters(socket, transactionID, startingAddress,
                                    quantity, data);
    if (result != 0) {
        PRINT("Error writing registers with error: %d\n", result);
        return -1;
    }

    PRINT("\nex1 done!\n");
    ////////////////////////////////* EX2 *///////////////////////////////////

    startingAddress = 122 - 1;  // register 122
    quantity = 4;
    uint8_t A[quantity];
    transactionID = 2;
    result = readHoldingRegisters(socket, transactionID, startingAddress,
                                  quantity, A);
    if (result != 0) {
        PRINT("Error reading registers with error: %d\n", result);
        return -1;
    }

    PRINT("ex2 done!\n");
    ////////////////////////////////* EX3 *///////////////////////////////////

    startingAddress = 126 - 1;  // register 126
    quantity = 1;
    transactionID = 3;
    uint8_t B[quantity];
    result = readHoldingRegisters(socket, transactionID, startingAddress,
                                  quantity, B);
    if (result != 0) {
        PRINT("Error reading registers with error: %d\n", result);
        return -1;
    }
    PRINT("ex3 done!\n");
    ////////////////////////////////* EX4 *///////////////////////////////////

    // Deternine the value of C
    PRINT("\nValue of A: ");
    printPacket(A, 4);
    PRINT("\n");
    PRINT("Value of B: ");
    printPacket(B, 1);
    PRINT("\n");

    uint16_t C;
    if (B[0] == (uint16_t)0) {
        PRINT("B is 0\n");
        PRINT("C = 9999\n");
        C = 9999;
    } else {
        PRINT("B is not 0\n");
        PRINT("C = A[0] + A[3]\n");
        C = A[0] + A[3];
        PRINT("C = %d + %d = %d\n", A[0], A[3], C);
    }

    transactionID = 4;
    startingAddress = 127 - 1;  // register 127
    quantity = 1;
    result = writeMultipleRegisters(socket, transactionID, startingAddress,
                                    quantity, &C);
    if (result != 0) {
        PRINT("Error writing registers with error: %d\n", result);
        return -1;
    }
    disconnectFromServer(socket);

    PRINT("ex4 done!\n");
    ////////////////////////////////* EX5 *///////////////////////////////////

    // socket = connectToServer(REMOTEHOST, PORT);
    socket = connectToServer(LOCALHOST, PORT);
    if (socket == -1) {
        PRINT("Error connecting to server\n");
        return -1;
    }

    transactionID = 5;
    startingAddress = 128 - 1;  // register 127
    quantity = 1;
    result = writeMultipleRegisters(socket, transactionID, startingAddress,
                                    quantity, &C);
    if (result != 0) {
        PRINT("Error writing registers with error: %d\n", result);
        return -1;
    }
    disconnectFromServer(socket);

    return 0;
}

#undef PRINT
#undef DEBUG