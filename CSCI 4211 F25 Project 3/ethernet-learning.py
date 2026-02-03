#!/usr/bin/env python

# Fall 2025 CSCI 4211: Introduction to Computer Networks

# This program serves as the SDN controller for the Ethernet-based
# self-learning switches. It was written in Python v3.

from pox.core import core
import pox.openflow.libopenflow_01 as of
import re

# Even a simple usage of the logger is much nicer than print statements!
log = core.getLogger()

flood_counter = 0
packets_received = 0

# TODO: Define your global data structures here.
mac_to_port = {} # Stores mappings of MAC addresses to port

def _handle_PacketIn(event):
  '''
  Handle an OFPacketIn message that a switch has sent to the controller 
  because the switch doesn't have a matching rule for the packet it received.
  '''
  global flood_counter
  global packets_received
  packets_received += 1
  log.info('Number of packets received so far: {}'.format(packets_received))
  
  # Extract the port the packet came in on for the switch that's contacting 
  # the controller.
  packet_input_port = event.port

  # Extract the number of ports attached to the sending switch except for 
  # the packet's input port. This variable should be used when updating your 
  # global flood counter.
  other_ports = len(event.connection.ports) - 2

  # Use POX to parse the packet.
  packet = event.parsed

  # Extract the packet's source and destination MAC addresses.
  src_mac = str(packet.src)
  dst_mac = str(packet.dst)

  # Packets with a destination MAC address that starts with "33:33:" are
  # invalid, so they're immediately dropped by the controller.
  pattern = r"33:33:"
  match = re.search(pattern, dst_mac) 
  if match:
    return

  # Extract the sending switch's ID.
  switch_ID = str(event.connection.dpid) + str(event.connection.ID)
  
  # This line of code prints information about packets that are sent to the 
  # controller.
  log.info('Packet has arrived: SRCMAC:{} DSTMAC:{} from switch:{} in-port:{}'.format(src_mac, dst_mac, switch_ID, packet_input_port))

  # TODO: Update the controller's global data structure that stores the 
  # information it learns about the network topology to include an entry for 
  # the packet's source host and the sending switch's port that can reach it, 
  # if such an entry does not already exist.

  # Update topology map by adding source MAC addresses not known in mac_to_port
  if switch_ID not in mac_to_port:
    mac_to_port[switch_ID] = {}
  if src_mac not in mac_to_port[switch_ID]:
    mac_to_port[switch_ID][src_mac] = packet_input_port


  # TODO: If the network topology already has an entry for the sending switch 
  # and the destination host, then install a new match-action rule or rules 
  # in the sending switch and have the original packet be forwarded to the 
  # correct output port. This is where you should use the code setting
  # message.match that was provided in Section 6.3 of the project 
  # instructions. 
  

  # NOTE: You will need to implement more code than the single line that is 
  # given to you.

  # Add output and input ports to mapping
  if dst_mac in mac_to_port[switch_ID]:
    output_port = mac_to_port[switch_ID][dst_mac]
    input_port = mac_to_port[switch_ID][src_mac]

    if output_port == packet_input_port:
      return

    # src to dst
    message1 = of.ofp_flow_mod()
    message1.match = of.ofp_match(dl_src=packet.src, dl_dst=packet.dst)
    message1.actions.append(of.ofp_action_output(port=output_port))
    event.connection.send(message1)

    # dst to src
    message2 = of.ofp_flow_mod()
    message2.match = of.ofp_match(dl_src=packet.dst, dl_dst=packet.src)
    message2.actions.append(of.ofp_action_output(port=input_port))
    event.connection.send(message2)

    # Forward the packet
    message_packet = of.ofp_packet_out()
    message_packet.data = event.ofp
    message_packet.actions.append(of.ofp_action_output(port=output_port))
    event.connection.send(message_packet)

  # From part 4 section 6.3
  # if dst_mac in mac_to_port[switch_ID]:
  #   output_port = mac_to_port[switch_ID][dst_mac]

  #   # Case where src and dest are on the same port. Drop.
  #   if output_port == packet_input_port:
  #     return
    
    
    # # Install rules for all matching fields of this packet
    # message = of.ofp_flow_mod()
    # message.match = of.ofp_match.from_packet(packet, event.port)
    # message.actions.append(of.ofp_action_output(port=output_port))
    # event.connection.send(message)

    # # Forward the packet
    # message_packet = of.ofp_packet_out()
    # message_packet.data = event.ofp
    # message_packet.actions.append(of.ofp_action_output(port=output_port))
    # event.connection.send(message_packet)
    # log.info('Installed flow rules and forwarded packet: SRCMAC:{} DSTMAC:{} from switch:{} in-port:{}'.format(src_mac, dst_mac, switch_ID, packet_input_port))



  # TODO: Otherwise, have the sending switch flood the original packet to 
  # every port except for the one the packet came in from originally. No rules
  # should be installed in the switch in this case. Also, don't forget to 
  # update your global counter for the number of flooded messages using the
  # provided other_ports variable.

  else:
    message = of.ofp_packet_out()
    message.data = event.ofp
    message.actions.append(of.ofp_action_output(port=of.OFPP_FLOOD))
    event.connection.send(message)
    flood_counter += other_ports
    log.info('Packet from SRCMAC:{} to DESTMAC:{} has been flooded. Total floods:{}'.format(src_mac, dst_mac, flood_counter))

def launch ():
  core.openflow.addListenerByName("PacketIn", _handle_PacketIn)
  log.info("Pair-Learning switch running.")