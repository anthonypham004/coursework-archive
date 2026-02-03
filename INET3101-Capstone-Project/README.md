# INET3101-Capstone-Project
## PROBLEM
Needed to make a bus reservation system. This system should be able to perform CRUD operations on ADT's. In this case, CRUD operations are available for bus trips and reservations.

This system can create and modify bus trips, including their individual seats. When a seat within a trip is reserved, the seat and its corresponding trip will be updated accordingly, showing its availability as well as holding information about the reservation for that seat. Information about a trip or reservation can be displayed, and a seat map will be shown when viewing details about a trip, showing which seats are already reserved.

Additionally, information about each trip and reservation can be requested through the client-server model, and reservations can be made for a seat as well.

## DESIGN AND ARCHITECTURE DETAILS
This system is primarily composed of three ADT's: Trip, Seat, and Reservation.

A Trip's metadata is composed of it's origin, destination, a three-digit bus number, an array of Seat structs arranged in rows and columns, and its corresponding trip ID, which is used to identify the trip in the GDBM database. Trips can be created, deleted, viewed, and updated through functions accessed by the bus_manager.c file.

Each Seat has a seat ID composed of its row number and column letter, its reservation status, and the details of the reservation that's claimed it.

A Reservation is composed of the trip ID it belongs to, the reserved seat's row and column, the first and last name of the person reserving the seat, and its reservation ID, which can be used to identify the reservation in the database. Reservations can also be created, deleted, viewed, and updated through bus_manager.c, and created via socket connection.

## PROS AND CONS
+ Full CRUD implementation of the Trip and Reservation ADTs
+ Socket-based lookup and reservation system
+ Persistent and detailed ADT data stored via GDBM databases
- Edge cases not fully covered. For example, invalid inputs that may be too long or of the wrong type.
- Initially, a few more features were planned, such as a waitlist/queueing system. This was left out.
- Some functions weren't fully implented with data fetching and displaying
- Some redundant code that could have been made as helper functions

## COMPILNG AND USAGE
Prerequisites:
Make sure the GDBM library is installed on your machine. Additionally, the Makefile may need to be modified to correctly link the GDBM library depending on your machine's architecture (the Makefile is originally set up for ARM).

This project consists of three programs: bus_manager, bus_client, and bus_server
- bus_manager is the main interface for running CRUD operations on Trips and Reservations. This is meant to serve as the "admin" controls for the bus system.
- bus_client and bus_server are the client-server programs that allow fetching Trip and Reservation data and creating Reservations over a socket connection. bus_server must be up and running in order for bus_client to be usable.

Makefile:
- Run 'make' to compile the program and 'make clean' to remove the compiled files
- Run 'make run_manager' to compile and run the bus_manager.c program
- Run 'make run_client' to compile and run the bus_client.c program
- Run 'make run_server' to compile and run the bus_server.c program