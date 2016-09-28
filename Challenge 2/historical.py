# This program displays a historical view of temperature data that has been grabbed from multiple sensors

import matplotlib.pyplot as plt
import numpy as np
from time import sleep

# Note that in order for graphics to be externalized, it must be changed in the spyder console

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
        
plt.figure()

# Main piece of code
if timeout == 0:
    readrows = 0
    
    id = []
    temp = []
    time = []
    
    while True:
    
        # Import data from file
        new_data = np.loadtxt("data.txt",skiprows = readrows, dtype = int)

        #find length of read rows
        print(len(new_data.shape))
        print(new_data.shape)
        if len(new_data.shape) == 1:
            readrows = readrows + 1
            print(readrows)
            print("I added 1")
        else:
            try:
                readrows = readrows + new_data.shape[0]
                print(readrows)
                print("I added more than 1")
            except:
                pass
                           
        
        try:
            id.extend(new_data[:,0])
            temp.extend(new_data[:,1])
            time.extend(new_data[:,2])        
            new_data = []
            plt.clf
            plt.plot(range(len(temp)),temp, 'k')
            plt.pause(.5)
            print("Got here!")
    
        except:
            print("No New Data")
            sleep(.1)
        
        # Add fake new line

        target = open("data.txt",'a')
        for n in range(2):
            target.write("4 23 46 15 20 20 \n")
        target.close
            
        
            
    

