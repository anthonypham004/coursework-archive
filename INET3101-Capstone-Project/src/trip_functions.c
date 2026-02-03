#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gdbm.h>

#include "bus.h"

// TRIP FUNCTIONS

/*
Creates a new trip.
Asks for the trip's origin, destination, and a bus number.
Makes a trip_id of value ORIGIN-DESTINATION-BUS_NUMBER, using the first 3 letters of origin and destinaton.
Initializes seats array by assigning seat_id's and default reservation info values.
Once saved to GDBM database, trip is identified by its key set as it's trip_id.
*/
void create_trip() {
    Trip new_trip;

    char origin[MAX_STRING];
    char destination[MAX_STRING];
    char bus_number[4];
    
    // Input trip details
    printf(DIVIDER);
    printf("Enter the trip origin: ");
    scanf("%s", origin);
    getchar();
    printf("Enter the trip destination: ");
    scanf("%s", destination);
    getchar();
    printf("Enter the 3-digit bus number making this trip: ");
    scanf("%3s", bus_number);
    //getchar();

    // Capitalize origin and destination strings
    for (int i = 0; origin[i] != '\0'; i++) {
        origin[i] = toupper((unsigned int) origin[i]);
    }
    for (int i = 0; destination[i] != '\0'; i++) {
        destination[i] = toupper((unsigned int) destination[i]);
    }

    // Format the trip ID
    char trip_id[MAX_STRING];
    strncpy(trip_id, origin, 3);
    trip_id[3] = '\0';

    strcat(trip_id, "-");
    strncat(trip_id, destination, 3);

    strcat(trip_id, "-");
    strcat(trip_id, bus_number);

    // Confirm trip details
    printf("%sBus Number: %s\nOrigin: %s\nDestination: %s\nTrip ID: %s\n", DIVIDER, bus_number, origin, destination, trip_id);
    printf("Do these details look correct? (y/n): ");
    char answer;
    scanf(" %c", &answer);
    getchar();
    while (answer != 'y' && answer != 'Y' && answer != 'n'&& answer != 'N') {
        printf("Enter \'y\' or \'n\': ");
        scanf("%c", &answer);
        getchar();
    }
    if (answer == 'n' || answer == 'N') {
        printf("%sCanceled creation of trip %s.\n", DIVIDER, trip_id);
        return;
    }
    printf(DIVIDER);

    // Set the trip struct's attributes
    strcpy(new_trip.bus_number, bus_number);
    strcpy(new_trip.origin, origin);
    strcpy(new_trip.destination, destination);
    strcpy(new_trip.trip_id, trip_id);


    // Initialize bus seats
    char seatLetters[] = {'A', 'B', 'C', 'D'};
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            // Assign the seat ID
            char seat_id[4] = "";
            char row_num[3];
            char col_letter[2] = {seatLetters[col], '\0'};

            sprintf(row_num, "%d", row + 1);
            strcat(seat_id, row_num);
            strcat(seat_id, col_letter);
            strcpy(new_trip.seats[row][col].seat_id, seat_id);
            
            // Set the seat's reservation information
            memset(&new_trip.seats[row][col].reservation, 0, sizeof(Reservation));
            new_trip.seats[row][col].is_reserved = false;
        }
    }

    /* 
    Check if there's already a trip w/ the same trip_id in database.
    If not, store the trip in the database.
    Otherwise, warn the user of the existing trip and ask to replace it.
    */
    GDBM_FILE dbf = gdbm_open(BUS_DATABASE, 512, GDBM_WRCREAT, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "create trip error: database could not be retrieved or created\n");
        return;
    }

    datum key;
    key.dptr = new_trip.trip_id;
    key.dsize = strlen(new_trip.trip_id) + 1;

    if (gdbm_exists(dbf, key) == 1) {
        gdbm_close(dbf);
        // Warn user of existing trip in database
        printf("WARNING: trip with the same trip ID found in database!\n");
        printf("Enter \"YES\" to overwrite this existing trip with a new empty one.\nOtherwise, enter anything else: ");
        char answer[MAX_STRING];
        scanf("%s", answer);
        getchar();

        // Overwrite existing trip
        if (strcmp(answer, "YES") == 0) {
            if (save_trip_to_db(&new_trip) == 0) {
                printf("Trip %s successfully overwritten.\n", new_trip.trip_id);
            }
        } else {
            printf("Canceled creation of trip %s.\n", new_trip.trip_id);
        }
    } else {
        gdbm_close(dbf);
        if (save_trip_to_db(&new_trip) == 0) {
            printf("Trip %s successfully created.\n", new_trip.trip_id);
        }
    }
}

// Prints the details of the requested trip, as well as a seat map showing which seats are reserved
void view_trip() {
    Trip *trip_ptr = get_trip();
    Trip trip;
    if (trip_ptr == NULL) {
        return;
    } else {
        trip = *trip_ptr;
    }

    printf("\n==================== TRIP INFO ====================\n");
    int total_seats = ROWS * COLS;
    int available_seats = total_seats;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (trip.seats[row][col].is_reserved == false) {
                available_seats--;
            }
        }
    }

    printf("Trip ID: %s\nOrigin: %s\nDestination: %s\nBus Number: %s\nSeats: %d total, %d reserved\n", trip.trip_id, trip.origin, trip.destination, trip.bus_number, total_seats, available_seats);


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

}

// Allows user to update the selected trip's metadata
void update_trip() {
    Trip *trip_ptr = get_trip();

    Trip trip;
    if (trip_ptr == NULL) {
        return;
    } else {
        trip = *trip_ptr;
    }

    // Prompt user to choose which trip metadata to alter
    int choice;
    char confirm;
    char entry[MAX_STRING];

    while (1) {
        printf("\n=== Choose Trip Data to Update ===\n");
        printf("1. Origin\n");
        printf("2. Destination\n");
        printf("3. Bus Number\n");
        printf("0. Exit\n");

        // Get choice from user
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Current origin: %s\n", trip.origin);
                printf("Enter new origin: ");
                scanf("%s", entry);
                getchar();

                printf("Change: %s -> %s\n", trip.origin, entry);
                printf("Enter 'y' to confirm. Otherwise, enter anything else: ");
                confirm = getchar();
                getchar();

                if (confirm == 'y' || confirm == 'Y') {
                    strcpy(trip.origin, entry);
                    printf("Origin updated successfully.\n");
                } else {
                    printf("Canceled change to origin.\n");
                }
                break;

            case 2:
                printf("Current destination: %s\n", trip.destination);
                printf("Enter new destination: ");
                scanf("%s", entry);
                getchar();

                printf("Change: %s -> %s\n", trip.destination, entry);
                printf("Enter 'y' to confirm. Otherwise, enter anything else: ");
                confirm = getchar();
                getchar();

                if (confirm == 'y' || confirm == 'Y') {
                    strcpy(trip.destination, entry);
                    printf("Destination updated successfully.\n");
                } else {
                    printf("Canceled change to destination.\n");
                }
                break;

            case 3:
                printf("Current bus number: %s\n", trip.bus_number);
                printf("Enter new bus number: ");
                scanf("%s", entry);
                getchar();

                printf("Change: %s -> %s\n", trip.bus_number, entry);
                printf("Enter 'y' to confirm. Otherwise, enter anything else: ");
                confirm = getchar();
                getchar();

                if (confirm == 'y' || confirm == 'Y') {
                    strcpy(trip.bus_number, entry);
                    printf("Bus number updated successfully.\n");
                } else {
                    printf("Canceled change to bus number.\n");
                }
                break;

            case 0:
                // Save changes to DB before exiting
                save_trip_to_db(&trip);
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}

// Trip searching helper function. Gets the pointer of a requested trip from database.
Trip *get_trip() {
    printf("================= AVAILABLE TRIPS =================\n");
    print_db_keys();

    GDBM_FILE dbf = gdbm_open(BUS_DATABASE, 512, GDBM_READER, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "get trip error: database could not be retrieved or created\n");
        return NULL;
    }
    
    // Trip database key lookup
    char key_name[MAX_STRING];
    datum key;

    printf("\nEnter the ID of the trip you want to get, or enter \'q\' to quit: ");
    scanf("%s", key_name);
    key_name[strcspn(key_name, "\n")] = 0;  // remove newline

    key.dptr = key_name;
    key.dsize = strlen(key_name) + 1;

    while ((strcmp(key_name, "q") != 0) && (strcmp(key_name, "Q") != 0) && !gdbm_exists(dbf, key)) {
        printf("Trip does not exist.\nEnter the ID of the trip you want to get, or enter \'q\' to quit: ");
        scanf("%s", key_name);
        key.dptr = key_name;
        key.dsize = strlen(key_name) + 1;
    }
    if (strcmp(key_name, "q") == 0 || strcmp(key_name, "Q") == 0) {
        gdbm_close(dbf);
        return NULL;
    }

    // Fetch requested trip
    datum value = gdbm_fetch(dbf, key);
    static Trip trip;

    if (value.dptr == NULL) {
        fprintf(stderr, "get trip error: unable to get value of trip from key.\n");
        gdbm_close(dbf);
        return NULL;
    } else {
        memcpy(&trip, value.dptr, sizeof(Trip));
        free(value.dptr);
    }
    gdbm_close(dbf);

    return &trip;
}

int save_trip_to_db(Trip *trip) {
    // Open or create the bus database
    GDBM_FILE dbf = gdbm_open(BUS_DATABASE, 512, GDBM_WRCREAT, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "save trip error: database could not be retrieved or created\n");
        return 1;
    }

    // Create a key, value pair from the trip
    // Trips in the GDBM database are identified by their trip_id
    datum key;
    key.dptr = trip->trip_id;
    key.dsize = strlen(trip->trip_id) + 1;
    datum value;
    value.dptr = (char *)trip;
    value.dsize = sizeof(*trip);

    // Insert the key, value pair into the database
    if (gdbm_store(dbf, key, value, GDBM_REPLACE) != 0) {
        fprintf(stderr, "save error: could not add trip to database\n");
        gdbm_close(dbf);
        return 1;
    }
    printf("Trip %s successfully saved to database.\n", trip->trip_id);
    gdbm_close(dbf);
    return 0;
}

// Deletes the requested trip from database
void delete_trip() {
    printf("================= AVAILABLE TRIPS =================\n");
    print_db_keys();

    GDBM_FILE dbf = gdbm_open(BUS_DATABASE, 512, GDBM_WRCREAT, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "delete error: database could not be retrieved or created\n");
        return;
    }

    // Trip database key lookup
    char key_name[MAX_STRING];
    datum key;

    printf("\nEnter the ID of the trip you want to delete, or enter \'q\' to quit: ");
    scanf("%s", key_name);
    key_name[strcspn(key_name, "\n")] = 0;  // remove newline

    key.dptr = key_name;
    key.dsize = strlen(key_name) + 1;

    while ((strcmp(key_name, "q") != 0) && (strcmp(key_name, "Q") != 0) && !gdbm_exists(dbf, key)) {
        printf("Trip does not exist.\nEnter the ID of the trip you want to delete, or enter \'q\' to quit: ");
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
    static Trip trip;

    if (value.dptr == NULL) {
        fprintf(stderr, "delete trip error: unable to get value of trip from key.\n");
        gdbm_close(dbf);
        return;
    } else {
        memcpy(&trip, value.dptr, sizeof(Trip));
        free(value.dptr);
    }

    int total_seats = ROWS * COLS;
    int reserved_seats = 0;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (trip.seats[row][col].is_reserved == true) {
                reserved_seats++;
            }
        }
    }

    // Delete confirmation
    printf("%sYou are about to delete trip %s with details\nOrigin: %s\nDestination: %s\nBus Number: %s\nWith %d total seats and %d seats reserved.\n", DIVIDER, trip.trip_id, trip.origin, trip.destination, trip.bus_number, total_seats, reserved_seats);

    char answer[MAX_STRING];
    printf("\nDo you wish to continue? Enter \"YES\" to delete, otherwise enter anything else: ");
    scanf("%s", answer);
    if (strcmp(answer, "YES") == 0) {
        gdbm_delete(dbf, key);
        printf("Trip %s has been deleted from the database.\n", trip.trip_id);
    } else {
        printf("Canceled deletion of trip %s.\n", trip.trip_id);
    }
    gdbm_close(dbf);
    return;
}

// DATABASE UTILITY FUNCTIONS

void print_db_keys() {
    GDBM_FILE dbf = gdbm_open(BUS_DATABASE, 512, GDBM_WRCREAT, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "print dbf error: database could not be retrieved or created\n");
        return;
    }
    
    datum k = gdbm_firstkey(dbf);
    while (k.dptr) {
        printf("%.*s\n", k.dsize, k.dptr);
        datum next = gdbm_nextkey(dbf, k);
        free(k.dptr);
        k = next;
    }
    gdbm_close(dbf);
    return;
}

void print_db_keys_pretty() {
    GDBM_FILE dbf = gdbm_open(BUS_DATABASE, 512, GDBM_WRCREAT, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "print dbf error: database could not be retrieved or created\n");
        return;
    }
    
    printf("---------- ALL KEYS IN DATABASE ----------\n");
    datum k = gdbm_firstkey(dbf);
    while (k.dptr) {
        printf("Key: %.*s\n", k.dsize, k.dptr);
        datum next = gdbm_nextkey(dbf, k);
        free(k.dptr);
        k = next;
    }
    gdbm_close(dbf);
    return;
}

void print_res_keys() {
    GDBM_FILE dbf = gdbm_open(RES_DATABASE, 512, GDBM_WRCREAT, 0644, 0);
    if (!dbf) {
        fprintf(stderr, "print dbf error: database could not be retrieved or created\n");
        return;
    }
    
    printf("---------- ALL KEYS IN DATABASE ----------\n");
    datum k = gdbm_firstkey(dbf);
    while (k.dptr) {
        printf("Key: %.*s\n", k.dsize, k.dptr);
        datum next = gdbm_nextkey(dbf, k);
        free(k.dptr);
        k = next;
    }
    gdbm_close(dbf);
    return;
}