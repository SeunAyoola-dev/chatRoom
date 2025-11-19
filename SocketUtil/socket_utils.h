#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <netinet/in.h>

// Shared function declarations
int createTCPIPv4Socket();
struct sockaddr_in* createIPv4Address(const char *ip, int port);

#endif
