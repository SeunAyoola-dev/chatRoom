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

void *receiveMessages(void *arg) {
    int socketFD = *(int *)arg;
    free(arg);
    char buffer[1024];

        while(true) {

            ssize_t bytesRead = recv(socketFD, buffer, 1024, 0);

            if(bytesRead == 0) break;
            buffer[bytesRead] = '\0';
            printf("%s\n", buffer);
        }

        close(socketFD);
}

void createNewThreadForMessaging(int socketFD) {
    int *fd = malloc(sizeof(int));
    *fd = socketFD;
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receiveMessages, fd);
}

int main() {

    int socketFD = createTCPIPv4Socket();
    if (socketFD < 0) printf("Failed to create sockets.\n");
        
    struct sockaddr_in *address = createIPv4Address("127.0.0.1", 2000);
    int result = connect(socketFD, address, sizeof (*address));
    if (result < 0) printf("Connection failed.\n");
    

    char *name = NULL;
    size_t nameSize = 0;
    printf("Please enter your name: \n");
    ssize_t nameCount = getline(&name, &nameSize, stdin);
    name[nameCount - 1] = '\0'; // remove newline character from the end

    char buffer[1024];
    char *line = NULL;
    size_t lineSize = 0; 

    printf("Type message (type exit to end communication):\n");
    createNewThreadForMessaging(socketFD);

    while(true) {
        ssize_t charCount = getline(&line, &lineSize, stdin); // read a line from standard input, blocks until enter is pressed
        line[charCount - 1] = '\0';
        sprintf(buffer, "%s:%s", name, line);

        if(charCount > 0){
            if(strcmp(line, "exit\n") == 0) break;

            ssize_t amountSent = send(socketFD, buffer, strlen(buffer), 0);


        }
    }

    close(socketFD);
    return 0; 
}