# INET3101-Module5-Assignment5
### PROBLEM
A server needs to be made that can take an input from a client and, if it's a text file name, return the contents of that file.

The server needs to be able to receive the client input and do one of the following:
- Send an error message to the client if:
    - the file exceeds the permittable size of 255 characters
    - the received input is not a text file
    - a text file with the given name does not exist
- Send the client the contents of the given text file, if it exists and within size

### SOLUTION
The sample C code to establish a client-server connection was modified to allow the client to send input to the server.
Once a connection is established between the server and client, the server will send a message to the client, prompting for a file name. The client's input is then received and handled by the server.

The server will send the client an error message if:
 - the server receives an input that does not end in a ".txt" extension
 - the requested file does not exist
 - the requested file is too large

Otherwise, if the received file request is valid, the server sends its contents to the client for printing. 
Once the client receives a message from the server, their sockets close, the server cleans up, and both programs end.

Additionally, if the server program terminates early with ctrl + C, the server will make sure to clean up and properly close connections before ending.

### PROS AND CONS
+ Back-and-forth communication between the server and client
+ Server retrieves file contents for a client's valid request
+ Error handling for different input cases
+ Graceful server program termination, ensuring files and connection are properly closed and memory is freed
- Not all errors stemming from connection-related system calls are handled
- Server can only handle one client at a time and shuts down once done with client

### Screen Recording Link
https://gofile.io/d/OPCGKf