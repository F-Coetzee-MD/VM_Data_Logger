# This script is used to create modbus frames in string format
# that can easily be copied into existing programs, that use tcp/ip
# for their communication with PLC's or any other industrial devices

import os
import json

def ReadJSON(file):
    jsonFile = open(file)
    jsonData = json.load(jsonFile)
    jsonFile.close()
    return jsonData
    
def TakeInput(message):
    if message != "":
        print("\n" + message)
    return input("\n==> ")

def WriteMessage(message):
    os.system("clear")
    if message != "":
        print("\n" + message)

def DisplayOutput(frame_info, frame_type, inputs, bInputs):
    message = "==============\n"
    message += "Frame Results:"
    message += "\n==============\n"
    WriteMessage(message)
    count = 0
    for fi in frame_info[frame_type]:
        print(fi[0:fi.find("{")-1] + 
        " ==> " + str(inputs[count]))
        count += 1
    hex_array = ""
    count = 0
    for x in range(int(len(bInputs)/8)):
        # convert binary to int
        num = int(bInputs[count:count + 8], 2)
        # convert int to hexadecimal
        hex_array += hex(num) + " "
        count += 8
    hex_array = hex_array[0:len(hex_array)-1]
    print("\nBinary Frame:\n\n" + bInputs)
    print("\nHex Array (for c++):\n\n" + hex_array)
    input("\n\nPress any key to restart")

def GetFrameData(frame_info, frame_type):
    message = "==============\n"
    message += "CT Frame Info:"
    message += "\n==============\n"
    WriteMessage(message)
    inputs = []
    for p in frame_info[frame_type]:
        inputs.append(int(TakeInput(p + "?")))
        # redudency for bad data provided
    return inputs

def CreateFrame(frame_info, frame_type):
    inputs = GetFrameData(frame_info, frame_type)
    names = frame_info[frame_type]
    bInputs = ""
    for i in range(len(inputs)):
        val = format(inputs[i],'08b')
        bCount = int(names[i][names[i].find("{") +
        1:names[i].find("}")])
        while len(val) < bCount:
            val = "0" + val
        bInputs += val
    DisplayOutput(frame_info, frame_type, inputs, bInputs)

def Main():
    frame_info = ReadJSON("./modbus_types.json")
    while True:
        message = "============="
        for t in frame_info["Types"]:
            message += "\n" + t
        message += "\n============="
        WriteMessage(message)
        inpt = TakeInput("")
        if inpt == "exit": break
        frame_type = ""
        invalid_message = True
        for t in frame_info["Types"]:
            if inpt == t:
                frame_type = t
                invalid_message = False
                break
        if invalid_message:
            WriteMessage("Invalid modbus frame type")
            Main()
        CreateFrame(frame_info, frame_type)

if __name__ == "__main__":
    Main()