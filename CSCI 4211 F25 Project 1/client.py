# Fall 2025 CSCI 4211: Introduction to Computer Networks

# This program serves as the client of the trivia game application.
# Written in Python v3.

import sys
from socket import *
import json

# The server's information when the clients and server run on the same
# machine.
LOCAL_HOST = "127.0.0.1"
LOCAL_PORT = 5001

# The server's information when the server runs in the cloud and the clients
# are remote.
REMOTE_HOST = "18.191.255.56"
REMOTE_PORT = 5001

class Client:
    '''
    Initializes any class variables and data structures.

    Parameters
    ----------
    - server_host : str
        - The IPv4 address or hostname of the server (e.g., "100.50.200.5" or 
          "localhost"). Its value is determined by which command line option
          was used to run the client (i.e., 1 or 2).
    - server_port : int
        - The TCP port number the server will run on (e.g., 5001). Its value 
          is determined by which command line option was used to run the 
          client (i.e., 1 or 2).
    '''
    def __init__(self, server_host, server_port):
            self.server_host = server_host
            self.server_port = server_port
            self.client_socket = socket(AF_INET, SOCK_STREAM) # client's socket
            self.client_socket.connect((server_host, server_port))
        

    '''
    Connects to the server and facilitates the trivia game with the server
    based on the general interaction described in Section 2 in the 
    instructions.
    '''
    def run(self):
        print("=====================================================================")
        print("                    University of Minnesota Trivia                   ")
        print("=====================================================================\n")
            
        print("\n------------------------- GAME INSTRUCTIONS -------------------------")
        print("  • Five random trivia questions about the University of Minnesota")
        print("    will be proposed to you along with possible answers.\n")
        
        print("  • Select your answer by entering the associated number (e.g., '1').\n")
        
        print("  • After all five questions have been answered, you will receive")
        print("    your total score.")

        while(1):
        
            print("\nWould you like to start a new game? [y/n]: ")

            # Get the user's response.
            while (1):
                print("Input: ", end = "")
                st = input()
                if ((st != "y") and (st != "Y") and (st != "n") and (st != "N")):
                    continue
                else:
                    break
            
            # If the input is "n" or "N", then quit the program.
            if ((st == "n") or (st == "N")):
                print("\nClient is exiting...")
                self.safe_send("end")
                self.client_socket.close()
                sys.exit(0)
            
            print("\nStarting a new game. Good luck!")
            self.safe_send("start")

            # Game loop
            for question in range(5):
                # Receive question from server
                print("---------------------------------------------------------------------\n")
                questionMsg = self.safe_recv()
                print(questionMsg)

                # Receive number of possible answers
                answerRange = int(self.safe_recv())
                #print(answerRange)

                # Validate and send answer to server
                while (True):
                    answerMsg = input("Answer: ")
                    try:
                        if int(answerMsg) >= 1 and int(answerMsg) <= answerRange:
                            break
                        else:
                            print("Invalid answer.")
                    except ValueError:
                        print("Invalid answer.")
                        pass
                        
                self.safe_send(answerMsg)


                # Receive feedback from server
                feedbackMsg = self.safe_recv()
                print(feedbackMsg)



            print("\n----------------------------- GAME OVER -----------------------------")
            
            # Receive the trivia game results
            resultsMsg = self.safe_recv()
            print(resultsMsg)

            print("\nThanks for playing!")

    """
    Helper function for safe_recv() that ensures all bytes of a message are received from the stream.
    """
    def recv_all(self, n):
        data = b''
        # Will continue to retrieve data from the stream until the data reaches the specified amount of bytes
        while len(data) < n:
            chunk = self.client_socket.recv(n - len(data))
            if not chunk:
                # Will likely happen when server closed the connection
                return None
            data += chunk
        return data
    
    '''
    This function does the receiving of the length header and message payload.
    This is to prevent the client from receiving multiple server messages in one recv() call.
    Also, if the connection were to be closed while the client is receiving a message, 
    this notifies the client of the closed connection and shuts down the program.
    '''
    def safe_recv(self):
        try:
            # Receive what should be the incoming message length
            msgLen = self.recv_all(4)
            if msgLen == None:
                # If something happens to the connection during receiving, raise error
                raise ConnectionResetError
            msgLen = int.from_bytes(msgLen, 'big')

            # Receive the incoming message of msgLen length
            message = self.recv_all(msgLen)
            if message == None:
                raise ConnectionResetError
            message = message.decode()
            return message
        
        except (BrokenPipeError, ConnectionResetError, TypeError):
            print("Connection shut down by server.")
            self.client_socket.close()
            sys.exit(0)
            
    '''
    This function first sends the length of the message in bytes to the server, then the message itself.
    This is to prevent the server from receiving multiple client messages in one recv() call.
    Not as much of a concern on the client side, but it's good to have the symmetry to be safe.
    '''
    def safe_send(self, message):
        try:
            message = message.encode()
            self.client_socket.sendall(len(message).to_bytes(4, 'big'))
            self.client_socket.sendall(message)
        except (BrokenPipeError, ConnectionResetError):
            print("Connection shut down by server.")
            self.client_socket.close()
            sys.exit(0)
            

'''
This is the main() function that first executes when client.py runs. It
initializes the client instance and then runs it. 

NOTE: Do not modify this function.
'''
if (__name__ == '__main__'):
    # Check if an argument was provided on the command line. If not, then
    # print a usage message and exit the program.
    if (len(sys.argv) != 2):
        print("\nusage: python3 client.py [1 | 2]\n")
        sys.exit(1)

    # Check the value of the provided command line argument and initialize the
    # server instance accordingly. If the value is invalid, then print a usage 
    # message and exit the program.
    option = sys.argv[1]
    if (option == "1"):
        client = Client(LOCAL_HOST, LOCAL_PORT)
    elif (option == "2"):
        client = Client(REMOTE_HOST, REMOTE_PORT)
    else:
        print("\nusage: python3 client.py [1 | 2]\n")
        sys.exit(1)
    
    client.run()