#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gdbm.h>

#include "bus.h"

// RESERVATION FUNCTIONS
void create_reservation() {
    Reservation new_res;

    char first_name[MAX_STRING];
    char last_name[MAX_STRING];

    // Have user pick a trip to reserve
    printf("\n================= TRIP SELECTION ==================\n");
    Trip *trip_ptr = get_trip();
    Trip trip;
    if (trip_ptr == NULL) {
        return;
    } else {
        trip = *trip_ptr;
    }

    int total_seats = ROWS * COLS;
    int available_seats = total_seats;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (trip.seats[row][col].is_reserved == true) {
                available_seats--;
            }
        }
    }

    if (available_seats == 0) {
        printf("No seats are available for this trip.");
        return;
    }

    // Display trip and seating info
    printf("\n==================== TRIP INFO ====================\n");
    printf("Trip ID: %s\nOrigin: %s\nDestination: %s\nBus Number: %s\nSeats: %d total, %d available\n", trip.trip_id, trip.origin, trip.destination, trip.bus_number, total_seats, available_seats);

    printf("\n==================== SEAT MAP ====================\n");
    printf("    A   B    C   D\n");
    for (int row = 0; row < ROWS; row++) {
        printf("%-2d ", row + 1);
        for (int col = 0; col < COLS; col++) {
            if (trip.seats[row][col].is_reserved)
                printf("[X]");
            else
                printf("[ ]");

            // Add spacing for aisle visualization
            if (col == 1)
                printf("  ");  // two spaces for aisle
            else
                printf(" ");   // single space between other seats
        }
        printf("\n");
    }
    printf("X = reserved, [ ] = available\n");

    // Seat picking
    int seat_row;
    char seat_col;
    int col_idx = 999;
    bool valid_seat = false;

    while (!valid_seat) {
        printf("\nEnter desired seat (ex: 3B): ");
        scanf("%d%c", &seat_row, &seat_col);
        seat_col = toupper(seat_col);

        switch (seat_col) {
            case 'A': 
                col_idx = 0; 
                break;
            case 'B': 
                col_idx = 1; 
                break;
            case 'C': 
                col_idx = 2; 
                break;
            case 'D': 
                col_idx = 3; 
                break;
            default:
                printf("Invalid seat column. Please use A-D.\n");
                continue;
        }

        if (seat_row < 1 || seat_row > ROWS) {
            printf("Invalid row number. Please use 1-%d.\n", ROWS);
            continue;
        }

        // Check reservation status
        if (trip.seats[seat_row - 1][col_idx].is_reserved) {
            printf("Seat %d%c is already reserved. Please pick another.\n", seat_row, seat_col);
        } else {
            valid_seat = true;
        }
    }

    // Reserver info
    printf("Enter your first name: ");
    scanf("%s", first_name);
    getchar();
    printf("Enter your last name: ");
    scanf("%s", last_name);
    getchar();

    // Set reservation data
    strcpy(new_res.first_name, first_name);
    strcpy(new_res.last_name, last_name);
    strcpy(new_res.trip_id, trip.trip_id);
    new_res.seat_row = seat_row - 1;
    new_res.seat_col = seat_col;
    new_res.col_idx = col_idx;

    int reservation_number = total_seats - available_seats + 1;
    sprintf(new_res.reservation_id, "%s-%d", trip.trip_id, reservation_number);

    // Reservation confirmation
    printf("You are about to reserve seat %d%c for trip %s. Confirm? (y/n) ", seat_row, seat_col, trip.trip_id);
    char confirm;
    scanf(" %c", &confirm);

    if (tolower(confirm) != 'y') {
        printf("Canceled reservation.\n");
        return;
    }

    if (save_res_to_db(&new_res) == 0) {
        trip.seats[seat_row - 1][col_idx].is_reserved = true;
        trip.seats[seat_row - 1][col_idx].reservation = new_res;
        save_trip_to_db(&trip);
        printf("%sYour reservation ID is: %s\nSave this in case your reservation needs to be modified.\n", DIVIDER, new_res.reservation_id);
    }
}

void view_reservation() {
    GDBM_FILE dbf = gdbm_open(RES_DATABASE, 512, GDBM_READER, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "view reservation error: database could not be retrieved or created\n");
        return;
    }
    
    // Reservation database key lookup
    char key_name[MAX_STRING];
    datum key;

    printf("\nEnter your reservation ID, or enter \'q\' to quit: ");
    scanf("%s", key_name);
    key_name[strcspn(key_name, "\n")] = 0;  // remove newline

    key.dptr = key_name;
    key.dsize = strlen(key_name) + 1;

    while ((strcmp(key_name, "q") != 0) && (strcmp(key_name, "Q") != 0) && !gdbm_exists(dbf, key)) {
        printf("Reservation does not exist.\nEnter your reservation ID, or enter \'q\' to quit: ");
        scanf("%s", key_name);
        key.dptr = key_name;
        key.dsize = strlen(key_name) + 1;
    }
    if (strcmp(key_name, "q") == 0 || strcmp(key_name, "Q") == 0) {
        gdbm_close(dbf);
        return;
    }

    // Fetch requested trip
    datum value = gdbm_fetch(dbf, key);
    static Reservation res;

    if (value.dptr == NULL) {
        fprintf(stderr, "view reservation error: unable to get value of reservation from key.\n");
        gdbm_close(dbf);
        return;
    } else {
        memcpy(&res, value.dptr, sizeof(Reservation));
        free(value.dptr);
    }
    gdbm_close(dbf);

    printf("%sFirst name: %s\nLast name: %s\nTrip: %s\nSeat: %d%c\n", DIVIDER, res.first_name, res.last_name, res.trip_id, res.seat_row, res.seat_col);
}

void update_reservation() {
    GDBM_FILE dbf = gdbm_open(RES_DATABASE, 512, GDBM_READER, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "update reservation error: database could not be retrieved or created\n");
        return;
    }
    
    // Reservation database key lookup
    char key_name[MAX_STRING];
    datum key;

    printf("\nEnter your reservation ID, or enter \'q\' to quit: ");
    scanf("%s", key_name);
    key_name[strcspn(key_name, "\n")] = 0;  // remove newline

    key.dptr = key_name;
    key.dsize = strlen(key_name) + 1;

    while ((strcmp(key_name, "q") != 0) && (strcmp(key_name, "Q") != 0) && !gdbm_exists(dbf, key)) {
        printf("Reservation does not exist.\nEnter your reservation ID, or enter \'q\' to quit: ");
        scanf("%s", key_name);
        key.dptr = key_name;
        key.dsize = strlen(key_name) + 1;
    }
    if (strcmp(key_name, "q") == 0 || strcmp(key_name, "Q") == 0) {
        gdbm_close(dbf);
        return;
    }

    // Fetch requested trip
    datum value = gdbm_fetch(dbf, key);
    static Reservation res;

    if (value.dptr == NULL) {
        fprintf(stderr, "update reservation error: unable to get value of reservation from key.\n");
        gdbm_close(dbf);
        return;
    } else {
        memcpy(&res, value.dptr, sizeof(Reservation));
        free(value.dptr);
    }
    gdbm_close(dbf);

    // Prompt user to choose which trip metadata to alter
    int choice;
    char confirm;
    char entry[MAX_STRING];

    while (1) {
        printf("\n=== Choose Reservation Info to Update ===\n");
        printf("1. First Name\n");
        printf("2. Last Name\n");
        printf("0. Exit\n");

        // Get choice from user
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Current first name: %s\n", res.first_name);
                printf("Enter new first name: ");
                scanf("%s", entry);
                getchar();

                printf("Change: %s -> %s\n", res.first_name, entry);
                printf("Enter 'y' to confirm. Otherwise, enter anything else: ");
                confirm = getchar();
                getchar();

                if (confirm == 'y' || confirm == 'Y') {
                    strcpy(res.first_name, entry);
                    printf("First name updated successfully.\n");
                } else {
                    printf("Canceled change to first name.\n");
                }
                break;

            case 2:
                printf("Current last name: %s\n", res.last_name);
                printf("Enter new last name: ");
                scanf("%s", entry);
                getchar();

                printf("Change: %s -> %s\n", res.last_name, entry);
                printf("Enter 'y' to confirm. Otherwise, enter anything else: ");
                confirm = getchar();
                getchar();

                if (confirm == 'y' || confirm == 'Y') {
                    strcpy(res.last_name, entry);
                    printf("Last name updated successfully.\n");
                } else {
                    printf("Canceled change to last name.\n");
                }
                break;

            case 0:
                // Save changes to DB before exiting
                save_res_to_db(&res);
                return;

            default:
                printf("Invalid choice.\n");
        }
    }

}

void delete_reservation() {
    GDBM_FILE dbf = gdbm_open(RES_DATABASE, 512, GDBM_WRCREAT, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "delete reservation error: database could not be retrieved or created\n");
        return;
    }

    printf("Enter your reservation ID for deletion: ");
    char key_name[MAX_STRING];
    datum key;
    scanf("%s", key_name);
    while (getchar() != '\n'); // flush buffer
    key_name[strcspn(key_name, "\n")] = 0;

    key.dptr = key_name;
    key.dsize = strlen(key_name) + 1;

    // Confirmation
    char confirm;
    printf("Are you sure you want to delete this reservation?\nEnter \'y\' to confirm. Otherwise, enter anything else: ");
    confirm = getchar();
    getchar();

    if (confirm != 'y' && confirm != 'Y') {
        printf("Canceled reservation deletion.\n");
        gdbm_close(dbf);
    }
    if (gdbm_exists(dbf, key) != 1) {
        printf("Reservation not found. Did you enter the correct reservation ID?\n");
        gdbm_close(dbf);
        return;
    }

    // Get reservation information
    datum value = gdbm_fetch(dbf, key);
    static Reservation res;
    if (value.dptr == NULL) {
        fprintf(stderr, "delete reservation error: unable to get value of reservation from key.\n");
        gdbm_close(dbf);
        return;
    } else {
        memcpy(&res, value.dptr, sizeof(Reservation));
        free(value.dptr);
    }

    char trip_id[MAX_STRING];
    strcpy(trip_id, res.trip_id);
    int seat_row = res.seat_row;
    int seat_col = res.col_idx;

    // Get trip reservation belongs to
    GDBM_FILE bus_database = gdbm_open(BUS_DATABASE, 512, GDBM_READER, 0644, 0);
    if (!bus_database) {
        fprintf(stderr, "delete reservation error: bus database could not be retrieved or created\n");
        return;
    }

    datum trip_key;
    trip_key.dptr = trip_id;
    trip_key.dsize = strlen(trip_id) + 1;
    datum trip_value = gdbm_fetch(bus_database, trip_key);

    static Trip trip;
    if (trip_value.dptr == NULL) {
        fprintf(stderr, "delete reservation error: unable to fetch trip.\n");
        gdbm_close(dbf);
        gdbm_close(bus_database);
        return;
    } else {
        memcpy(&trip, trip_value.dptr, sizeof(Trip));
        free(trip_value.dptr);
    }
    
    // Delete reservation from trip
    trip.seats[seat_row][seat_col].is_reserved = false;
    memset(&trip.seats[seat_row][seat_col].reservation, 0, sizeof(Reservation));
    gdbm_close(bus_database);
    if (save_trip_to_db(&trip) != 0) {
        printf("Awww man!");
        return;
    }

    // Delete reservation from database
    gdbm_delete(dbf, key);
    gdbm_close(dbf);
    printf("%sDeleted reservation %s from system.\n", DIVIDER, key_name);
    

    return;
}

int save_res_to_db(Reservation *res) {
    // Open or create the bus database
    GDBM_FILE dbf = gdbm_open(RES_DATABASE, 512, GDBM_WRCREAT, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "save reservation error: database could not be retrieved or created\n");
        return 1;
    }

    // Create a key, value pair from the trip
    // Trips in the GDBM database are identified by their trip_id
    datum key;
    key.dptr = res->reservation_id;
    key.dsize = strlen(res->reservation_id) + 1;
    datum value;
    value.dptr = (char *)res;
    value.dsize = sizeof(*res);

    // Insert the key, value pair into the database
    if (gdbm_store(dbf, key, value, GDBM_REPLACE) != 0) {
        fprintf(stderr, "save error: could not add reservation to database\n");
        gdbm_close(dbf);
        return 1;
    }
    printf("Reservation %s successfully saved to database.\n", res->reservation_id);
    gdbm_close(dbf);
    return 0;
}