#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define IN_BUFF_SIZE 1024
#define SERVER_PORT 5502
#define SERVER_IP "127.0.0.1"

void toUpperCase(char *str) {
    int i = 0;
    while (str[i]) {
        str[i] = toupper(str[i]);
        i++;
    }
}

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
    int socket_desk, out, in;
    struct sockaddr_in server_addr;
    char *out_buffer, in_buffer[1024] = {'\0'};

    // Create socket
    socket_desk = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_desk < 0) {
        PRINT("Error creating socket\n");
        return -1;
    } else {
        PRINT("Socket created\n");
    }

    // Prepare sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind socket to port
    if (bind(socket_desk, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        PRINT("Error binding socket to port\n");
        return -1;
    } else {
        PRINT("Socket binded to port\n");
    }

    // Listen for connections
    if (listen(socket_desk, 3) < 0) {
        PRINT("Error listening for connections\n");
        return -1;
    }
    PRINT("Listening for connections on port %d...\n", SERVER_PORT);

    // Accept connection from an incoming client
    out = sizeof(struct sockaddr_in);
    in = accept(socket_desk, (struct sockaddr *)&server_addr, (socklen_t *)&out);
    if (in < 0) {
        PRINT("Error accepting connection\n");
        return -1;
    } else {
        PRINT("Connection accepted\n");
    }

    while (!find(in_buffer, '#')) {
        // Receive a message from client
        int bytes_received = recv(in, in_buffer, IN_BUFF_SIZE, 0);
        if (bytes_received < 0) {
            PRINT("Error receiving message from client\n");
            return -1;
        } else {
            PRINT("[INFO] - Message received from client: %s\n", in_buffer);
            PRINT("[INFO] - Recieved %d bytes\n", (int)strlen(in_buffer));
        }

        // Send message to client
        // out_buffer = "Hello from server";
        toUpperCase(in_buffer);
        out_buffer = in_buffer;
        PRINT("[INFO] - Message to send to client: %s\n", out_buffer);

        if (send(in, out_buffer, strlen(out_buffer), 0) < 0) {
            PRINT("Error sending message to client\n");
            return -1;
        } else {
            PRINT("[INFO] - Message sent to client\n");
            PRINT("[INFO] - Sending %d bytes\n", (int)strlen(in_buffer));
        }
    }
    return 0;
}