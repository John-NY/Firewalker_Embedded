# https://wiki.python.org/moin/UdpCommunication
import socket
import struct

UDP_IP = "localhost"
UDP_PORT = 45454

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # Allow other processes to
                                                         #    bind to port
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print "received message:", data

