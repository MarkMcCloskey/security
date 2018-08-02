import socket
import re

hostname = 'pwnable.kr'
port = 9007

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((hostname,port))

#handle introduction
s = sock.recv(4096)
print(s)

#now get num coins and num guesses
s = sock.recv(4096)
print s
m = re.match(r"N=(\d+) C=(\d+)",s)
numCoins = int(m.group(1))
numGuesses = int(m.group(2))

start = 0
end = numCoins - 1
mid = end/2

for x in range(0, numGuesses - 1):
    guess = 
sock.close()
