
#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

/**
 * @brief Sends a Modbus TCP request to a server and receives a response.
 *
 * @param ip The IP address of the server.
 * @param port The port number to connect to.
 * @param request The APDU (Application Protocol Data Unit) to request.
 * @param requestLength The length of the APDU.
 * @param response The buffer to store the response.
 * @param responseLength The length of the response buffer.
 * @return int Returns 0 on success, or a negative value on failure.
 */
int sendModbusReq(char *ip, int port, char *request, int requestLength, char *response, int responseLength);

