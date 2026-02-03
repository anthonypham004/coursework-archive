# INET3101-Module3-Assignment3
PROBLEM:
A seating reservation program needed to be made that can track the outbound and inbound flights of a plane that holds 24 seats.
The program needs to be able to do these for each flight:
1. Show number of empty seats
2. Show list of empty seaats
3. Show alphabetical list of seats
4. Assign a custoemr to a seat assignment
5. Delete a seat assignment
6. Return to Main menu

SOLUTION:
The solution.c program holds two arrays of the Seat struct— one for the outbound flight and one for the inbound flight. The Seat struct holds:
- int id, a numerical id for each seat (displayed as 1-24, internally stored as 0-23)
- _Bool taken, a Boolean value that describes whether or not a customer is assigned to that seat (true if assigned, false otherwise)
- char lastName, a string that holds a customer's last name. Stores up to 29 characters.
- char firstName, a string that holds a customer's first name. Stores up to 29 characters.

The main function handles user selection between modifying the inbound or outbound flights and exiting the program.

Once a flight selection is made, the user is then given the options listed earlier. Options 4 and 5 allow the user to abort their entry after details have been entered. The flight_manager function handles all of these options.

PROS AND CONS:
+ Stores and allows manipulation between two separate flights
+ Each seat can be assigned a customer
+ Seat assignments can be deleted
+ Seat assignments can be listed in alphabetical order
+ Simple navigation through menus
- Some exception handling not implimented (such as names over 29 characters)
- As a result of the previous con, names can only go up to 29 characters
- Only two flights of 24 seats each can be manipulated. Changes to seat numbers or adding additional flights must be hardcoded.
