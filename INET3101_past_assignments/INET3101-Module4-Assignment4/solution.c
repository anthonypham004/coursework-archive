#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NAMELEN 30
#define MAXSEATS 24
#define FILENAME "flight_details.bin"

enum mainMenu { OUTBOUND = 'a', INBOUND = 'b', SAVE_QUIT = 'c', QUIT_NO_SAVE = 'd' };
enum flightMenu { NUM_EMPTY = 'a', LIST_EMPTY = 'b', ALPHA_SEATS = 'c', ASSIGN_SEAT = 'd', DELETE_SEAT = 'e', MAIN_MENU = 'f' };

struct Seat {
    int id;
    _Bool taken;
    char lastName[NAMELEN];
    char firstName[NAMELEN];
};
struct Seat outboundFlight[MAXSEATS];
struct Seat inboundFlight[MAXSEATS];

void flight_manager(struct Seat flight[]);
void alphabetize(struct Seat sorted[], const struct Seat flight[]);
void readFromFile(FILE *f);
void writeToFile(FILE *f);

int main(int argc, char *argv[]) {
    // Initial file handling
    
    // Look for the file on startup. If found, open and load contents.
    printf("Looking for file...\n");
    FILE *f = fopen(FILENAME, "rb+");
    _Bool file_existed = 1; // Determines whether or not to load data from the file

    if (!f) {
        printf("File not found. Creating new file...\n");
        f = fopen(FILENAME, "wb+"); // Creates file if missing
        file_existed = 0;
    }

    // Second check in case file needed to be created
    if (!f) {
        fprintf(stderr, "Failed to open or create file. Terminating program.\n");
        exit(1);
    }
    printf("File is ready for use.\n");

    if (file_existed) {
        readFromFile(f);
        fclose(f);
    } else {
        // Flight arrays default initialization
        for (int i = 0; i < MAXSEATS; i++) {
            outboundFlight[i].id = i;
            outboundFlight[i].taken = 0;
            strcpy(outboundFlight[i].lastName, "-----");
            strcpy(outboundFlight[i].firstName, "-----");

            inboundFlight[i].id = i;
            inboundFlight[i].taken = 0;
            strcpy(inboundFlight[i].lastName, "-----");
            strcpy(inboundFlight[i].firstName, "-----");
        }
        printf("Initializing file...\n");
        writeToFile(f);
        fclose(f);
    }
    
    
    // Flight data handling

    char choice;
    while (1) {
        printf("\na) Outbound Flight\nb) Inbound Flight\nc) Save and Quit\nd) Quit Without Saving\n");
        printf("Enter your selection > ");
        scanf(" %c", &choice);

        switch (choice) {
            case OUTBOUND:
                flight_manager(outboundFlight);
                break;
            case INBOUND:
                flight_manager(inboundFlight);
                break;
            case SAVE_QUIT:
                // Save before terminating
                f = fopen(FILENAME, "wb");
                printf("Saving contents...\n");
                writeToFile(f);
                fclose(f);
                return 0;
            case QUIT_NO_SAVE:
                printf("Really quit without saving? (y to confirm) > ");
                scanf(" %c", &choice);
                if (choice == 'y' || choice == 'Y') {
                    return 0;
                }
                break;
        }
    }

    return 0;
}

// Handles all the user actions for the selected flight. 
void flight_manager(struct Seat flight[]) {
    char choice;
    do {
        printf("\na) Show number of empty seats\nb) Show list of empty seats\nc) Show alphabetical list of seats\nd) Assign a customer to a seat assignment\ne) Delete a seat assignment\nf) Return to Main menu");
        printf("\nEnter your selection > ");
        scanf(" %c", &choice);

        int emptySeats = 0;
        // vars for holding user input
        int chosenSeat;
        char newLastName[NAMELEN];
        char newFirstName[NAMELEN];
        char confirmEntry;
        switch (choice) {
            case NUM_EMPTY:
                for (int i = 0; i < MAXSEATS; i++) {
                    if (flight[i].taken == 0) {
                        emptySeats++;
                    }
                }
                printf("EMPTY SEATS AVAILABLE: %d\n", emptySeats);
                break;

            case LIST_EMPTY:
                printf("LIST OF EMPTY SEATS: [");
                for (int i = 0; i < MAXSEATS; i++) {
                    if (flight[i].taken == 0) {
                        printf("%d, ", flight[i].id + 1);
                    }
                }
                printf("\b \b\b]\n"); // at the end, remove the extra comma and space and add an end bracket
                break;

            case ALPHA_SEATS:
                printf("ALPHABETICAL LIST OF SEATS:\n");
                struct Seat sorted[MAXSEATS]; // make a new array that'll hold the alphabetically sorted seats
                alphabetize(sorted, flight);
                // first loop -- first list the seats w/ passengers
                for (int i = 0; i < MAXSEATS; i++) {
                    if (strcmp(sorted[i].lastName, "-----") != 0) {
                        printf("SEAT %d: %s, %s\n", sorted[i].id + 1, sorted[i].lastName, sorted[i].firstName);
                    }
                }
                // second loop -- list the rest of the empty seats
                for (int i = 0; i < MAXSEATS; i++) {
                    if (strcmp(sorted[i].lastName, "-----") == 0) {
                        printf("SEAT %d: %s, %s\n", sorted[i].id + 1, sorted[i].lastName, sorted[i].firstName);
                    }
                }
                break;

            case ASSIGN_SEAT:
                // assigned these at the beginning so that old input isn't reused (just in case)
                confirmEntry = 'z';
                chosenSeat = 999;
                do {
                    printf("\nENTER SEAT NUMBER TO ASSIGN > ");
                    int result = scanf("%d", &chosenSeat); // needed for cases where non-int input is entered (an infinite loop would happen otherwise)
                    
                    // Non-int input handler
                    if (result != 1) {
                        printf("ERROR: ENTRY MUST BE AN INTEGER.\n");
                        chosenSeat = 999;
                        while (getchar() != '\n'); // flush the buffer
                        continue;
                    }
                    if (chosenSeat < 1 || chosenSeat > MAXSEATS) {
                        printf("\nERROR: SELECTED SEAT DOES NOT EXIST IN FLIGHT.\nPLEASE ENTER A SEAT NUMBER WITHIN 1-%d.\n", MAXSEATS);
                    }
                } while (chosenSeat < 1 || chosenSeat > MAXSEATS);
                
                printf("PASSENGER LAST NAME: ");
                scanf("%s", newLastName);
                printf("PASSENGER FIRST NAME: ");
                scanf("%s", newFirstName);

                // confirmation. if entry is confirmed, the details are written into the Seat's data.
                printf("\nASSIGNED SEAT: %d\nPASSENGER LAST NAME: %s\nPASSENGER FIRST NAME: %s\nARE THESE DETAILS CORRECT? (y/n) > ", chosenSeat, newLastName, newFirstName);
                while (confirmEntry != 'n' && confirmEntry != 'N' && confirmEntry != 'y' && confirmEntry != 'Y') {
                    scanf(" %c", &confirmEntry);
                    if (confirmEntry == 'N' || confirmEntry == 'n') {
                        break;
                    } else if (confirmEntry == 'Y' || confirmEntry == 'y') {
                        flight[chosenSeat - 1].taken = 1;
                        strcpy(flight[chosenSeat - 1].lastName, newLastName);
                        strcpy(flight[chosenSeat - 1].firstName, newFirstName);
                    }
                }
                break;

            case DELETE_SEAT:
                confirmEntry = 'z';
                chosenSeat = 999;
                do {
                    printf("\nENTER SEAT NUMBER TO DELETE ASSIGNMENT > ");
                    int result = scanf("%d", &chosenSeat);
                    // non-int input handler
                    if (result != 1) {
                        printf("ERROR: ENTRY MUST BE AN INTEGER.\n");
                        chosenSeat = 999;
                        while (getchar() != '\n'); // flush the buffer
                        continue;
                    }
                    if (chosenSeat < 1 || chosenSeat > MAXSEATS) {
                        printf("\nERROR: SELECTED SEAT DOES NOT EXIST IN FLIGHT.\nPLEASE ENTER A SEAT NUMBER WITHIN 1-%d.\n", MAXSEATS);
                    }
                } while (chosenSeat < 1 || chosenSeat > MAXSEATS);

                printf("\nASSIGNMENT FOR SEAT %d SEATING PASSENGER %s, %s WILL BE DELETED.\nDO YOU WISH TO CONTINUE? (y/n) > ", chosenSeat, flight[chosenSeat - 1].lastName, flight[chosenSeat - 1].firstName);
                while (confirmEntry != 'n' && confirmEntry != 'N' && confirmEntry != 'y' && confirmEntry != 'Y') {
                    scanf(" %c", &confirmEntry);
                    if (confirmEntry == 'N' || confirmEntry == 'n') {
                        break;
                    } else if (confirmEntry == 'Y' || confirmEntry == 'y') {
                        flight[chosenSeat - 1].taken = 0;
                        strcpy(flight[chosenSeat - 1].lastName, "-----");
                        strcpy(flight[chosenSeat - 1].firstName, "-----");
                    }
                }
                break;

            case MAIN_MENU:
                return;
        }
    } while (choice != MAIN_MENU);
}

// Helper function that sorts the list of names in a flight alphabetically
void alphabetize(struct Seat sorted[], const struct Seat flight[]) {
    for (int i = 0; i < MAXSEATS; i++) {
        sorted[i] = flight[i];
    }
    // bubble sort to arrange seats by last name then first name alphabetically
    for (int i = 0; i < MAXSEATS - 1; i++) {
        for (int j = 0; j < MAXSEATS - i - 1; j++) {
            if ((strcmp(sorted[j].lastName, sorted[j+1].lastName) > 0) || (strcmp(sorted[j].lastName, sorted[j+1].lastName) == 0 && strcmp(sorted[j].firstName, sorted[j+1].firstName) > 0)) {
                struct Seat temp = sorted[j];
                sorted[j] = sorted[j+1];
                sorted[j+1] = temp;
            }
        }
    }
}

// Loads contents from file into flight arrays
void readFromFile(FILE *f) {
    printf("Reading from file...\n");
    if (!f) {
        fprintf(stderr, "File invalid. Aborting process.\n");
        return;
    }
    fread(outboundFlight, sizeof(struct Seat), MAXSEATS, f);
    fread(inboundFlight, sizeof(struct Seat), MAXSEATS, f);
    printf("Done.\n");
}

// Writes flight array data onto file
void writeToFile(FILE *f) {
    printf("Writing to file...\n");
    if (!f) {
        fprintf(stderr, "File invalid. Aborting process.\n");
        return;
    }
    fwrite(outboundFlight, sizeof(struct Seat), MAXSEATS, f);
    fwrite(inboundFlight, sizeof(struct Seat), MAXSEATS, f);
    printf("Done.\n");
}