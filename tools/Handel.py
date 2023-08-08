import matplotlib.pyplot as plt
import csv

def SaveToCSV(data1, data2, FileName, XName="X", YName="Y"):
    """
    Saves data to a CSV file.

    Args:
        data1: First data array.
        data2: Second data array.
        FileName: Name of the CSV file to be created.
        XName: Name of the X column (default: "X").
        YName: Name of the Y column (default: "Y").
    """

    # Create the CSV file
    csv_file = FileName

    # Assign the input data to arrays
    array1 = data1
    array2 = data2

    # Get the data types of the arrays
    ls_DataType1 = str(type(array1[0]))
    ls_DataType2 = str(type(array2[0]))

    # Check if the first data array contains complex numbers
    if type(array1[0]) == type(complex(1+1j)):
        print("Complex numbers detected in {}".format(XName))

    # Check if the second data array contains complex numbers
    if type(array2[0]) == type(complex(1+1j)):
        print("Complex numbers detected in {}".format(YName))

    with open(csv_file, 'w', newline='') as file:
        writer = csv.writer(file)

        # Write the data to the CSV file
        writer.writerow([XName, YName])  # Write the header row
        writer.writerow([ls_DataType1, ls_DataType2])  # Write the data types row
        writer.writerows(zip(array1, array2))  # Write the data rows

    print("Data has been written to", csv_file)


def LoadFromCSV(FileName):
    """
    Loads data from a CSV file.

    Args:
        FileName: Name of the CSV file to load data from.

    Returns:
        larr_X: Loaded X data array.
        larr_Y: Loaded Y data array.
        ls_XName: Name of the X column.
        ls_YName: Name of the Y column.
    """

    # Initialize variables
    larr_X = []
    larr_Y = []
    ls_XName = ""
    ls_YName = ""

    with open(FileName + '.csv', newline='') as csvfile:
        data = list(csv.reader(csvfile))

    # Extract column names and data types
    ls_XName, ls_YName = data[0]
    ls_DataType1, ls_DataType2 = data[1]

    # Extract X and Y data
    larr_XTemp = data[2::]
    larr_YTemp = data[2::]

    # Convert data to appropriate types and populate X and Y arrays
    for C1 in range(len(larr_XTemp)):
        if ls_DataType1 == str(type(complex(1+1j))):
            larr_X.append(complex(larr_XTemp[C1][0]))
        if ls_DataType2 == str(type(complex(1+1j))):
            larr_Y.append(complex(larr_YTemp[C1][1]))

        if ls_DataType1 == str(type(1412)):
            larr_X.append(int(larr_XTemp[C1][0]))
        if ls_DataType2 == str(type(1412)):
            larr_Y.append(int(larr_YTemp[C1][1]))

        if ls_DataType1 == str(type(1412.0)):
            larr_X.append(float(larr_XTemp[C1][0]))
        if ls_DataType2 == str(type(1412.0)):
            larr_Y.append(float(larr_YTemp[C1][1]))

    return larr_X, larr_Y, ls_XName, ls_YName

def AnimatePlot(x, y, d):
    
    plt.subplot(2, 1, 1)
    plt.cla()
    plt.title("SBS Lidar 1")
    plt.xlabel("X (mm)")
    plt.ylabel("Y (mm)")
    plt.grid()
    plt.plot(x, y)
    plt.scatter(0, 0)

    plt.subplot(2, 1, 2)
    plt.grid()
    plt.title("SBS Lidar 1")
    plt.xlabel("Sample Number")
    plt.ylabel("Radius")
    plt.plot(range(len(d)), d)
    plt.savefig("SBS Lidar 1.png")
    plt.show()