#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char const* arg[])
{
    // create server socket 
    int serverSocketID = socket(AF_INET, SOCK_STREAM, 0);

    // define server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9001);
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int connectStatus = connect(serverSocketID, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (connectStatus == -1) {
        printf("Error: Could not connect to the server. \n");
    }
    else {
        char serverMsg[255];
        char clientMsg[255];
        /*
        for some reason, leftover characters will appear in the received message so I had to
        manually add a null terminator to each received message so it prints *only* the message that just got received.
        */

        // receive the server's initial message.
        int bytesReceived = recv(serverSocketID, serverMsg, sizeof(serverMsg), 0);
        serverMsg[bytesReceived] = '\0';

        printf("%s", serverMsg);
        
        // prompt the client to enter a file name, which will then get sent
        scanf("%s", clientMsg);
        send(serverSocketID, clientMsg, strlen(clientMsg), 0);

        // client will either receive the file contents or an error message
        bytesReceived = recv(serverSocketID, serverMsg, sizeof(serverMsg), 0);
        if (bytesReceived == 0) {
            printf("Server closed the connection. Sorry!\n");
        } else {
            serverMsg[bytesReceived] = '\0';
            printf("%s", serverMsg);
        }
    }
    
    return 0;

}