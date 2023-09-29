#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define IN_BUFF_SIZE 1024
#define SERVER_PORT 5502
#define SERVER_IP "127.0.0.1"

bool find(char *strcat, const char terminator) {
    int i = 0;
    while (strcat[i]) {
        if (strcat[i] == terminator) {
            return true;
        }
        i++;
    }
    return false;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
        printf("Error creating socket\n");
        return -1;
    } else {
        printf("Socket created\n");
    }

    // Prepare sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error connecting to server\n");
        return -1;
    } else {
        printf("Connected to server\n");
    }

    char *message, userInput[IN_BUFF_SIZE] = {'\0'};

    while (!find(userInput, '#')) {
        printf("[INFO] - Ready to send messages to server\n");
        message = userInput;
        fgets(userInput, IN_BUFF_SIZE, stdin);

        if (send(client_socket, message, strlen(message), 0) < 0) {
            printf("Error sending message to server\n");
            return -1;
        } else {
            printf("Message sent to server: %s\n", message);
            printf("Sending %d bytes\n", (int)strlen(message));
        }

        // Receive message from server
        char server_message[IN_BUFF_SIZE];
        ssize_t bytes_recieved = recv(client_socket, server_message, IN_BUFF_SIZE, 0);
        if (bytes_recieved < 0) {
            printf("Error receiving message from server\n");
            return -1;
        } else {
            printf("[INFO] - Message received from server: %s\n", server_message);
            printf("Recieving %d bytes\n", (int)strlen(server_message));
        }
    }
    // Close socket
    close(client_socket);
}