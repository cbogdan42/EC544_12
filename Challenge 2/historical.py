# This program displays a historical view of temperature data that has been grabbed from multiple sensors

import matplotlib.pyplot as plt
import numpy as np
from time import sleep
import sqlite3
import PIL
from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw
import os

# Note that in order for graphics to be externalized, it must be changed in the spyder console

# Database Name

db_filename = 'data_from_sensor.db'
connection = sqlite3.connect(db_filename)
cursor = connection.cursor()

# Figure name
hist_avg_fig_name = 'hist_avg.jpg'
hist_all_fig_name = 'hist_all.jpg'
current_ind_fig_name = 'current_ind.jpg'
current_avg_fig_name = 'current_avg.jpg'

# Averaging Values
delta_t = 5

# Open nest picture for base of current fig
img = Image.open("nest.jpg")
draw = ImageDraw.Draw(img)
#font = ImageFont.truetype("sans-serif.ttf",16)

# Update Rate
update = .5

# Open Variables
max_temp = {}
min_temp = {}
average_temp = {}
sensor_hist = {}
sensor_hist_dates = {}

sensor_hist_list = []

        
plt.figure()

# Main piece of code
   
    
while True:
    
    ### Create Historical Graph w/ Min and Max and Average Per Day
    
    # Query Database for list of all dates available
    cursor.execute('SELECT DISTINCT date FROM sensor_data')
    dates = [tup[0] for tup in cursor.fetchall()]
    
    # For Each Date, Calculate Average, Max and Min
    for n in dates:
        cursor.execute('SELECT sensor_output FROM sensor_data WHERE date= ?',[n])
        temp_val = [tup[0] for tup in cursor.fetchall()]
        max_temp[n] = max(temp_val)
        min_temp[n] = min(temp_val)
        average_temp[n] = sum(temp_val)/len(temp_val)
    
    ### Create Historical Graph for each Sensor
    
    # Query Database for list of all sensors
    cursor.execute('SELECT DISTINCT sensor_id FROM sensor_data')
    sensors = [tup[0] for tup in cursor.fetchall()]
    current_avg_ind= np.zeros(len(sensors))
    
    # For Each Sensor, grab all data ordered on date then time
    for n in sensors:
        cursor.execute('SELECT sensor_output FROM sensor_data WHERE sensor_id = {sid} ORDER BY date, time'.\
                       format(sid = n))
        sensor_hist[n] = [tup[0] for tup in cursor.fetchall()]
        cursor.execute('SELECT date,time FROM sensor_data WHERE sensor_id = {sid} ORDER BY date, time'.\
                       format(sid = n))
        sensor_hist_dates[n] = [tup[0:1] for tup in cursor.fetchall()]

    # Print to Plot, Av/Max/Min on top, All datapoints below
    max_temp_list = []
    min_temp_list = []
    avg_temp_list = []

    # Turn max, min & avg into lists
    for n in range(len(dates)):
        max_temp_list.append(max_temp[dates[n]])
        min_temp_list.append(min_temp[dates[n]])
        avg_temp_list.append(average_temp[dates[n]])
            
    plt.plot(range(len(dates)),max_temp_list)
    plt.plot(range(len(dates)),min_temp_list)
    plt.plot(range(len(dates)),avg_temp_list)
    plt.savefig(hist_avg_fig_name)
    plt.close()
        
    # Get List for each sensor of data
    for n in sensors:
        sensor_hist_list.extend(sensor_hist[n])
        plt.plot(range(len(sensor_hist_list)), sensor_hist_list)
        sensor_hist_list = []

    plt.savefig(hist_all_fig_name)
    plt.close()
    
    ### Create Current Graph
    
    # Query Database for all  in last delta_t min and take average
    cursor.execute('SELECT sensor_output FROM sensor_data WHERE time < Time("now", "-{delt_t} minutes")'.\
                       format(delt_t = delta_t))
    latest_data = cursor.fetchall()
    if latest_data == []:
        current_avg = 'No Recent Data'
    else:
        current_avg = sum(sum(tuple) for tuple in latest_data)/len(latest_data)
    
    # For each sensor, query database for all data in last delta_t min and take average
    for n in sensors:
        cursor.execute('SELECT sensor_output FROM sensor_data WHERE time < Time("now", "-{delt_t} minutes") AND sensor_id = {sid}'.\
                       format( delt_t = delta_t, sid= n))
        latest_data_ind = cursor.fetchall()
        if latest_data_ind == []:
            current_avg_ind[n] = 0.0
        else:
            current_avg_ind[n] = sum(sum(tuple) for tuple in latest_data_ind)/len(latest_data_ind)
    
    # Create Nest fig
    draw.text((0,0),str(current_avg),(255,255,255))
    img.save(current_avg_fig_name)
    
    # Print to Plot, Different Temps w/ Nest Avg
    plt.bar(sensors,current_avg_ind, color = 'blue')
    plt.savefig(current_ind_fig_name)
    
    #Move generated images to directory where the server reads it from
    cmd = "cp current_avg.jpg current_ind.jpg hist_all.jpg hist_avg.jpg ./data_server/public/images"
    os.system(cmd)

    # Wait for a bit before updating again
    sleep(update)
            
        
            
    

