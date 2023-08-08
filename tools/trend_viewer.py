##################
# Module Imports #
##################

# import module for reading csv files
import pandas as pd
# import module for reading csv headers
import csv
# import module for system commands
import os
# import module used for getting file names
from os import listdir
# import module for creating GUI
import tkinter as tk
# import module for plotting data
import seaborn as sns
# import module for plotting data
import matplotlib.pyplot as plt
# import module for adding x values to graphs
import matplotlib.ticker as ticker
# import module for creating GUI
import tkinter as tk
# iport module that allows for date selection in GUI
from tkcalendar import Calendar
# import module for determining the date and time
import datetime as dt

###################
# Global Variable #
###################

data_folder_locations   = ["./data/"]
checkBoxValues          = {}
dateTimeData            = []
checkBoxes              = {}
dateTime                = dt.datetime.now()
logPlot                 = False
f                       = ('Times', 20)

#####################
# Program Functions #
#####################

def ShowTrendData(plotData):
    if len(plotData) >= 1: 
        try:
            plt.close("all")
        finally:
            plotData.index = plotData.index.map(plotData["date time"])
            sns.set_theme(style="whitegrid")
            plot = sns.lineplot(data=plotData, palette="tab10", linewidth=2.5)
            plot.xaxis.set_major_locator(ticker.LinearLocator(16))
            plt.title("Trend Viewer")
            if logPlot: plt.yscale('log')
            plt.show()

def ScaleDataForMultiPlot():
    print()

def SortData(data): 
    # make it that this function is called only when needed, 
    # determined in GetDataToPlot() function
    # remove samples taken on the same date and time
    return data

def GetDataToPlot(variable_list, start, stop):
    # remove possible duplicates from variable_list
    variable_list = list(dict.fromkeys(variable_list))
    # create needed fields for dataframe
    fields = ["date time"]
    for current_variable in variable_list:
        fields.append(current_variable)
    tempData = {}
    plotData = pd.DataFrame(columns = fields)
    for current_folder in data_folder_locations:
        files_in_folder = os.listdir(current_folder)
        files_in_folder.sort()
        breakFromFolder = False
        for current_file in files_in_folder:
            if not current_file.startswith("data_"):    continue
            if not current_file.endswith(".csv"):       continue
            if current_file[5:24] < start:              continue
            current_data = pd.read_csv(current_folder+current_file)
            for current_row_number in range(len(current_data)):
                # append date and time to tempData
                try: tempData["date time"] = (
                str(current_data.iloc[current_row_number]["date"])+"\n"+
                str(current_data.iloc[current_row_number]["time"]))
                except: break
                if tempData["date time"][0:19] > stop: 
                    breakFromfolder = True
                    break
                for current_variable in variable_list:
                    # if logged value does exist, append value
                    try: tempData[current_variable] = float(
                    current_data.iloc[current_row_number][current_variable])
                    # if logged value does NOT exist append 0 to data
                    except: tempData[current_variable] = float(0)
                # add new values to plotData
                plotData = plotData._append(tempData, ignore_index = True)
            if breakFromFolder == True: break
    plotData = SortData(plotData)
    return plotData

def GetVariableData():
    found_variable = []
    for data_folder in data_folder_locations:
        files_in_folder = os.listdir(data_folder)
        for current_file in files_in_folder:
            if not current_file.endswith(".csv"): continue
            variables = csv.DictReader(open(data_folder+current_file))
            for current_variable in variables.fieldnames:
                if current_variable not in found_variable:
                    found_variable.append(current_variable)
    return found_variable

def GenerateTrendSelector(frameA):
    for variable in allVariableData:
        checkBoxValues[variable] = tk.IntVar()
        checkBoxes[variable] = tk.Checkbutton(frameA, 
        text = variable, onvalue = 1, background = "white",
        offvalue = 0, variable = checkBoxValues[variable])
        checkBoxes[variable].pack()

def NumpadPressed(num):
    print()

def GenerateTrends(dateTimeData, checkBoxValues):
    variablesToPlot = []
    keys = list(checkBoxValues.keys())
    for key in keys:
        if checkBoxValues[key].get():
            variablesToPlot.append(key)
    startYear   = "20" + str(dateTimeData[0].get_date().split("/")[2])
    stopYear    = "20" + str(dateTimeData[1].get_date().split("/")[2])
    startMounth = str(dateTimeData[0].get_date().split("/")[0])
    startDay    = str(dateTimeData[0].get_date().split("/")[1])
    stopMounth  = str(dateTimeData[1].get_date().split("/")[0])
    stopDay     = str(dateTimeData[1].get_date().split("/")[1])
    startHour   = str(dateTimeData[2].get())
    startMinute = str(dateTimeData[3].get())
    startSecond = str(dateTimeData[4].get())
    stopHour    = str(dateTimeData[5].get())
    stopMinute  = str(dateTimeData[6].get())
    stopSecond  = str(dateTimeData[7].get())
    if len(startMounth) < 2: startMounth    = "0" + startMounth
    if len(startMinute) < 2: startMinute    = "0" + startMinute
    if len(startSecond) < 2: startSecond    = "0" + startSecond
    if len(stopMounth)  < 2: stopMounth     = "0" + stopMounth
    if len(stopMinute)  < 2: stopMinute     = "0" + stopMinute
    if len(stopSecond)  < 2: stopSecond     = "0" + stopSecond
    if len(startHour)   < 2: startHour      = "0" + startHour
    if len(stopHour)    < 2: stopHour       = "0" + stopHour
    if len(startDay)    < 2: startDay       = "0" + startDay
    if len(stopDay)     < 2: stopDay        = "0" + stopDay
    start = (startYear + "-" + startMounth + "-" + startDay +
    "_" + startHour + "-" + startMinute + "-" + startSecond)
    stop = (stopYear + "/" + stopMounth + "/" + stopDay + 
    "\n" + stopHour + ":" + stopMinute + ":" + stopSecond)
    plotData = GetDataToPlot(variablesToPlot, start, stop)
    ShowTrendData(plotData)

###########################
# GUI Elements and Set-Up #
###########################

root = tk.Tk()

canvasA = tk.Canvas(root, 
width = 50, height = 100, 
scrollregion = (0, 0, 3000, 3000))
canvasA.pack(fill = 'both', 
expand = True, side = tk.LEFT)

frameA = tk.Frame(canvasA)
frameA.pack(side = tk.LEFT)

frameB = tk.Frame(root)
frameB.pack(side = tk.TOP)

frameC = tk.Frame(root)
frameC.pack(side = tk.TOP)

frameD = tk.Frame(root)
frameD.pack(side = tk.TOP)

frameE = tk.Frame(root)
frameE.pack(side = tk.RIGHT)

vbar = tk.Scrollbar(canvasA, orient = 'vertical')
vbar.pack(side = 'right', fill = 'y')
vbar.config(command = canvasA.yview)
canvasA.config(yscrollcommand = vbar.set)
canvasA.create_text(5, 0, anchor = 'n')

canvasA.create_window((200, 0), 
window = frameA, anchor = "n")

# add search function for all variables that can be displayed

# add list off all sensors that can be displayed
allVariableData = GetVariableData()
GenerateTrendSelector(frameA)

# add tickbox for logarithmic scale

# add refresh current date and time button

b1 = tk.Button(frameE, text = "1", command = lambda: NumpadPressed("1"))
b2 = tk.Button(frameE, text = "2", command = lambda: NumpadPressed("2"))
b3 = tk.Button(frameE, text = "3", command = lambda: NumpadPressed("3"))
b4 = tk.Button(frameE, text = "4", command = lambda: NumpadPressed("4"))
b5 = tk.Button(frameE, text = "5", command = lambda: NumpadPressed("5"))
b6 = tk.Button(frameE, text = "6", command = lambda: NumpadPressed("6"))
b7 = tk.Button(frameE, text = "7", command = lambda: NumpadPressed("7"))
b8 = tk.Button(frameE, text = "8", command = lambda: NumpadPressed("8"))
b9 = tk.Button(frameE, text = "9", command = lambda: NumpadPressed("9"))
b0 = tk.Button(frameE, text = "0", command = lambda: NumpadPressed("0"))
bB = tk.Button(frameE, text = "<-", command = lambda: NumpadPressed("b"))

calStart = Calendar(frameB, selectmode = 'day',
year = dateTime.year, month = dateTime.month, 
day = dateTime.day)
calStart.pack()
dateTimeData.append(calStart)

calStop = Calendar(frameC, selectmode = 'day',
year = dateTime.year, month = dateTime.month, 
day = dateTime.day)
calStop.pack()
dateTimeData.append(calStop)

hourStart = tk.Spinbox(frameB,
from_ = 0, to = 23, wrap = True,
font = f, width = 2, justify = tk.CENTER)
hourStart.pack(side = tk.LEFT, 
fill = tk.X, expand = False)
dateTimeData.append(hourStart)

minStart = tk.Spinbox(frameB,
from_ = 0, to = 59, wrap = True,
width = 2, font = f, justify = tk.CENTER)
minStart.pack(side = tk.LEFT, 
fill = tk.X, expand = False)
dateTimeData.append(minStart)

secStart = tk.Spinbox(frameB,
from_ = 0, to = 59, wrap = True,
width = 2, font = f, justify = tk.CENTER)
secStart.pack(side = tk.LEFT, 
fill = tk.X, expand = False)
dateTimeData.append(secStart)

hourStop = tk.Spinbox(frameC,
from_ = 0, to = 23, wrap = True,
font = f, width = 2, justify = tk.CENTER)
hourStop.delete(0, "end")
hourStop.insert(0, dateTime.hour)
hourStop.pack(side = tk.LEFT, 
fill = tk.X, expand = False)
dateTimeData.append(hourStop)

minStop = tk.Spinbox(frameC,
from_ = 0, to = 59, wrap = True,
width = 2, font = f, justify = tk.CENTER)
minStop.delete(0, "end")
minStop.insert(0, dateTime.minute)
minStop.pack(side = tk.LEFT, 
fill = tk.X, expand = False)
dateTimeData.append(minStop)

secStop = tk.Spinbox(frameC,
from_ = 0, to = 59, wrap = True,
width = 2, font = f, justify = tk.CENTER)
secStop.delete(0, "end")
secStop.insert(0, dateTime.second)
secStop.pack(side = tk.LEFT, 
fill = tk.X, expand = False)
dateTimeData.append(secStop)

showTrendsButton = tk.Button(frameD,
text = "Show Trends", padx = 10,
pady = 10, command = lambda : 
GenerateTrends(dateTimeData,
checkBoxValues))
showTrendsButton.pack(pady = 10)

###################
# Main Loop Start #
###################

def main():

    root.mainloop()

# start application in main file
if __name__ == "__main__": main()
