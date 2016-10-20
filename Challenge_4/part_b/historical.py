# This program displays a historical view of temperature data that has been grabbed from multiple sensors

import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from time import sleep
import sqlite3
import PIL
from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw
import os
import datetime

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
sensor_hist_times = {}

sensor_hist_list = []

        
plt.figure()

# Main piece of code
   
while True:
    
    ### Create Historical Graph w/ Min and Max and Average Per Day
    
    # Query Database for list of all dates available
    cursor.execute('SELECT DISTINCT date FROM sensor_data')
    dates = [tup[0] for tup in cursor.fetchall()]

    date_format = []
        
        
    # For Each Date, Calculate Average, Max and Min
    for n in dates:
        cursor.execute('SELECT sensor_output FROM sensor_data WHERE date= ?',[n])
        temp_val = [tup[0] for tup in cursor.fetchall()]
        max_temp[n] = max(temp_val)
        min_temp[n] = min(temp_val)
        average_temp[n] = sum(temp_val)/len(temp_val)
        date_format.append(datetime.date(int(n[0:4]),int(n[4:6]),int(n[6:8])))
        
        
    # Format list of dates into plottable dates    
    date_plt = matplotlib.dates.date2num(date_format)
    x_up_lim = max(date_format) + datetime.timedelta(days = 2)
    x_low_lim = min(date_format) + datetime.timedelta(days = -2)
    x_up_plt = matplotlib.dates.date2num(x_up_lim)
    x_low_plt = matplotlib.dates.date2num(x_low_lim)

    
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
        cursor.execute('SELECT date FROM sensor_data WHERE sensor_id = {sid} ORDER BY date, time'.\
                       format(sid = n))
        sensor_hist_dates[n] = [tup[0:1] for tup in cursor.fetchall()]
        cursor.execute('SELECT time FROM sensor_data WHERE sensor_id = {sid} ORDER BY date, time'.\
                       format(sid = n))
        sensor_hist_times[n] = [tup[0:1] for tup in cursor.fetchall()]
                          

    # Print to Plot, Av/Max/Min on top, All datapoints below
    max_temp_list = []
    min_temp_list = []
    avg_temp_list = []

    # Turn max, min & avg into lists
    for n in range(len(dates)):
        max_temp_list.append(max_temp[dates[n]])
        min_temp_list.append(min_temp[dates[n]])
        avg_temp_list.append(average_temp[dates[n]])
        
    y_up_lim = max(max_temp_list) + 2
    y_low_lim = min(min_temp_list) - 2
            
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    ax1.plot_date(date_format,max_temp_list, label = 'Max Temp')
    ax1.plot_date(date_format,min_temp_list, label = 'Min Temp')
    ax1.plot_date(date_format,avg_temp_list, label = 'Avg Temp')
    ax1.set_xlim([x_low_lim,x_up_lim])
    ax1.set_ylim([y_low_lim,y_up_lim])
    fig.autofmt_xdate()
    plt.ylabel("temp, deg C")
    plt.xlabel("Date of Reading")
    plt.legend(loc = 'upper right')
    plt.savefig(hist_avg_fig_name)
    plt.close()
        
    sensor_hist_dates_list = []
    sensor_hist_times_list = []
    sensor_hist_list = []

    # Get List for each sensor of data
    for n in sensors:
        sensor_hist_list.extend(sensor_hist[n])
        sensor_hist_dates_list.extend(sensor_hist_dates[n])
        sensor_hist_times_list.extend(sensor_hist_times[n])
        
    shd_format = []
    shd_nums = []

    for n in range(len(sensor_hist_dates_list)):
        date_holder = datetime.date(int(sensor_hist_dates_list[n][0][0:4]),int(sensor_hist_dates_list[n][0][4:6]),int(sensor_hist_dates_list[n][0][6:8]))
        time_holder = datetime.time(int(sensor_hist_times_list[n][0][0:2]),int(sensor_hist_times_list[n][0][2:4]),int(sensor_hist_times_list[n][0][4:6]))
        combo = datetime.datetime.combine(date_holder,time_holder)
        shd_format.append(datetime.datetime.combine(date_holder,time_holder))
        shd_nums.append(matplotlib.dates.date2num(combo))
        
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    ax1.plot_date(shd_nums, sensor_hist_list)
    fig.autofmt_xdate()
    plt.ylabel("temp, deg C")
    plt.xlabel("Date/Time of Reading, in Num form")
    plt.savefig(hist_all_fig_name)
    plt.close()
    
    ### Create Current Graph
    
    # Query Database for all  in last delta_t min and take average
    cursor.execute('SELECT sensor_output FROM sensor_data WHERE time < Time("now", "-{delt_t} minutes")'.\
                       format(delt_t = delta_t))
    latest_data = cursor.fetchall()
    recent_data_check = 1
    
    if latest_data == []:
        current_avg = 'No Recent Data'
        recent_data_check = 0
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
    if recent_data_check == 1:
        draw.text((110,80),str(int(current_avg))+" deg C",(255,255,255))
    else:
        draw.text((110,80),current_avg)
    img.save(current_avg_fig_name)
    
    # Print to Plot, Different Temps w/ Nest Avg
    plt.bar(sensors,current_avg_ind, color = 'blue')
    plt.ylabel("temp, deg C")
    plt.xticks(sensors,sensors)
    plt.savefig(current_ind_fig_name)
    
    #Move generated images to directory where the server reads it from
    cmd = "cp current_avg.jpg current_ind.jpg hist_all.jpg hist_avg.jpg ./data_server/public/images"
    os.system(cmd)

    # Wait for a bit before updating again
    sleep(update)
