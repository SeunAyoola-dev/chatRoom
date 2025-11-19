#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../SocketUtil/socket_utils.h"


struct AcceptedSocket {
    int socketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket acceptedSockets[10];
int acceptedSocketCount = 0;

struct AcceptedSocket * acceptIncomingConnections(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, &clientAddress, &clientAddressSize); // blocking call till a client connects
   
    struct AcceptedSocket* acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->socketFD = clientSocketFD;
    acceptedSocket->address = clientAddress;
    acceptedSocket->error = (clientSocketFD < 0) ? -1 : 0;
    acceptedSocket->acceptedSuccessfully = (clientSocketFD >= 0);
    
    return acceptedSocket;
}

void sendRecievedMessageToClients(char *buffer, int socketFD) {
    for(int i = 0; i < acceptedSocketCount; i++) {
        if(acceptedSockets[i].socketFD != socketFD) {
            send(acceptedSockets[i].socketFD, buffer, strlen(buffer), 0);
        }
    }
}

void *receiveData(void *arg) {
    int socketFD = *(int *)arg;
    free(arg);
    char buffer[1024];

    while(true) {

        ssize_t bytesRead = recv(socketFD, buffer, 1024, 0);

        if(bytesRead == 0) break;
        buffer[bytesRead] = '\0';
        printf("%s\n", buffer);

        sendRecievedMessageToClients(buffer, socketFD);
    }

    close(socketFD);
}

void createThreadForClient(struct AcceptedSocket *clientSocket) {
    int *fd = malloc(sizeof(int));
    *fd = clientSocket->socketFD;
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receiveData, fd); // create a new thread to handle incoming connections
}

void startAcceptingIncomingConnections(int serverSocketFD) {

    while(true) {

        struct AcceptedSocket* clientSocket = acceptIncomingConnections(serverSocketFD);
        acceptedSockets[acceptedSocketCount++] = *clientSocket;

        createThreadForClient(clientSocket);

    }
}

int main() {

    int serverSocketFD = createTCPIPv4Socket();
    struct sockaddr_in *serverAddress = createIPv4Address("", 2000);
    
    int result = bind(serverSocketFD, serverAddress, sizeof (*serverAddress));

    if (result == 0) printf("Binding success.\n");

    startAcceptingIncomingConnections(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);

    return 0;
}

