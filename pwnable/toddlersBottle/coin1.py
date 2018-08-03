import socket
import re

hostname = 'localhost'
port = 9007

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((hostname,port))

#handle introduction
s = sock.recv(4096)
print(s)

#find 100 fakes
for i in range(0,100):
    #now get num coins and num guesses
    s = sock.recv(1024)
    m = re.match(r"N=(\d+) C=(\d+)",s)
    numCoins = int(m.group(1))
    numGuesses = int(m.group(2))

    start = 0
    end = numCoins
    mid = end/2

    #binary search for correct coin
    for x in range(0, numGuesses):
        firstHalf = [ str(x) for x in range(start, mid) ]
        firstHalfString = ' '.join(firstHalf)
        sock.sendall(firstHalfString + '\x0a')
    
        #receive and check weight 
        allGold = len(firstHalf) * 10
        weight = int(sock.recv(1024))

        if weight == allGold:
            start = mid
            #end stays the same
        elif weight == allGold - 1:
            end = mid
            #start stays the same
        else:
            print("Wuh-oh, weight probs")

        mid = (start + end)/2

        #get final answer ready
        if weight > 10:
            pass
        elif weight == 10:    
            answer = str(mid)
        elif weight == 9:
            answer = firstHalfString
        else:
            print("Wahhh, final answer/weight weirdness")
    
    #send final answer and get response
    sock.sendall(answer + '\x0a')
    s = sock.recv(1024)
    print s

#done
flag = sock.recv(4096)
print flag
sock.close()
