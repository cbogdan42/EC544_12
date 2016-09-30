#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Sep 30 12:31:52 2016

@author: Ritika
"""

import sqlite3

# Note that in order for graphics to be externalized, it must be changed in the spyder console

# Database Name

db_filename = 'data_from_sensor.db'
connection = sqlite3.connect(db_filename)
cursor = connection.cursor()
id = 1

#cursor.execute("SELECT name FROM sqlite_master WHERE type = 'table';")
#print(cursor.fetchall())
cursor.execute('SELECT DISTINCT date FROM sensor_data')
cursor.execute('SELECT sensor_output FROM sensor_data WHERE date= "9/30/2016"')
cursor.execute('SELECT DISTINCT sensor_id FROM sensor_data')
cursor.execute('SELECT * FROM sensor_data ORDER BY date, time')
cursor.execute('SELECT * FROM sensor_data ORDER BY date, time WHERE sensor_id = %d' %id)
all_rows = cursor.fetchall()

#print (all_rows)
cursor.execute('SELECT sensor_output FROM sensor_data WHERE time < Time("now", "-60 minutes")')
all_rows = cursor.fetchall()

print (all_rows)

connection.close()