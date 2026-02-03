#!/usr/bin/env python

# NOTE: Do NOT modify this file beyond commenting out line 65 and commenting 
# in line 68 in Section 6.4 of the project instructions.

# Fall 2025 CSCI 4211: Introduction to Computer Networks

# This program serves as a script that constructs Topology A.
# It was written in Python v3.

from mininet.cli import CLI
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.topo import Topo
from mininet.log import setLogLevel
from mininet.node import RemoteController

class AssignmentNetworks(Topo):
    def __init__(self, **opts):
        Topo.__init__(self, **opts)

	# Define all of the hosts.
        h1 = self.addHost('h1')
        h2 = self.addHost('h2')
        h3 = self.addHost('h3')
        h4 = self.addHost('h4')
        h5 = self.addHost('h5')
        h6 = self.addHost('h6')
        h7 = self.addHost('h7')
        h8 = self.addHost('h8')
        h9 = self.addHost('h9')

	# Define each switch.
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')
        s5 = self.addSwitch('s5')

	# Add links between each host and the switches.
        self.addLink(h1, s2)
        self.addLink(h2, s3)
        self.addLink(h3, s4)
        self.addLink(h4, s4)
        self.addLink(h5, s1)
        self.addLink(h6, s1)
        self.addLink(h7, s5)
        self.addLink(h8, s5)
        self.addLink(h9, s5)

	# Add links between the switches, with bandwidth (Mbps) and delay (ms)
        # constraints.
        self.addLink(s1, s2, bw = 30, delay = '10ms')
        self.addLink(s2, s3, bw = 20, delay = '40ms')
        self.addLink(s3, s4, bw = 40, delay = '30ms')
        self.addLink(s4, s5, bw = 10, delay = '20ms')
        

if (__name__ == '__main__'):
    setLogLevel( 'info' )
        
    # Create data network.
    topo = AssignmentNetworks()
    # TODO: For Section 6.4 only, comment OUT the following code line to test
    # your self-learning Ethernet switch.
#     net = Mininet(topo = topo, link = TCLink, autoSetMacs = True, autoStaticArp = True)
    # TODO: For Section 6.4 only, comment IN the following code line to test 
    # your self-learning Ethernet switch.
    net = Mininet(controller = RemoteController, topo = topo, link = TCLink, autoSetMacs = True, autoStaticArp = True)

    # Run the network.
    net.start()
    CLI( net )
    net.stop()