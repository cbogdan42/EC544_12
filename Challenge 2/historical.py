// This program displays a historical view of temperature data that has been grabbed from multiple sensors

import matplotlib as plot
import numpy as np

// Check if file exists

while True:
	try:
	    test = np.loadtxt(data.txt)
        break
	except:
		print("Data File does not yet exist")


// Main piece of code
readrows = 0

id = []
temp = []
time = []

while True:

	// Import data from file
    new_data = np.loadtxt(data.txt,skiprows = readrows)
    readrows = readrows + len(new_data)
    
    id.append(new_data[0,:])
    temp.append(new_data[1,:])
    time.append(new_data[2,:])

    new_data = []
    plot.plot(temp)
    
//