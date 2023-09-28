#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MYPORT 22222

struct sockaddr_in my_addr;
socklen_t addr_size;

struct sockaddr_in {
    sa_family_t sin_family;
    // short sin_family;         // e.g. AF_INET, AF_INET6
    // unsigned short sin_port;  // e.g. port in network byte order
    uint16_t sin_port;
    struct in_addr sin_addr;  // internet address
    // char sin_zero[8];
};

struct in_addr {
    // unsigned long s_addr; // load with inet_aton()
    u_int32_t s_addr;  // 32 bit address in network byte order
};

/**
 * @brief Creating a socket
 *
 * @param domain Protocol family - PF_INET (IPv4 protocol)
 * @param type Channel type - SOCK_STREAM (TCP)
 * @param protocol 0 for IPPROTO_IP
 * @return int Socket local identifier
 * Used in the server and the client sides
 */
int socket(int domain, int type, int protocol);

/**
 * @brief Used by the server to bind the socket to a port
 *
 * @param sockfd socket local identifier (returned by socket())
 * @param addr pointer to structure with the address to attach to the socket
 * @param addrlen length of the structure pointed by addr
 * @return int
 */
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

// Converting IPv4 and IPv6 addresses from text to binary form
// Network to host

unsigned long int htonl(unsigned long int hostlong);

unsigned short int htons(unsigned short int hostshort);

unsigned long int ntohl(unsigned long int netlong);

unsigned short int ntohs(unsigned short int netshort);

// Converting Network to dotter-decimal notation

long inet_aton(char *cp, struct in_addr *inp);

char *inet_ntoa(struct in_addr in);
