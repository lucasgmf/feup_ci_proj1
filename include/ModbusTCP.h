
#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 502

/**
 * @brief Sends a Modbus TCP request to a server and receives a response.
 *
 * @param ip The IP address of the server.
 * @param port The port number to connect to.
 * @param APDUreq The APDU (Application Protocol Data Unit) to request.
 * @param APDUreqLen The length of the APDU.
 * @param response The buffer to store the response.
 * @param responseLen The length of the response buffer.
 * @return int Returns 0 on success, or a negative value on failure.
 */
int sendModbusReq(char *ip, unsigned int port, uint8_t *APDUreq, uint16_t APDUreqLen, uint8_t *response, uint16_t responseLen);
