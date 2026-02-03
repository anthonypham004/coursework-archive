#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define PORT 5002
#define BUFFER_SIZE 4096
#define MAX_STRING 1024

// Interacts with the bus_server to fetch information about a trip or reservation
int main() {
    // Create socket
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to connect to server.\n");
        close(sock);
        exit(1);
    }

    printf("Successfully connected to server.\n");
    printf("[Commands]\nTRIP:<trip_id>\nRES:<res_id>\nRESERVE\nEXIT\n");

    // Command input loop
    while (1) {
        // Get user input to send to client
        printf("> ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        // RESERVE command
        if (strcmp(buffer, "RESERVE") == 0) {
            char trip_id[MAX_STRING];
            char first_name[MAX_STRING];
            char last_name[MAX_STRING];
            int seat_row;
            char seat_col;

            printf("Enter trip ID: ");
            scanf("%s", trip_id);
            getchar();
            printf("Enter seat row (1-16): ");
            scanf("%d", &seat_row);
            getchar();
            printf("Enter seat column (A-D): ");
            scanf("%c", &seat_col);
            getchar();
            printf("Enter first name: ");
            scanf("%s", first_name);
            getchar();
            printf("Enter last name: ");
            scanf("%s", last_name);
            getchar();

            // Build the command string for sending
            sprintf(buffer, "RESERVE:%s:%d:%c:%s:%s", trip_id, seat_row, seat_col, first_name, last_name);
        }

        send(sock, buffer, strlen(buffer), 0);

        // Read message from server
        int bytes_read = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            printf("Server has disconnected.\n");
            break;
        }
        buffer[bytes_read] = '\0';
        printf("%s", buffer);

        if (strncmp(buffer, "Terminate", 9) == 0) {
            break;
        }

    }
    close(sock);
    return 0;
}