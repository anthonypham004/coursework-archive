#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "bus.h"

#define PORT 5002
#define BUFFER_SIZE 4096

// Helper function that makes the seat map visual
void format_seat_map(Trip *trip, char *out_str) {
    char line[128];
    strcat(out_str, "    A   B    C   D\n");
    for (int row = 0; row < ROWS; row++) {
        sprintf(line, "%-2d ", row + 1);
        strcat(out_str, line);
        for (int col = 0; col < COLS; col++) {
            if (trip->seats[row][col].is_reserved == true) {
                strcat(out_str, "[X]");
            } else {
                strcat(out_str, "[ ]");
            }
            if (col == 1)
                strcat(out_str, "  ");  // aisle spacing
            else
                strcat(out_str, " ");
        }
        strcat(out_str, "\n");
    }
    strcat(out_str, "X = reserved, [ ] = available\n");
}

int main() {
    // Socket setup
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Set up socket to listen
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to bind socket.\n");
        close(server_sock);
        exit(1);
    }

    if (listen(server_sock, 5) < 0) {
        printf("Failed to set socket to listen.\n");
        close(server_sock);
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        printf("Failed to connect to client.\n");
        close(server_sock);
        exit(1);
    }

    printf("Client connected.\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from client
        int bytes_read = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            printf("Client has disconnected.\n");
            break;
        }

        buffer[bytes_read] = '\0';
        printf("Received: %s\n", buffer);

        char response[BUFFER_SIZE] = {0};

        // Process command
        if (strncmp(buffer, "TRIP:", 5) == 0) {
            char *trip_id = buffer + 5;

            // Fetch requested trip
            GDBM_FILE dbf = gdbm_open(BUS_DATABASE, 512, GDBM_READER, 0644, 0);
            if (!dbf) {
                snprintf(response, BUFFER_SIZE, "get trip error: could not open bus database.\n");
            } else {
                datum key;
                key.dptr = trip_id;
                key.dsize = strlen(trip_id) + 1;
                datum value;
                if (!gdbm_exists(dbf, key)) {
                    snprintf(response, BUFFER_SIZE, "Trip %s not found.\n", trip_id);
                } else {
                    value = gdbm_fetch(dbf, key);
                    static Trip trip;
                    memcpy(&trip, value.dptr, sizeof(Trip));
                    free(value.dptr);

                    snprintf(response, BUFFER_SIZE, "Trip %s: Origin %s, Destination %s, Bus %s\n", 
                        trip.trip_id, trip.origin, trip.destination, trip.bus_number);

                    char seat_map[2048] = "";
                    format_seat_map(&trip, seat_map);
                    strcat(response, seat_map);
                }
                gdbm_close(dbf);
            }
        // Fetch reservation details
        } else if (strncmp(buffer, "RES:", 4) == 0) {
            char *res_id = buffer + 4;

            GDBM_FILE dbf = gdbm_open(RES_DATABASE, 512, GDBM_READER, 0644, 0);
            if (!dbf) {
                snprintf(response, BUFFER_SIZE, "get reservation erorr: could not open reservation database.\n");
            } else {
                datum key;
                key.dptr = res_id;
                key.dsize = strlen(res_id) + 1;
                datum value;
                if (!gdbm_exists(dbf, key)) {
                    snprintf(response, BUFFER_SIZE, "Reservation %s not found.\n", res_id);
                } else {
                    value = gdbm_fetch(dbf, key);
                    static Reservation res;
                    memcpy(&res, value.dptr, sizeof(Reservation));
                    free(value.dptr);

                    snprintf(response, BUFFER_SIZE, "Reservation %s\nFirst name: %s\nLast name: %s\nSeat: %d%c\nTrip: %s\n", 
                        res.reservation_id, res.first_name, res.last_name, res.seat_row + 1, res.seat_col, res.trip_id);
                }
                gdbm_close(dbf);
            }
        // Parses reserve command and creates a reservation from the command
        } else if (strncmp(buffer, "RESERVE:", 8) == 0) {
            char trip_id[MAX_STRING];
            char first_name[MAX_STRING];
            char last_name[MAX_STRING];
            int seat_row;
            char seat_col;

            // Parse command
            if (sscanf(buffer, "RESERVE:%[^:]:%d:%c:%[^:]:%s", trip_id, &seat_row, &seat_col, first_name, last_name) != 5) {
                snprintf(response, BUFFER_SIZE, "Invalid RESERVE format.\n");
            } else {
                seat_col = toupper(seat_col);

                // Fetch trip from database
                GDBM_FILE dbf = gdbm_open(BUS_DATABASE, 512, GDBM_READER, 0644, 0);
                if (!dbf) {
                    snprintf(response, BUFFER_SIZE, "reserve error: could not open bus database.\n");
                } else {
                    datum key;
                    datum value;
                    key.dptr = trip_id;
                    key.dsize = strlen(trip_id) + 1;
                    if (!gdbm_exists(dbf, key)) {
                        snprintf(response, BUFFER_SIZE, "Trip %s was not found.\n", trip_id);
                        gdbm_close(dbf);
                    } else {
                        // Get trip value from database
                        value = gdbm_fetch(dbf, key);
                        static Trip trip;
                        memcpy(&trip, value.dptr, sizeof(Trip));
                        free(value.dptr);
                        gdbm_close(dbf);

                        // Check seat validity
                        int col_idx = seat_col - 'A'; // Convert letter to col index
                        if (col_idx < 0 || col_idx >= COLS || seat_row < 1 || seat_row > ROWS) {
                            snprintf(response, BUFFER_SIZE, "Seat is invalid.\n");
                        } else if (trip.seats[seat_row - 1][col_idx].is_reserved) {
                            snprintf(response, BUFFER_SIZE, "Seat %d%c is aleady reserved.\n", seat_row, seat_col);
                        } else {
                            // Set reservation values
                            Reservation new_res;
                            memset(&new_res, 0, sizeof(Reservation));
                            strcpy(new_res.trip_id, trip.trip_id);
                            new_res.seat_row = seat_row - 1;
                            new_res.seat_col = seat_col;
                            new_res.col_idx = col_idx;
                            strcpy(new_res.first_name, first_name);
                            strcpy(new_res.last_name, last_name);

                            // Make seat reservation ID
                            int reservation_count = 0;
                            for (int row = 0; row < ROWS; row++) {
                                for (int col = 0; col < COLS; col++) {
                                    if (trip.seats[row][col].is_reserved == true) {
                                        reservation_count++;
                                    }
                                }
                            }
                            reservation_count++;
                            sprintf(new_res.reservation_id, "%s-%d", trip.trip_id, reservation_count);

                            // Save reservation to database
                            if (save_res_to_db(&new_res) == 0) {
                                trip.seats[seat_row - 1][col_idx].is_reserved = true;
                                trip.seats[seat_row - 1][col_idx].reservation = new_res;
                                save_trip_to_db(&trip);
                                snprintf(response, BUFFER_SIZE, "Successfully created reservation! Your reservation ID is: %s\nSave this in case your reservation needs to be modified.\n", new_res.reservation_id);
                            } else {
                                snprintf(response, BUFFER_SIZE, "reserve error: failed to save reservation.\n");
                            }
                        }
                    }
                }
            }

        } else if (strncmp(buffer, "EXIT", 4) == 0) {
            snprintf(response, BUFFER_SIZE, "Terminate\n");
            send(client_sock, response, strlen(response), 0);
            break;
        } else {
            snprintf(response, BUFFER_SIZE, "Invalid command.\n");
        }

        // Send response
        send(client_sock, response, strlen(response), 0);
    }

    close(client_sock);
    close(server_sock);
    return 0;
}
