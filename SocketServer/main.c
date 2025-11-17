#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>


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

void startAcceptingIncomingConnections(int serverSocketFD) {

    while(true) {

        struct AcceptedSocket* clientSocket = acceptIncomingConnections(serverSocketFD);
        acceptedSockets[acceptedSocketCount++] = *clientSocket;

        createThreadForClient(clientSocket);

    }
}

void sendRecievedMessageToClients(char *buffer, int socketFD) {
    for(int i = 0; i < acceptedSocketCount; i++) {
        if(acceptedSockets[i].socketFD != socketFD) {
            send(acceptedSockets[i].socketFD, buffer, strlen(buffer), 0);
        }
    }
}

void receiveData(int socketFD) {
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
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receiveData, clientSocket->socketFD); // create a new thread to handle incoming connections
}


int createTCPIPv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in* createIPv4Address(char* ip, int port) {
    struct sockaddr_in* address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if(strlen(ip) == 0) {
        address->sin_addr.s_addr = INADDR_ANY; // if empty will accept connections of any IP address of the host
    } else {
        inet_pton(AF_INET, ip, &address->sin_addr); // otherwise bind to that specific IP address
    }

    return address;
}

int main() {

    int serverSocketFD = createTCPIPv4Socket();
    struct sockaddr_in *serverAddress = createIPv4Address("", 2000);
    
    int result = bind(serverSocketFD, serverAddress, sizeof (*serverAddress));

    if (result == 0) printf("Binding success.\n");
    
    int listenResult = listen(serverSocketFD, 10); // 10: maximum number of queued connections

    startAcceptingIncomingConnections(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);

    return 0;
}

