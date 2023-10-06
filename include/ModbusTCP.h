#ifndef _MODBUS_TCP_H_
#define _MODBUS_TCP_H_

#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int tcpCreateSocket();
int tcpConnect(int socketfd, char *ip, int port);
int tcpDisconnect(int socketfd);
int sendModbusPacket(int socketfd, u_int8_t *response, int responseLen);
int tcpSendMBAP(int socketfd, uint8_t *pdu, int pduLen, int id);
uint8_t *tcpRecieveMBAP(int socketfd, uint8_t *mbapdu, int *responseLen);
int recieveModbusPacket(int socketfd, u_int8_t *packet, int sizePacket);

#endif  // _MODBUS_TCP_H_