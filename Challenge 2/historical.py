# This program displays a historical view of temperature data that has been grabbed from multiple sensors

import matplotlib.pyplot as plt
import numpy as np
from time import sleep

# Check if file exists
find_file_timeout = 0
timeout = 0

while True:
    try:
        test = np.loadtxt("data.txt")
        break
    except:
        print("Data File does not yet exist")
        sleep(.1)
        find_file_timeout += 1
        if find_file_timeout > 20:
            print("Timeout, File Not Found")
            timeout = 1
            break
        

# Main piece of code
if timeout == 0:
    readrows = 0
    
    id = []
    temp = []
    time = []
    
    while True:
    
        # Import data from file
        new_data = np.loadtxt("data.txt",skiprows = readrows, dtype = int)
        readrows = readrows + len(new_data)
        
        id.append(new_data[0,:])
        temp.append(new_data[1,:])
        time.append(new_data[2,:])
    
        new_data = []
        plt.plot(temp)
    
