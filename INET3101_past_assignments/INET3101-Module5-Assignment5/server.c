#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strlen()
#include <signal.h> // for SIGINT handling
#include <unistd.h> // for close()
#include <sys/socket.h>
#include <sys/types.h>

/*
I have these global varaibles out here so that sigint_handler() can access them and
do its cleanup process accordingly.
*/
int serverSocketID = -1;
int clientSocket = -1;
FILE *f = NULL;
char *fileContents = NULL;

void close_sockets(int serverSocket, int clientSocket) {
    close(serverSocket);
    close(clientSocket);
    printf("Connections successfully ended.\n");
}

// when CTRL + C is pressed, clean up and end the program
void sigint_handler(int sig) {
    if (f)
        fclose(f);
    if (fileContents)
        free(fileContents);
    if (serverSocketID != -1)
        close(serverSocketID);
    if (clientSocket != -1)
        close(clientSocket);
    char sigintMsg[] = "\nCaught SIGINT. Terminated program.\n";
    // async-signal-safe alternatives to printf() and exit()
    write(STDOUT_FILENO, sigintMsg, sizeof(sigintMsg));
    _exit(0);
}

int main(int argc, char const* arg[])
{
    // signal handler for SIGINT
    signal(SIGINT, sigint_handler);

    // create server socket 
    serverSocketID = socket(AF_INET, SOCK_STREAM, 0);

    // string store data to be sent to client 
    char greetingMsg[] = "Successfully connected to server.\nPlease enter file name: ";

    // define server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9001);
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // bind socket to the specified port and IP
    int opt = 1;
    setsockopt(serverSocketID, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // (hopefully) helps prevent "Address already in use" errors
    bind(serverSocketID, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    printf("Listening for Connections\n");
    // listen for connections 
    // second parameter that this server will keep a backlog of 1 request 
    listen(serverSocketID, 1);

    // define variable to hold client socket
    clientSocket = accept(serverSocketID, NULL, NULL);

    // acknowledge and identify connection
    printf("Client %i connected to server.\n", clientSocket);

    send(clientSocket, greetingMsg, strlen(greetingMsg), 0);

    // store message received from client
    char clientMsg[255];

    // listen for a response from client
    int bytesReceived = recv(clientSocket, clientMsg, sizeof(clientMsg), 0);
    clientMsg[bytesReceived] = '\0';
    printf("Client is requesting: %s\n", clientMsg);

    // File I/O

    // check if given message has a file extention
    char *extention = strstr(clientMsg, ".txt");
    if (extention == NULL) {
        char errorMsg[] = "Error: Given input is not a file.\n";
        send(clientSocket, errorMsg, strlen(errorMsg), 0);
        printf("Error: Client did not provide a valid file name.\n");
        close_sockets(serverSocketID, clientSocket);
        exit(1);
    }

    // attempt to open file from name
    char filePath[266] = "textfiles/";
    strcat(filePath, clientMsg);

    f = fopen(filePath, "r");
    if (!f) {
        char errorMsg[] = "Error: File not found.\n";
        send(clientSocket, errorMsg, strlen(errorMsg), 0);
        printf("Error: Could not find file for client.\n");
        close_sockets(serverSocketID, clientSocket);
        exit(1);
    }

    // get the file size
    fseek(f, 0, SEEK_END);
    int fileSize = ftell(f);
    rewind(f);

    // return error if file size exceeds 255 characters
    if (fileSize > 255) {
        char errorMsg[] = "Error: File size exceeds 255 characters.\n";
        send(clientSocket, errorMsg, strlen(errorMsg), 0);
        printf("Error: Client's requested file is too large.\n");
        fclose(f);
        close_sockets(serverSocketID, clientSocket);
        exit(1);
    }

    // if given file is valid, read the file and send contents to client
    fileContents = malloc(fileSize + 1); // allocate enough space for contents + null terminator
    int bytesRead = fread(fileContents, 1, fileSize, f);
    fileContents[bytesRead] = '\0'; // end with null terminator
    send(clientSocket, fileContents, bytesRead, 0);

    free(fileContents);
    fclose(f);
    close_sockets(serverSocketID, clientSocket);
    return 0;

}
