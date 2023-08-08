import matplotlib.animation
import matplotlib.pyplot
import socket
import numpy
import math
import time
import sys
import os

os.system("clear")

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
sock.connect(("192.168.0.1", 2112))

# Variable set up
##########################################
averageCounter      = 1
dampingConstant     = 1
calibrationConstant = 1 # add later
fftSampleRate       = 0.1
fftBandPassLow      = 3000
fftBandPassHigh     = 3000
fftCutOffFreqHigh   = 267
pollWaitTime        = 0 # add later
derivativeTime      = 0.1
sharpeningConst     = 10
##########################################

# LiDAR commands initiation
#########################################################################################################
login   = "on page 12 on data sheet"
default = bytearray("<STX>sMN{SPC}mSCloadfacdef<ETX>", "ascii")
start   = bytes.fromhex("02 02 02 02 00 00 00 10 02 73 4D 4E 20 4C 4D 43 73 74 61 72 74 6D 65 61 73 03")
poll    = bytes.fromhex("02 02 02 02 00 00 00 0F 02 73 52 4E 20 4C 4D 44 73 63 61 6E 64 61 74 61 03")
stop    = bytes.fromhex("02 02 02 02 00 00 00 0F 02 73 4D 4E 20 4C 4D 43 73 74 6F 70 6D 65 61 73 03")
#########################################################################################################

# Set up data preccessing
##########################################################
def Com(value):

    return -(value & 0x80000000) | (value & 0x7fffffff)

def FilterLiDAR(prev, new):
    
    return dampingConstant * (new - prev) + prev

def LowPassFilter():
    print()

def FilterWithFFT(values):

    fft = numpy.fft.fft(values)

    #matplotlib.pyplot.plot(fft)
    #matplotlib.pyplot.show()

    for i in range(len(fft)):

        if (i <= fftBandPassHigh) and (i >= fftBandPassLow):

            fft[i] = fft[i]

        elif (i >= fftCutOffFreqHigh): 

            fft[i] = 0

    ifft = numpy.fft.fft(fft)

    return ifft

def CalculateDerivative(list):

    dir = []
    
    for i in range(len(list) - 1):

        dir.append((list[i + 1] - list[i])/derivativeTime)

    return dir
#########################################################

# Set up the plotter (for testing and debugging)
################################################
showPlots = False

def AnimatePlot(x, y, d):

    matplotlib.pyplot.subplot(2, 1, 1)
    matplotlib.pyplot.cla()
    matplotlib.pyplot.title("SBS Lidar 1")
    matplotlib.pyplot.xlabel("X (mm)")
    matplotlib.pyplot.ylabel("Y (mm)")
    matplotlib.pyplot.grid()
    matplotlib.pyplot.plot(x, y)
    matplotlib.pyplot.scatter(0, 0)

    matplotlib.pyplot.subplot(2, 1, 2)
    matplotlib.pyplot.grid()
    matplotlib.pyplot.title("SBS Lidar 1")
    matplotlib.pyplot.xlabel("Sample Number")
    matplotlib.pyplot.ylabel("Radius")
    matplotlib.pyplot.plot(range(len(d)), d)
    matplotlib.pyplot.show()
################################################

# start the liDAR and it's motor
sock.send(start)
ack = sock.recv(100).decode("UTF-8").split(" ")

oldDistances = []

# initiate some values
# ask LiDAR for measurments
sock.send(poll)

# receive the measured data
frame = sock.recv(2000).decode("UTF-8").split(" ")

for cAngle in range(26, len(frame)-8):

    oldDistances.append(int(frame[cAngle], 16)/1000)

# main loop
while (1):

    angles          = []
    avgValues       = []
    finalDistances  = []

    for x in range(averageCounter):

        # ask LiDAR for measurments
        sock.send(poll)

        # receive the measured data
        frame = sock.recv(2000).decode("UTF-8").split(" ")

        print(frame)
        input()
        os.system("clear")

        # extract the data
        angle       = Com(int(frame[23], 16))/10000
        interval    = int(frame[24], 16)/10000

        print(int(frame[23], 16))
        input()
        os.system("clear")

        print(angle)
        input()
        os.system("clear")

        print(interval)
        input()
        os.system("clear")

        xVals           = []
        yVals           = []
        newDistances    = [] 
        demoDistances   = []
        
        for cAngle in range(26, len(frame)-8):

            newDistance = int(frame[cAngle], 16)/1000
            oldDistance = oldDistances[cAngle - 26]
            demoDistances.append(newDistance)
            
            newDistances.append(FilterLiDAR(oldDistance, newDistance))
            angles.append(angle)
            angle += interval

        oldDistances = newDistances
        avgValues.append(newDistances)

        print(demoDistances)
        input()
        os.system("clear")

    # transpose avgValues
    avgValuesT = list(zip(*avgValues))

    for i in range(len(avgValuesT)):

        average = sum(avgValuesT[i])/len(avgValuesT[i])
        finalDistances.append(average)

    finalDistances = FilterWithFFT(finalDistances)

    div = CalculateDerivative(finalDistances)
    div2 = CalculateDerivative(div)

    #matplotlib.pyplot.plot(finalDistances)
    #matplotlib.pyplot.plot(div2)
    #matplotlib.pyplot.grid()
    #matplotlib.pyplot.show()

    #for i in range(len(div2)):

        #finalDistances[i] = finalDistances[i] - sharpeningConst*div2[i]

    #for i in range(len(finalDistances)):

        #xVals.append(math.cos(angles[i] * math.pi/180) * finalDistances[i])
        #yVals.append(math.sin(angles[i] * math.pi/180) * finalDistances[i])

    # plot the data
    #if showPlots == True:

        #AnimatePlot(xVals, yVals, finalDistances)

    # send the processed data to the PLC
        
    break

# stop the LiDAR and it's motor
sock.send(stop)