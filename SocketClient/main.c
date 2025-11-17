#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>

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

void receiveMessages(int socketFD) {
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
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receiveMessages, socketFD);
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