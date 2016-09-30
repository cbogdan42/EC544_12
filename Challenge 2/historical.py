# This program displays a historical view of temperature data that has been grabbed from multiple sensors

import matplotlib.pyplot as plt
import numpy as np
from time import sleep
import sqlite

# Note that in order for graphics to be externalized, it must be changed in the spyder console

# Database Name

db_filename = 'data_from_sensor.db'
connection = sqlite.connect(db_filename)
cursor = connection.cursor()

# Figure name
hist_fig_name = 'hist.jpg'
current_fig_name = 'current.jpg'

# Averaging Values
delta_t = 5
        
plt.figure()

# Main piece of code
   
    
while True:
    
### Create Historical Graph w/ Min and Max and Average Per Day

# Query Database for list of all dates available

# For Each Date, Calculate Average, Max and Min

### Create Historical Graph for each Sensor

# Query Database for list of all sensors

# For Each Sensor, grab all data ordered on date then time

# Print to Plot, Av/Max/Min on top, All datapoints below

(ax1,ax2) = plt.subplots(2,1)
ax1.plot()
ax2.plot()
savefig(hist_fig_name)

### Create Current Graph

# Query Database for all data in last delta_t min and take average

latest_data = 
current_avg = sum(latest_data)/len(latest_data)



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
            
        
            
    

