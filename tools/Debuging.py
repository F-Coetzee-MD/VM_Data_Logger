import Handel
import matplotlib
matplotlib.use('TkAgg')  # Switch to the TkAgg backend

import matplotlib.pyplot as plt
import numpy as np
import socket as sct
import json
import os
import random
import math
import time 
from collections import deque
from sklearn.metrics import mean_squared_error as mse


# Global variable for the loop condition
gb_KeepRunning = True
gb_UseDemoData = False
gi_PollsToAvrage = 25
gf_SmoothingConstant = 0.15
gi_ClearMemoryMSE = 1 #Not Active
gi_MoveingAverageWindowSize = 3
gi_ExpectedDataLength = 270

# Character used for splitting data
gc_SplitChar = ','
gc_StartingChar = '('
gc_EndingChar = ')'

gi_LimPlotY1 = (0, 0.3)
gi_LimPlotX1 = (0, 270)
garr_Colors = ['red'] * 50 + ['blue'] * 50 + ['green'] * 50 + ['orange'] * 50 + ['purple'] * 50 + ['pink']*50

gi_LimPlotYTOP = (-0.5, 0.5)
gi_LimPlotXTOP = (-0.5, 0.5)

gi_LimPlotYFFT = (0, 8)
gi_LimPlotXFFT = (-45, 270-45)
gi_DataSaveCount = 0

plt.ion()
fig, (ax2, ax1) = plt.subplots(2, 1)
line, = ax1.plot([], [], 'b-')
line2, = ax2.plot([], [], 'r-')
plt.subplots_adjust(wspace=0.4, hspace=0.6)
figfft, (fft_mag, fft_rad) = plt.subplots(2, 1)
fftline, = fft_mag.plot([], [], 'b-')
fftradline, = fft_rad.plot([], [], 'b-')
plt.subplots_adjust(wspace=0.4, hspace=0.6)
figavg, (ax2avg, ax1avg) = plt.subplots(2, 1)
lineavg, = ax1avg.plot([], [], 'b-')
line2avg, = ax2avg.plot([], [], 'r-')
plt.subplots_adjust(wspace=0.4, hspace=0.6)
figifft, (ifft_mag, ifft_rad) = plt.subplots(2, 1)
ifftline, = ifft_mag.plot([], [], 'b-')
ifftradline, = ifft_rad.plot([], [], 'b-')
plt.subplots_adjust(wspace=0.4, hspace=0.6)

def GetDistance(x1, x2, y1, y2):
    return math.sqrt(pow(x1-x2, 2) + pow(y1-y2, 2))

def GetPointIndexes(parr_Data):
    larr_Indexes = []
    larr_Data = []
    larr_X, larr_Y = ConvertForTopPlot(parr_Data)
    for C1 in range(len(parr_Data)-1):
        larr_Data.append(GetDistance(larr_X[C1], larr_X[C1+1], larr_Y[C1], larr_Y[C1+1]))

    larr_Data = np.array(larr_Data)
    lf_Mean = larr_Data.mean()
    lf_STD = larr_Data.std()
    for C1 in range(len(larr_Data)):
        if (larr_Data[C1] <= lf_Mean - (3 * lf_STD)) or (larr_Data[C1] >= lf_Mean + (3*lf_STD)):
            larr_Indexes.append(C1)
    return larr_Indexes

def SimpleFFT(parr_Data):
    larr_Mag, larr_Frequencies = [], []
    larr_FFTResult = np.fft.fft(np.array(parr_Data))
    larr_Mag = np.abs(larr_FFTResult).tolist()
    larr_Frequencies = np.fft.fftfreq(len(parr_Data), 1/1).tolist()
    larr_Phase = np.angle(larr_FFTResult)
    larr_MagReturn = []
    larr_FreqReturn = []
    larr_PhaseReturn = []
    for C1 in range(len(larr_Frequencies)):
        if (larr_Frequencies[C1] < 0.01) or (larr_Frequencies[C1] > 0.04):
            larr_Mag[C1] = 0
            larr_Phase[C1] = 0
        if (larr_Frequencies[C1] > 0):
            larr_MagReturn.append(larr_Mag[C1])
            larr_FreqReturn.append(larr_Frequencies[C1])
            larr_PhaseReturn.append(larr_Phase[C1])


    return larr_MagReturn, larr_FreqReturn, larr_PhaseReturn

def SimpleIFFT(parr_Mag, parr_Phase):
    larr_FFT = np.array(parr_Mag) * np.exp(1j * np.array(parr_Phase))
    larr_Original = np.fft.ifft(larr_FFT)

    return larr_Original

def SmoothRA(parr_Data, alpha = 0.75):
    larr_Smooth = [] 
    lf_RunningAverage = np.array(parr_Data).mean() #Can be change to only use the first value and not the avarge
    for C1 in parr_Data:
        lf_RunningAverage = (lf_RunningAverage *alpha) + (C1 * (1 - alpha))
        larr_Smooth.append(lf_RunningAverage)

    return larr_Smooth 
def AvarageData(parr_Data):
    larr_Average = []
    larr_npData = np.array(parr_Data)
    larr_Average = np.mean(larr_npData, axis=0)
    return larr_Average.tolist()
def MovingAverage(parr_Data):
    larr_Average = []
    for C1 in range(len(parr_Data)-gi_MoveingAverageWindowSize):
        larr_Average.append(np.array(parr_Data[C1:C1+gi_MoveingAverageWindowSize]).mean())
    return larr_Average
def Plotting(parr_Data, ps_DataName):

    if (ps_DataName == "lidar_scan"):
        larr_X, larr_Y = ConvertForTopPlot(parr_Data)
    
        ax1.cla()
        ax1.grid()
        ax1.plot(range(0,len(parr_Data)), parr_Data)
        ax1.set_xlim(gi_LimPlotX1[0], gi_LimPlotX1[1])
        ax1.set_ylim(gi_LimPlotY1[0], gi_LimPlotY1[1])
        ax1.set_title("Lidar Scan (FRONT)")
        ax1.set_xlabel("Sample Number")
        ax1.set_ylabel("Radius")
                    
        ax2.cla()
        ax2.set_title("Lidar Scan (TOP)")
        ax2.set_xlabel("X (mm)")
        ax2.set_ylabel("Y (mm)")
        ax2.plot(larr_X, larr_Y, 'b-', linewidth=0.5)
        ax2.scatter(larr_X, larr_Y)
        ax2.scatter(0,0, c='r')
        #ax2.set_xlim(gi_LimPlotX1[0], gi_LimPlotX1[1])
        ax2.grid()
        #ax2.set_xlim(gi_LimPlotXTOP[0], gi_LimPlotXTOP[1])                  
        #ax2.set_ylim(gi_LimPlotYTOP[0], gi_LimPlotYTOP[1])

        fig.canvas.draw()
    elif (ps_DataName == "ifft_data"):
        larr_X, larr_Y = ConvertForTopPlot(parr_Data)
    
        fft_mag.cla()
        fft_mag.grid()
        fft_mag.plot(range(0,len(parr_Data)), parr_Data)
        fft_mag.set_xlim(gi_LimPlotX1[0], gi_LimPlotX1[1])
        fft_mag.set_ylim(gi_LimPlotY1[0], gi_LimPlotY1[1])
        fft_mag.set_title("(IFFT) lidar Scan (FRONT)")
        fft_mag.set_xlabel("Sample Number")
        fft_mag.set_ylabel("Radius")
                    
        fft_rad.cla()
        fft_rad.set_title("(IFFT) Lidar Scan (TOP)")
        fft_rad.set_xlabel("X (mm)")
        fft_rad.set_ylabel("Y (mm)")                
        fft_rad.plot(larr_X, larr_Y, 'b-', linewidth=0.5)
        fft_rad.scatter(larr_X, larr_Y)
        fft_rad.scatter(0,0, c='r')
        #ax2.set_xlim(gi_LimPlotX1[0], gi_LimPlotX1[1])
        fft_rad.grid()
        #ax2.set_xlim(gi_LimPlotXTOP[0], gi_LimPlotXTOP[1])                  
        #ax2.set_ylim(gi_LimPlotYTOP[0], gi_LimPlotYTOP[1])

        figifft.canvas.draw()
    else:
        larr_X, larr_Y = ConvertForTopPlot(parr_Data)
        
        ax1avg.cla()
        ax1avg.grid()
        x = range(len(parr_Data))
        y = parr_Data

        ax1avg.set_xlim(gi_LimPlotX1[0], gi_LimPlotX1[1])
        ax1avg.set_ylim(gi_LimPlotY1[0], gi_LimPlotY1[1])
        for i in range(1, len(x)):
            ax1avg.plot(x[i-1:i+1], y[i-1:i+1], color=garr_Colors[i])
        ax1avg.set_title("SMOOTH Lidar Scan (FRONT)")
        ax1avg.set_xlabel("Sample Number")
        ax1avg.set_ylabel("Radius")
                    
        ax2avg.cla()
        ax2avg.set_title("SMOOTH Lidar Scan (TOP)")
        ax2avg.set_xlabel("X (mm)")
        ax2avg.set_ylabel("Y (mm)")    
                    
        ax2avg.scatter(larr_X, larr_Y,  c = garr_Colors[0:len(larr_X)])
        ax2avg.plot(larr_X, larr_Y, 'b-', linewidth=0.5)
        ax2avg.scatter(0,0, c='black')
        larr_GetPoints = GetPointIndexes(parr_Data)
        for C1 in range(len(larr_GetPoints)):
            ax2avg.scatter(larr_X[larr_GetPoints[C1]], larr_Y[larr_GetPoints[C1]], c='black')
        #ax2.set_xlim(gi_LimPlotX1[0], gi_LimPlotX1[1])
        ax2avg.grid()

        larr_Mag, larr_Freq, larr_Phase = SimpleFFT(parr_Data)
        fft_rad.cla() 
        fft_rad.grid()
        fft_rad.plot(larr_Freq, larr_Phase, 'y-')
        fft_rad.set_title("Phase")
        fft_rad.set_xlabel("Degrees")
        fft_rad.set_ylabel("Magnitude")

        fft_mag.cla()
        fft_mag.grid()
        fft_mag.plot(larr_Freq, larr_Mag, 'g-')
        fft_mag.set_title("FFT")
        fft_mag.set_xlabel("Frequency (Hz)")
        fft_mag.set_ylabel("Magnitude")
        figfft.canvas.draw()

        larr_Original = SimpleIFFT(larr_Mag, larr_Phase)
        larr_X, larr_Y = ConvertForTopPlot(larr_Original)
        
        ifft_mag.cla()
        ifft_mag.grid()
        x = range(len(larr_Original))
        y = larr_Original

        for i in range(1, len(x)):
            ifft_mag.plot(x[i-1:i+1], y[i-1:i+1], color=garr_Colors[i])
        ifft_mag.set_title("IFFT Lidar Scan (FRONT)")
        ifft_mag.set_xlabel("Sample Number")
        ifft_mag.set_ylabel("Radius")
                    
        ifft_rad.cla()
        ifft_rad.set_title("IFFT Lidar Scan (TOP)")
        ifft_rad.set_xlabel("X (mm)")
        ifft_rad.set_ylabel("Y (mm)")                
        ifft_rad.scatter(larr_X, larr_Y,  c = garr_Colors[0:len(larr_X)])
        ifft_rad.plot(larr_X, larr_Y, 'b-', linewidth=0.5)
        ifft_rad.scatter(0,0, c='black')
        #ax2.set_xlim(gi_LimPlotX1[0], gi_LimPlotX1[1])
        ifft_rad.grid()


def RandomDemoDataGeneration(pi_DataCount = 272):
    #Generate RandomFloats
    larr_Floats = [round(random.uniform(1.0, 4.0), 2)  for _ in range(pi_DataCount)]

    # Convert the list to a string
    ls_Data = gc_StartingChar + ",".join(str(num) for num in larr_Floats) + gc_EndingChar

    return "lidar_scan" + str(ls_Data)

def InitiateServer(ps_Host="127.0.0.1", pi_Port=7900):
    # Function to initiate the server

    # Get the IP and Port values
    ls_HOST = ps_Host  # Get IP
    li_PORT = pi_Port  # Get Port

    # Print the connection details
    print("Connecting on: {}, {}".format(ps_Host, pi_Port))

    # Create a socket object
    lc_sock = sct.socket(sct.AF_INET, sct.SOCK_STREAM)

    # Bind the socket to the host and port
    lc_sock.bind((ls_HOST, li_PORT))

    # Listen for connections
    lc_sock.listen()

    # Accept a connection and get the connection object and address
    print("Waiting for client to connect...")
    conn, addr = lc_sock.accept()

    # Return the connection object
    return conn

def ConvertForTopPlot(parr_Data):
    lf_Angle = -45.0
    lf_interval =  1.0
    larr_X = []
    larr_Y = []

    larr_Angels = []

    for C1 in range(len(parr_Data)):
        larr_Angels.append(lf_Angle)
        lf_Angle += lf_interval

    for C1 in range(len(parr_Data)):
        larr_X.append(math.cos(larr_Angels[C1] * math.pi/180) * parr_Data[C1])
        larr_Y.append(math.sin(larr_Angels[C1] * math.pi/180) * parr_Data[C1])

    return larr_X, larr_Y

def ExstractRIMDataFFT(ps_Data):

    # Split the input data into real and imaginary parts
    larr_Real = ps_Data[0:len(ps_Data)//2]  # Extract the first half as the real values
    larr_Im = ps_Data[len(ps_Data)//2::]  # Extract the second half as the imaginary values

    # Convert the real and imaginary parts to a NumPy array and create a list of complex numbers
    larr_Complex = np.vectorize(complex)(np.array(larr_Real), np.array(larr_Im)).tolist()

    # Return the lists of real values, imaginary values, and complex numbers
    return larr_Real, larr_Im, larr_Complex

def ExstractData(ps_Data):
    # Function to extract data from a string
    global gc_StartingChar
    global gc_SplitChar
    # Find the index of the starting character
    li_SplitPos = ps_Data.index(gc_StartingChar)

    # Extract the data name before the starting character
    ls_DataName = ps_Data[0:li_SplitPos]

    # Print the extracted data name
    print("Data Received: {}".format(ls_DataName))

    # Extract the content after the starting character
    ls_Content = ps_Data[li_SplitPos::]

    # Print the content
    print("Content (First 10):")

    # Split the content into a list using the splitting character
    larr_Data = ls_Content[1:-2].split(gc_SplitChar)
    larr_Data = [float(ls_string) for ls_string in larr_Data]
    if (len(larr_Data) > 10):
        print(larr_Data[0:10])
    else:
        print(larr_Data)

    # Print the length of the resulting list
    print("Data recieved of length: {}".format(len(larr_Data)))

    # Return the data name and the list of data
    return ls_DataName, larr_Data
def Settings():
    global gi_DataSaveCount
    print('#'*100)
    print("Settings: ")
    print("0 : Continue")
    print("1 : Save All Plots and Data")
    print("2 : Save All Plots")
    print("3 : Save All Data")
    print("4 : Save The Next X Data")
    print("5 : Enable Stepping")
    print("6 : Exit Program")
    li_Selection = -1    

    while not(1 <= li_Selection <= 6):
        li_Selection = int(input("Enter the number: "))
        if (li_Selection == 0):
            print('#'*100)
            return 0
        if (li_Selection == 2) or (li_Selection == 1):
            print("Saving Plots initialized. Will commence once data capture has been completed.")
        if (li_Selection == 3) or (li_Selection == 1):
            print("Saving Data initialized. Will commence once data capture has been completed.")
        if (li_Selection == 4):
            li_count = int(input("Enter the number of samples to Save: "))
            gi_DataSaveCount = li_count
    return li_Selection
def GetAvrageOfArray():
    larr_Average = []

    return larr_Average
def main():
    # Main function
    global gb_KeepRunning
    global gb_UseDemoData
    global gi_DataSaveCount
    # Path to the JSON file
    ls_JSONFile = "../backend/settings/communication.json"
    
    # Open the JSON file
    with open(ls_JSONFile, "r") as lf_file:
        # Load the JSON data
        lj_JsonData = json.load(lf_file)

        # Access and process the data
        ls_IP = lj_JsonData["tester ip"]
        li_Port = lj_JsonData["tester port"]

    # Call the InitiateServer function to establish a connection
    ls_Server = 0
    if not(gb_UseDemoData):
        ls_Server = InitiateServer(ps_Host=ls_IP, pi_Port=li_Port)
    #ls_Server = InitiateServer()

    # Print connection status
    print("...Connected...")
    # Enable interactive mode
    
    li_CounterForClearingScreen = 0
    li_SettingSelection = 0
    larr_BigSaveFFT = []
    larr_BigSaveRaw = []
    larr_StorageData = deque(maxlen=gi_PollsToAvrage)
    larr_StorageName = deque(maxlen=gi_PollsToAvrage)
    larr_Average = []
    # Loop until gb_KeepRunning becomes False
    while (gb_KeepRunning):

        # Receive data from the server
        li_CounterForClearingScreen += 1
        ls_Data = ""

        if (gb_UseDemoData):
            ls_Data = RandomDemoDataGeneration()
        else:
            ls_Data = ls_Server.recv(10000).decode()

        # Check if no data is received
        if not ls_Data:
            # Set gb_KeepRunning to False to exit the loop
            gb_KeepRunning = False
        else:
            # Print the received data
            print('#'*50)
            print("Raw Frame: {} ... {}".format(ls_Data[0:25], ls_Data[-25::]))

            #Exstract The data
            ls_DataName, larr_Data = ExstractData(ls_Data)
            """"
            if (gb_UseDemoData):
                if (li_CounterForClearingScreen % 2 == 1):
                    ls_DataName = "lidar_scan"
                else:
                    ls_DataName = "fft_data"
            """
            while len(larr_Data) > gi_ExpectedDataLength:
                larr_Data = larr_Data[0:-1]
            if (ls_DataName == "lidar_scan"):
                larr_StorageData.append(larr_Data)
                larr_StorageName.append(ls_DataName)

            print("Data in storage: ", len(larr_StorageData))

            li_SaveNumber = int(time.time())
            Plotting(larr_Data, ls_DataName)

            #if (ls_DataName == "lidar_scan"):
            #    Plotting(SmoothRA(larr_Data, alpha=0.80), "")
            """
            larr_FilterdData = SmoothRA(AvarageData(larr_StorageData), alpha=gf_SmoothingConstant)
            if (ls_DataName == "lidar_scan"):
                    Plotting(larr_FilterdData, "")
                    print("MSE: {}".format(mse(larr_FilterdData, larr_Data)))
                    if (mse(larr_FilterdData, larr_Data) > gi_ClearMemoryMSE):
                        larr_StorageData.clear()
                        larr_StorageName.clear()
                    #Plotting(SmoothRA(larr_Data, alpha=0.95), "")
            """
            #larr_FilterdData = MovingAverage(SmoothRA(AvarageData(larr_StorageData), alpha=gf_SmoothingConstant))
            larr_FilterdData = MovingAverage(SmoothRA(AvarageData(larr_StorageData), alpha=gf_SmoothingConstant))
            if (ls_DataName == "lidar_scan"):
                    Plotting(larr_FilterdData, "")
                    #print("MSE: {}".format(mse(larr_FilterdData, larr_Data)))
                    #if (mse(larr_FilterdData, larr_Data) > gi_ClearMemoryMSE):
                    #    larr_StorageData.clear()
                    #    larr_StorageName.clear()
                    #Plotting(SmoothRA(larr_Data, alpha=0.95), "")
            if (li_SettingSelection == 2) or (li_SettingSelection == 1):
                print("Saving All Plots...")
                if not(os.path.exists('Data/lidar_scan_{}.png'.format(li_SaveNumber))):
                    fig.savefig('Data/lidar_scan_{}.png'.format(li_SaveNumber))
                    figfft.savefig('Data/fft_data_{}.png'.format(li_SaveNumber))
                    figavg.savefig('Data/avg_data_{}.png'.format(li_SaveNumber))
                    print("Saved")
                    li_SettingSelection = 0
                else:
                    print("Saving FAILED (PLOT)!!!")
                    input()
            else:
                Plotting(larr_Data, ls_DataName)
            """"
            if (li_SettingSelection == 4):
                if (gi_DataSaveCount > 0):
                    for C1 in larr_Data:
                        larr_BigSaveRaw.append(C1)

            #larr_Real, larr_Imaginary, larr_Complex = ExstractRIMDataFFT(larr_Data)
            if (li_SettingSelection == 1) or (li_SettingSelection == 3):
                print("Saving All Data...")
                
                if not(os.path.exists('Data/lidar_scan_{}.csv'.format(li_SaveNumber))):
                    Handel.SaveToCSV(range(len(larr_Data)), larr_Data, 'Data/lidar_scan_{}.csv'.format(li_SaveNumber))
                    #Handel.SaveToCSV(larr_Real, larr_Imaginary, 'Data/fft_data_{}.csv'.format(li_SaveNumber), "Real", "Img")
                    print("Saved")
                    if (li_SettingSelection != 1):
                        li_SettingSelection = 0
                else:
                    print("Saving FAILED (DATA)!!!")
                    input()

            if (li_SettingSelection == 2) or (li_SettingSelection == 1):
                print("Saving All Plots...")
                if not(os.path.exists('Data/lidar_scan_{}.png'.format(li_SaveNumber))):
                    fig.savefig('Data/lidar_scan_{}.png'.format(li_SaveNumber))
                    figfft.savefig('Data/fft_data_{}.png'.format(li_SaveNumber))
                    print("Saved")
                    li_SettingSelection = 0
                else:
                    print("Saving FAILED (PLOT)!!!")
                    input()
            
            if (li_SettingSelection == 4):
                if (gi_DataSaveCount > 0):
                    for C1 in larr_Complex:
                        larr_BigSaveFFT.append(C1)
                    gi_DataSaveCount -= 1

        if (li_SettingSelection == 4):
            if (gi_DataSaveCount == 0):
                print("Saving All Data For X Frames")
                if not(os.path.exists('Data/Big_lidar_scan_{}.csv'.format(li_SaveNumber))):
                    Handel.SaveToCSV(range(len(larr_BigSaveRaw)), larr_BigSaveRaw, 'Data/Big_lidar_scan_{}.csv'.format(li_SaveNumber))
                    Handel.SaveToCSV(range(len(larr_BigSaveFFT)), larr_BigSaveFFT, 'Data/Big_fft_data_{}.csv'.format(li_SaveNumber), "Real", "Img")
                    print("Saved")
                    li_SettingSelection = 0
                else:
                    print("Saving FAILED (X)!!!")
                    input()
            """
            plt.pause(0.01)
            if (li_CounterForClearingScreen % 2 == 1):
                if (li_SettingSelection == 5):
                    li_Input = input("Press Enter For Next Data (0 to stop stepping, 1 for settings, 2 for saving): ")
                    if ('0' == li_Input):
                        li_SettingSelection = 0
                    if (li_Input == '1'):
                        li_SettingSelection = Settings()  
                    if (li_Input == '2'): 
                        print("Saving All Data...")
                        if not(os.path.exists('Data/lidar_scan_{}.csv'.format(li_SaveNumber))):
                            Handel.SaveToCSV(range(len(larr_Data)), larr_Data, 'Data/lidar_scan_{}.csv'.format(li_SaveNumber))
                            #Handel.SaveToCSV(larr_Real, larr_Imaginary, 'Data/fft_data_{}.csv'.format(li_SaveNumber), "Real", "Img")
                            print("Saved")
                        else:
                            print("Saving FAILED (DATA)!!!")
                            input()
                        print("Saving All Plots...")
                        if not(os.path.exists('Data/lidar_scan_{}.png'.format(li_SaveNumber))):
                            fig.savefig('Data/lidar_scan_{}.png'.format(li_SaveNumber))
                            print("Plot has been saved to", 'Data/lidar_scan_{}.png'.format(li_SaveNumber))
                            figfft.savefig('Data/fft_data_{}.png'.format(li_SaveNumber))
                            print("Plot has been saved to", 'Data/fft_data_{}.png'.format(li_SaveNumber))
                            print("Saved")
                        else:
                            print("Saving FAILED (PLOT)!!!")
                            input()         
            # Send a response to the server
            if not(gb_UseDemoData):
                ls_Server.sendall(b"Data Received And Processed!!!")

    # Turn off interactive mode
    plt.ioff()
    # Show the plot
    plt.show()

if (__name__ == "__main__"):
    ls_JSONFile = "../backend/settings/lida_filtering.json"
    
    # Open the JSON file
    with open(ls_JSONFile, "r") as lf_file:
        # Load the JSON data
        lj_JsonData = json.load(lf_file)

        # Access and process the data

        gf_SmoothingConstant = lj_JsonData["ra_smoothfilter"]
        gi_MoveingAverageWindowSize = lj_JsonData["average_windowsize"]
        gi_PollsToAvrage = lj_JsonData["average_pollscount"]
        gi_ExpectedDataLength = lj_JsonData["expexcted_datalength"]

        print('#'*175)
        print("ra_smoothfilter ", gf_SmoothingConstant)
        print("average_windowsize ", gi_MoveingAverageWindowSize)
        print("average_pollscount ", gi_PollsToAvrage)
        print("expexcted_datalength ", gi_ExpectedDataLength)
        print('#'*175)
        input()


    main()
    print("Programm Ended")
