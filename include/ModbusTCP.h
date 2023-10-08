#ifndef _MODBUS_TCP_H_
#define _MODBUS_TCP_H_

#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void printPacket(uint8_t *packet, int packetLen);
int tcpCreateSocket();
int tcpConnect(int socketfd, char *ip, int port);
int tcpDisconnect(int socketfd);
int sendModbusRequest(int socketfd, uint16_t id, uint8_t *apdu, int apduLen);
int sendModbusPacket(int socketfd, u_int8_t *packet, int responseLen);
uint8_t recieveModbusPacket(int socketfd, u_int8_t *packet, int sizePacket);
#endif  // _MODBUS_TCP_H_