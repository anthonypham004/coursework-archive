# Fall 2025 CSCI 4211: Introduction to Computer Networks

# This program serves as the sender in a Go-Back-N data transfer. It was 
# written in Python v3.

from simulator import sim
from simulator import to_layer_three
from event_list import evl
from packet import *
from circular_buffer import circular_buffer

class S_sender:
    ''' Represents the sender in the Go-Back-N protocol. '''
    def __init__(self):
        ''' Initializes the relevant class variables for the sender. '''
        
        # NOTE: Do NOT change the naming of any of the provided class 
        # variables.

        # Statistics Counters
        #---------------------------------------------------------------------
        # Number of Application Layer (layer 5) messages the sender receives.
        self.new_messages = 0
        # A layer 5 message is dropped when it's received while the sender's 
        # buffer is full.
        self.dropped_messages = 0
        # Number of new data packets sent by the sender.
        self.new_data = 0
        # A data packet is lost when it never arrives at the receiver.
        self.lost_data = 0
        # Data packets are retransmitted when a timer expires.
        self.retransmitted_data = 0
        # A received ACK is uncorrupted (verified using the checksum) and 
        # in-order.
        self.correct_ACKs = 0
        # A received ACK is corrupted (verified using the checksum).
        self.corrupted_ACKs = 0
        # A received ACK is out-of-window (i.e., has an acknowledgment number
        # not currently in the sender's window).
        self.out_of_window_ACKs = 0

        # Useful Variables
        #---------------------------------------------------------------------
        # The size of the sliding window and circular buffer.
        self.N = 7
        # The circular buffer that will store any outstanding and 
        # unacknowledged packets.
        self.c_b = circular_buffer(self.N)
        # This should be used as the second argument to evl.start_timer().
        # It represents the length of the timers.
        self.estimated_rtt = 30
        # This should be used as the first argument to to_layer_three() and 
        # evl.start_timer(). It indicates that the sender called the function.
        self.entity = 'S'
        # TODO: Initialize any other useful class variables that you think
        # are necessary.

        self.S = self.N + 1

        # Tracking variables
        self.base = 0 # The oldest unACK'd seq num [*1* 2 3 4 5 6 7]
        self.next_seqnum = 0 # The next seq num to be added to window [1 2 3 4 5 6 7] *8*
        self.timer_running = False
        self.last_received = -1 # Last received correct ACK
        
        # Debug variables
        self.total_sent = 0

        return

    def S_output(self, message):
        '''
        The sender received a message from layer 5, so it should try to create
        a data packet containing the message and send it to the Network Layer
        (layer 3). 
        
        Parameters
        ----------
        - message : msg
            - The message the sender received from layer 5.
        '''
        
        self.new_messages += 1

        # Add packet to window (c_b) if there's room
        if not self.c_b.is_full():
            self.new_data += 1
            #self.total_sent += 1 # DEBUG
            pkt = packet(seqnum=self.next_seqnum, payload=message)

            # Add packet to the circular buffer and transport
            self.c_b.push(pkt)
            to_layer_three(self.entity, pkt)
            #print(f"(S) Packet {pkt.seqnum} sent") # DEBUG
            
            # If packet is first in window, start the timer for timeout
            #print(f"(S) base:{self.base}, seqnum:{pkt.seqnum}. timer_running:{self.timer_running}") # DEBUG
            if self.base == self.next_seqnum and not self.timer_running:
                #print(f"(S) Started timer for packet {pkt.seqnum}") # DEBUG
                evl.start_timer(self.entity, self.estimated_rtt)
                self.timer_running = True

            self.next_seqnum = (self.next_seqnum + 1) % self.S
        
        else:
            # Drop message if window is already full
            self.dropped_messages += 1

        return

    def S_input(self, received_packet):
        '''
        The sender received an ACK packet from layer 3. It should verify the
        received ACK and behave accordingly. 
        
        Parameters
        ----------
        - received_packet : packet
            - The received ACK packet that was sent by the receiver.
        '''

        # Check to see if the ACK packet got corrupted
        checksum = received_packet.checksum
        if received_packet.get_checksum() != checksum:
            self.corrupted_ACKs += 1
            return
        
        acknum = received_packet.acknum

        # Check to see if received ACK is the base seq's
        if acknum == self.base:
            self.correct_ACKs += 1
            self.last_received = acknum
            self.c_b.pop()
            #print(f"(S) Successfully received ACK {acknum}") # DEBUG
            
            evl.stop_timer()
            self.timer_running = False
            #print(f"(S) Stopped timer for packet {acknum}") # DEBUG

            self.base = (self.base + 1) % self.S
            #print(f"(S) New base is now {self.base}")
            return

        # Case for a cumulative ACK if ACK is not the base's
        window = []
        for pkt in self.c_b.read_all():
            window.append(pkt.seqnum)
        
        if acknum not in window:
            # If ACK does not match packet in window, drop it
            self.out_of_window_ACKs += 1
            return
        else:
            #print(f"(S) Successfully received cumulative ACK {acknum}.") # DEBUG
                
            # Clear out all cumulatively ACK'd packets in buffer window from base to acknum
            while self.base != (acknum + 1) % self.S:
                self.c_b.pop()
                self.base = (self.base + 1) % self.S # Slide base of window over

            self.last_received = acknum

            # Stop timer if all packets in window were ACK'd
            if self.base == self.next_seqnum:
                evl.stop_timer()
                #print("(S) Timer stopped for all packets in window") # DEBUG
                self.timer_running = False
            else:
                # Else restart the timer
                evl.start_timer(self.entity, self.estimated_rtt)
                self.timer_running = True

        return

    def S_handle_timer(self):
        ''' Handles the expiration of the sender's timer. If this function
            is triggered, then it means that an ACK for any of the most 
            recently sent packets wasn't received by the sender in time, so 
            all currently outstanding and unacknowledged packets need to be 
            retransmitted. '''
        
        #print(f"(S) Timeout occured for packet {self.last_received + 1}") # DEBUG

        # Retransmit all unACK'd packets in window
        pkts = self.c_b.read_all()

        for pkt in pkts:
            #print(f"(S) Outstanding packet {pkt.seqnum} has been retransmitted") # DEBUG
            to_layer_three(self.entity, pkt)
            self.retransmitted_data += 1

        # Restart timer if window's not empty
        if len(pkts) > 0:
            evl.start_timer(self.entity, self.estimated_rtt)
            self.timer_running = True
            #print(f"(S) Restarted timer for packet {pkts[0].seqnum}") # DEBUG
        
        return
    
    def S_lost_packet(self, lost_packet):
        '''
        This function is called by the simulator when a packet is lost. In
        addition to updating the lost data packet counter, it will be useful
        for debugging when you run the code with a non-zero loss probability. 
        
        Parameters
        ----------
        - lost_packet : packet
            - A data packet that was lost in transit to the receiver.
        '''        
        self.lost_data += 1
        #print(f"(S) Packet {lost_packet.seqnum} has been lost") # DEBUG
        
        return

    def S_sim_end(self):
        ''' Called at the end of the simulation. Prints out all of the 
            statistics tracked by the sender. '''

        print("============SENDER STATISTICS===========")
        print("  Received Messages")
        print("    New                  -> ", self.new_messages)
        print("    Dropped              -> ", self.dropped_messages)
        print("  Sent Data")
        print("    New                  -> ", self.new_data)
        print("    Lost                 -> ", self.lost_data)
        print("    Retransmitted        -> ", self.retransmitted_data)
        print("  Received ACKs")
        print("    Correct              -> ", self.correct_ACKs)
        print("    Corrupted            -> ", self.corrupted_ACKs)
        print("    Out of Window        -> ", self.out_of_window_ACKs)
        print("")

a = S_sender()