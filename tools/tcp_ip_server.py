import socket

def InitiateServer():
    HOST = "fe80::9347::240b:b0e7:bacf"
    PORT = 3001
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((HOST, PORT))
    sock.listen()
    conn, addr = sock.accept()
    return conn

# INITIATE SERVER
server = InitiateServer()


'''place below code somwhere else'''
while True:
    data = server.recv(10000)
    if not data:
        break    
    print(data)
    server.sendall(b"response from frontend server")


    
