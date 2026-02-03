# Fall 2025 CSCI 4211: Introduction to Computer Networks

# This program serves as the receiver in a Go-Back-N data transfer. It was 
# written in Python v3.

from simulator import to_layer_five
from packet import send_ack

class R_receiver:
    ''' Represents the receiver in the Go-Back-N protocol. '''
    def __init__(self):
        ''' Initializes the relevant class variables for the receiver. '''
        
        # NOTE: Do NOT change the naming of any of the provided class 
        # variables.

        # Statistics Counters
        #---------------------------------------------------------------------
        # Number of new ACKs the receiver sends to the Network Layer (layer 3).
        self.new_ACKs = 0
         # An ACK packet is lost when it never arrives at the sender.
        self.lost_ACKs = 0
        # An ACK is retransmitted if the receiver receives a data packet that 
        # is corrupted or has the wrong sequence number (i.e., the number of
        # NACKs sent by the receiver).
        self.retransmitted_ACKs = 0
        # A received data packet is uncorrupted (verified using the checksum)
        # and in-order.
        self.correct_data = 0
        # A received data packet is corrupted (verified using the checksum).
        self.corrupted_data = 0
        # A received data packet is out-of-order (i.e., has the wrong sequence
        # number).
        self.out_of_order_data = 0

        # Useful Variables
        #---------------------------------------------------------------------
        # The size of the sliding window.
        self.N = 7
        # This should be used as the first argument to to_layer_five() and 
        # send_ack(). It indicates that the receiver called the function.
        self.entity = 'R'
        # TODO: Initialize any other useful class variables that you think
        # are necessary.

        self.S = self.N + 1

        # Tracking variables
        self.expected_seqnum = 0 # The expected in-order sequence number receiver should receive
        self.last_correct_ACK = (self.expected_seqnum - 1 + self.S) % self.S # ACK number of the last correct packet

        return

    def R_input(self, received_packet):
        ''' 
        The receiver received a data packet from layer 3. It should verify the
        received data packet and behave accordingly. 
        
        Parameters
        ----------
        - received_packet : packet
            - The received data packet that was sent by the sender.
        '''

        seqnum = received_packet.seqnum
        checksum = received_packet.checksum

        #print(f"(R) seqnum: {seqnum}, expected seqnum: {self.expected_seqnum}") # DEBUG
        if received_packet.get_checksum() != checksum:
            # Corrupted if checksum does not match. Send last sent ACK.
            self.corrupted_data += 1
            self.retransmitted_ACKs += 1
            #print(f"(R) Packet {seqnum} has been corrupted") # DEBUG

            send_ack(self.entity, self.last_correct_ACK)
            
        elif seqnum != self.expected_seqnum:
            # If packet is out of order, send last sent ACK
            self.out_of_order_data += 1
            self.retransmitted_ACKs + 1
            #print(f"(R) Packet {seqnum} was received out of order. Expected packet {self.expected_seqnum}") # DEBUG

            send_ack(self.entity, self.last_correct_ACK)
            
        else:
            # Send correct ACK, update tracking variables
            self.correct_data += 1
            self.new_ACKs += 1
            send_ack(self.entity, seqnum)

            #print(f"(R) Successfully received packet {seqnum}. ACK {seqnum} has been sent.") # DEBUG
            self.expected_seqnum = (self.expected_seqnum + 1) % self.S
            self.last_correct_ACK = seqnum

            to_layer_five(self.entity, received_packet.payload.data)

        return
    
    def R_lost_packet(self, lost_packet):
        '''
        This function is called by the simulator when a packet is lost. In
        addition to updating the lost ACK packet counter, it will be useful
        for debugging when you run the code with a non-zero loss probability. 
        
        Parameters
        ----------
        - lost_packet : packet
            - An ACK packet that was lost in transit to the Sender.
        '''
        self.lost_ACKs += 1
        #print(f"(R) ACK {lost_packet.seqnum} has been lost") # DEBUG


        return

    def R_sim_end(self):
        ''' Called at the end of the simulation. Prints out all of the 
            statistics tracked by the receiver. '''

        print("===========RECEIVER STATISTICS==========")
        print("  Sent ACKs")
        print("    New                  -> ", self.new_ACKs)
        print("    Lost                 -> ", self.lost_ACKs)
        print("    Retransmitted        -> ", self.retransmitted_ACKs)
        print("  Received Data")
        print("    Correct              -> ", self.correct_data)
        print("    Corrupted            -> ", self.corrupted_data)
        print("    Out of Order         -> ", self.out_of_order_data)
        print("")

b = R_receiver()