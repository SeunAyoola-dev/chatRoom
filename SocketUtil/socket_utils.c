#include "socket_utils.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int createTCPIPv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in* createIPv4Address(const char *ip, int port) {
struct sockaddr_in* address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if (strlen(ip) == 0) {
        address->sin_addr.s_addr = INADDR_ANY; // if empty will accept connections of any IP address of the host
    } else {
        inet_pton(AF_INET, ip, &address->sin_addr); // otherwise bind to that specific IP address
    }

    return address;
}