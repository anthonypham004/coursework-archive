#include <stdio.h>
#include <string.h>

#define NAMELEN 30
#define MAXSEATS 24

struct Seat {
        int id;
        _Bool taken;
        char lastName[NAMELEN];
        char firstName[NAMELEN];
    };

void flight_manager(struct Seat flight[]);
void alphabetize(struct Seat sorted[], struct Seat flight[]);

int main() {
    // flight arrays initialization
    struct Seat outboundFlight[MAXSEATS];
    struct Seat inboundFlight[MAXSEATS];
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
    
    char choice;
    do {
        printf("\na) Outbound Flight\nb) Inbound Flight\nc) Quit\n");
        printf("Enter your selection: ");
        scanf(" %c", &choice);

        if (choice == 'c') {
            return 0;
        } else if (choice == 'a') {
            flight_manager(outboundFlight);
        } else if (choice == 'b') {
            flight_manager(inboundFlight);
        }
    } while (scanf("%c", &choice) == 1);

    return 0;
}

// this function handles all the user actions for the selected flight. 
void flight_manager(struct Seat flight[]) {
    char choice;
    do {
        printf("\na) Show number of empty seats\nb) Show list of empty seats\nc) Show alphabetical list of seats\nd) Assign a customer to a seat assignment\ne) Delete a seat assignment\nf) Return to Main menu");
        printf("\nEnter your selection: ");
        scanf(" %c", &choice);

        int emptySeats = 0;
        // vars for holding user input
        int chosenSeat;
        char newLastName[NAMELEN];
        char newFirstName[NAMELEN];
        char confirmEntry;
        switch (choice) {
            case 'a':
                for (int i = 0; i < MAXSEATS; i++) {
                    if (flight[i].taken == 0) {
                        emptySeats++;
                    }
                }
                printf("EMPTY SEATS AVAILABLE: %d\n", emptySeats);
                break;

            case 'b':
                printf("LIST OF EMPTY SEATS: [");
                for (int i = 0; i < MAXSEATS; i++) {
                    if (flight[i].taken == 0) {
                        printf("%d, ", flight[i].id + 1);
                    }
                }
                printf("\b \b\b]\n"); // at the end, remove the extra comma and space and add an end bracket
                break;

            case 'c':
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

            case 'd':
                // assigned these at the beginning so that old input isn't reused (just in case)
                confirmEntry = 'z';
                chosenSeat = 999;
                do {
                    printf("\nENTER SEAT NUMBER TO ASSIGN: ");
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
                printf("\nASSIGNED SEAT: %d\nPASSENGER LAST NAME: %s\nPASSENGER FIRST NAME: %s\nARE THESE DETAILS CORRECT? (y/n): ", chosenSeat, newLastName, newFirstName);
                while (confirmEntry != 'n' && confirmEntry != 'N' && confirmEntry != 'y' && confirmEntry != 'Y') {
                    scanf("%c", &confirmEntry);
                    if (confirmEntry == 'N' || confirmEntry == 'n') {
                        break;
                    } else if (confirmEntry == 'Y' || confirmEntry == 'y') {
                        flight[chosenSeat - 1].taken = 1;
                        strcpy(flight[chosenSeat - 1].lastName, newLastName);
                        strcpy(flight[chosenSeat - 1].firstName, newFirstName);
                    }
                }
                break;

            case 'e':
                confirmEntry = 'z';
                chosenSeat = 999;
                do {
                    printf("\nENTER SEAT NUMBER TO DELETE ASSIGNMENT: ");
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

                printf("\nASSIGNMENT FOR SEAT %d WILL BE DELETED.\nDO YOU WISH TO CONTINUE? (y/n): ", chosenSeat);
                while (confirmEntry != 'n' && confirmEntry != 'N' && confirmEntry != 'y' && confirmEntry != 'Y') {
                    scanf("%c", &confirmEntry);
                    if (confirmEntry == 'N' || confirmEntry == 'n') {
                        break;
                    } else if (confirmEntry == 'Y' || confirmEntry == 'y') {
                        flight[chosenSeat - 1].taken = 0;
                        strcpy(flight[chosenSeat - 1].lastName, "-----");
                        strcpy(flight[chosenSeat - 1].firstName, "-----");
                    }
                }
                break;

            case 'f':
                return;
        }
    } while (scanf("%c", &choice) == 1);
}

// helper function that sorts the list of names in a flight alphabetically
void alphabetize(struct Seat sorted[], struct Seat flight[]) {
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