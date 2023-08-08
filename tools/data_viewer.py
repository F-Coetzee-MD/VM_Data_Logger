import socket
import numpy as np
import matplotlib.pyplot as plt

def ReadIncomingData(raw_data):
    data = raw_data.decode('ascii')
    type = data.split("(")[0]
    data_str = data.split("(")[1].split(")")[0].split(",")
    data_num = [float(i) for i in data_str]

    if type == "lidar_scan":
        plt.title("LiDAR Scan")
        plt.plot(list(range(-45, len(data_num)-45)), data_num)
        #plt.show()

    if type == "fft_data":
        plt.title("Fourier Transform")
        plt.plot(list(range(0, len(data_num))), data_num)
        #plt.show()

    if type == "mag_data":
        plt.title("Magnitude Response")
        plt.plot(list(range(0, len(data_num))), data_num)
        #plt.show()

    if type == "pha_data":
        plt.title("Phase Response")
        plt.plot(list(range(0, len(data_num))), data_num)
        #plt.show()

def InitiateServer():
    HOST = "127.0.0.1"
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
    ReadIncomingData(data)
    server.sendall(b"response from frontend server")