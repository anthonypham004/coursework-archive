# Fall 2025 CSCI 4211: Introduction to Computer Networks

# This program serves as the server of the trivia game application.
# Written in Python v3.

import sys, os
from socket import *
import threading
import time
import json
import random
import signal

# The server's information when the clients and server run on the same
# machine.
LOCAL_HOST = "127.0.0.1"
LOCAL_PORT = 5001

# The server's information when the server runs in the cloud and the clients
# are remote.
REMOTE_HOST = "0.0.0.0"
REMOTE_PORT = 5001

class Server:
    '''
    Initializes any class variables and data structures and reads necessary
    files.

    Parameters
    ----------
    - server_host : str
        - The IPv4 address or hostname of the server (e.g., "100.50.200.5" or 
          "localhost"). Its value is determined by which command line option
          was used to run the server (i.e., 1 or 2).
    - server_port : int
        - The TCP port number the server will run on (e.g., 5001). Its value 
          is determined by which command line option was used to run the 
          server (i.e., 1 or 2).
    '''
    def __init__(self, server_host, server_port):
        # Establish the server's listening socket
        self.server_host = server_host
        self.server_port = server_port
        self.server_socket = socket(AF_INET, SOCK_STREAM)
        self.server_socket.bind((self.server_host, self.server_port))
        
        # Thread management setup, mostly for signal_handler to access and close sockets
        self.client_sockets = set()
        self.lock = threading.Lock()
        signal.signal(signal.SIGINT, self.signal_handler)

        # Load contents of the trivia game file into the Server
        try:
            with open('trivia_questions.json', 'r') as file:
                self.questions = json.load(file)
        except FileNotFoundError:
                print("[SERVER]: Error: server was not able to find JSON file.") # DEBUG
                sys.exit(1)
        return

    '''
    Configures the server socket and waits to receive a new client connection.
    Once a client connection is accepted, the server handles the client and 
    facilitates a trivia game with it.
    '''
    def run(self):
        self.server_socket.listen() # Set socket to listen for connection
        print("[SERVER]: Server is up and listening for connections...")
        while True:
            try:
                # Listening socket accepts incoming connection
                # Each connected client will have its own independent thread of the trivia_game function running
                conn, addr = self.server_socket.accept() 
                print(f"\n[SERVER]: Connected by {addr}") # DEBUG
                t = threading.Thread(target=self.trivia_game, args=(conn, addr))
                t.start()
            except OSError:
                print("[SERVER]: Client socket has already closed.") # DEBUG
                pass
        return

    '''
    The server facilitates the trivia game with the client based on the
    general interaction described in Section 2 in the instructions.

    Parameters
    ----------
    - connection_socket : socket
        - The open socket connected to the client that wants to play the
          trivia game.
    - client_address : (), tuple
        - Identifying information about the client connection (i.e., the 
          host and port number). There's no requirement for this parameter to
          be used in this function. However, it's useful when printing 
          debugging information to the server's terminal to differentiate
          between multiple clients.
    '''
    def trivia_game(self, connection_socket, client_address): 
        # Register connection_socket to client_socket set
        with self.lock:
            self.client_sockets.add(connection_socket)
        # Sets a timeout for the client socket. 
        # If no response is heard for the specified amount of secs, a timeout exception is raised.
        connection_socket.settimeout(60)
        try:
            # Main game loop
            while (1):
                # Wait for the client to start a new game
                print(f"[SERVER]: Waiting for client {client_address} to start a new game...") # DEBUG
                startGame = self.safe_recv(connection_socket)
                if startGame == "end":
                    print(f"[SERVER]: Client {client_address} has ended the game.\n") # DEBUG
                    break
                print(f"[SERVER]: Client {client_address} has started a new game!") # DEBUG
                
                # Randomize the sequence of questions to be asked
                questionSequence = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]
                random.shuffle(questionSequence)

                # Send 5 questions to the client
                points = 0
                for qNum, question in enumerate(questionSequence[0:5]):
                    # Format the question to send
                    currQuestion = self.questions[question]
                    questionMsg = f"Question {qNum + 1}: {currQuestion[0]}\n"
                    for aNum, choice in enumerate(currQuestion[1]):
                        questionMsg += f"{aNum + 1}) {choice}\n"

                    # Send the question
                    self.safe_send(connection_socket, questionMsg)
                    print(f"[SERVER]: Sent question {qNum + 1} to {client_address}. The correct answer is {currQuestion[2]}.") # DEBUG

                    # Send the answer range for client to validate answer input
                    answerRange = str(len(currQuestion[1]))
                    self.safe_send(connection_socket, answerRange)
                    #print(answerRange)

                    # Wait for client to answer
                    clientMsg = self.safe_recv(connection_socket)
                    print(f"[FROM {client_address}]: {clientMsg}") # DEBUG

                    # Send feedback to the client
                    if clientMsg == currQuestion[2]:
                        feedbackMsg = "That is correct! You scored 1 point.\n"
                        self.safe_send(connection_socket, feedbackMsg)
                        points += 1

                        print(f"[TO {client_address}]: {feedbackMsg}") # DEBUG
                    else:
                        correctAnswerString = currQuestion[1][int(currQuestion[2]) - 1]
                        feedbackMsg = f"Not quite. The correct answer was {currQuestion[2]}) {correctAnswerString}.\n"
                        self.safe_send(connection_socket, feedbackMsg)

                        print(f"[TO {client_address}]: {feedbackMsg}") # DEBUG
                
                # Format the results message
                resultsMsg = f"Total score: {points} points."
                if points == 1:
                    resultsMsg = resultsMsg.replace("points", "point")
                if points == 5:
                    resultsMsg += " That's a perfect score!"
                if points == 0:
                    resultsMsg += " Better luck next time!"
                self.safe_send(connection_socket, resultsMsg)
        except Exception as e:
            print(f"[SERVER]: Error with {client_address} -> {e}") # DEBUG
            pass
        finally:
            with self.lock:
                self.client_sockets.discard(connection_socket) # Remove this socket from the set
        connection_socket.close()
        print(f"[SERVER]: Connection with {client_address} has closed.") # DEBUG
        return

    '''
    This function first sends the length of the message in bytes to the client, then the message itself.
    This is to prevent the client from receiving multiple server messages in one recv() call.
    '''
    def safe_send(self, connection_socket, message):
        message = message.encode()
        connection_socket.sendall(len(message).to_bytes(4, 'big'))
        connection_socket.sendall(message)

    '''
    This function does the receiving of the length header and message payload.
    This is to prevent the server from receiving multiple client messages in one recv() call.
    Not as much of a concern on the server side, but it's good to have the symmetry to be safe.
    '''
    def safe_recv(self, connection_socket):
        msgLength = int.from_bytes(connection_socket.recv(4), 'big')
        message = connection_socket.recv(msgLength).decode()
        return message
    
    '''
    The server's signal handler. If Ctrl+C is pressed while the server is
    running, then the server will shutdown gracefully. This will allow you to
    immediately restart the server using the same port and a provide an easier
    testing process.

    NOTE: Do not modify this function beyond the listed TODO statement below.
    '''
    def signal_handler(self, sig, frame):
        print('\nReceived signal: ', sig)
        print('Performing cleanup...')
        # TODO: Add your cleanup code here (e.g., closing files, releasing 
        # resources, etc.).

        # Close the listening socket
        if self.server_socket:
            self.server_socket.close()

        # Close client sockets in set
        with self.lock:
            for sock in list(self.client_sockets):
                try:
                    sock.shutdown(socket.SHUT_RDWR)
                except:
                    pass
                sock.close()
            self.client_sockets.clear()
        
        print('Exiting gracefully.')
        sys.exit(0)

'''
This is the main() function that first executes when server.py runs. It
initializes the server instance and then runs it. 

NOTE: Do not modify this function.
'''
if (__name__ == '__main__'):
    # Check if an argument was provided on the command line. If not, then
    # print a usage message and exit the program.
    if (len(sys.argv) != 2):
        print("\nusage: python3 server.py [1 | 2]\n")
        sys.exit(1)

    # Check the value of the provided command line argument and initialize the
    # server instance accordingly. If the value is invalid, then print a usage 
    # message and exit the program.
    option = sys.argv[1]
    if (option == "1"):
        server = Server(LOCAL_HOST, LOCAL_PORT)
    elif (option == "2"):
        server = Server(REMOTE_HOST, REMOTE_PORT)
    else:
        print("\nusage: python3 server.py [1 | 2]\n")
        sys.exit(1)
    
    # Configure the server's signal handler to handle when Ctrl+C is pressed.
    signal.signal(signal.SIGINT, server.signal_handler)
    server.run()