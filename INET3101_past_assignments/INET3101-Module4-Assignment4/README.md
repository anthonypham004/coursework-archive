# INET3101-Module4-Assignment4
### PROBLEM
The flight reservation system from Module 3 did not save changes to flights one the program is shut down. A data saving system needed to be implemented so that changes to a flight can be saved to a file to be accessed later when the program is restarted.

### SOLUTION
A file I/O system is implemented on top of the reservation system so that data can be saved and loaded when the program is restarted.

The program looks for a binary file 'flight_details.bin' upon startup.
- If such file exists within the same directory as the program, the program reads the binary data from the file and dumps it into the flight arrays in the program.
- If no file exists, the program creates one and loads the file with the default seat values.

The program then allows the user to choose to save their changes before quitting.

### PROS AND CONS
+ Program now allows the user to save their changes onto a binary file
+ Data from a previous session will now be loaded onto the program
+ Made small changes and implemented best practices to the pre-existing code
    + For example, opting to use enums for use in switch cases for better clarity
- Lack of user control over saving and loading
    - User only has the option to save when quitting
    - Program automatically loads data from file without asking user

Screen recording link: https://gofile.io/d/nEDkLb