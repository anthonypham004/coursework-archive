#include <stdio.h>
#include <stdlib.h>
#include "bus.h"

/*
This file is the main interface for managing trips and bus details. 
This serves as the "admin" panel for the bus system.
*/
int main() {
    // Main selection loop
    int choice;

    while (1) {
        printf("\n=== Bus Trip Management Menu ===\n");
        printf("1. Create Trip\n");
        printf("2. View Trip\n");
        printf("3. Update Trip\n");
        printf("4. Delete Trip\n");
        printf("5. Reservation Management Menu\n");
        printf("0. Exit\n");

        // Get choice from user
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                create_trip();
                break;
            case 2:
                view_trip();
                break;
            case 3:
                update_trip();
                break;
            case 4:
                delete_trip();
                break;
            case 5:
                seat_menu();
                break;
            case 6: // Not mentioned in menu, but here for debug purposes
                print_db_keys_pretty();
                break;
            case 0:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}

void seat_menu() {
    // Main selection loop
    int choice;

    while (1) {
        printf("\n=== Reservation Management Menu ===\n");
        printf("1. Create Reservation\n");
        printf("2. View Reservation\n");
        printf("3. Update Reservation\n");
        printf("4. Delete Reservation\n");
        printf("0. Exit\n");

        // Get choice from user
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                create_reservation();
                break;
            case 2:
                view_reservation();
                break;
            case 3:
                update_reservation();
                break;
            case 4:
                delete_reservation();
                break;
            case 5:
                print_res_keys();
                break;
            case 0:
                return;
            default:
                printf("Invalid choice.\n");
        }
    }

    return;
}