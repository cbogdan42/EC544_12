#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 11 08:09:15 2016
@author: Ritika
"""
# -*- coding: utf-8 -*-
"""
Created on Thu Nov 10 10:00:38 2016
@author: cbogdan
"""
#import modules
import sys
import pygame
from time import sleep
import math
import operator
import numpy
from collections import Counter
import numpy as np
# Initiate Variables for kNN
data = []
rssi = []
test = []
k = 3
current_rssi = []
### Initiate PyGame Interface Variables
# Initial Position of Robot
position=[30,30]
# Beacon Locations
Beacons = {0:[0,0],1:[24,0],2:[24,100],3:[0,100]}
# Colors
white=[255,255,255]
black = [0,0,0]
light_red = [239,129,129]
light_blue= [0,128,255]
light_green = [0,204,0]
red = [255,0,0]
green = [0,255,0]
blue = [0,0,255]
# Setup Background Map - 1 pixel = 5 cm
outer_wall_points = [(20,780),(20,20),(580,20),(580,780)]
inner_wall_points = [(100,100),(500,100),(500,500),(350,500),(350,400),(250,400),(250,500),(100,500)]
lower_wall_points = [(100,780),(100,580),(500,580),(500,780)]
# Rectangle Areas
Rec_1 = (22,22,78,78)
Rec_2 = (100,22,401,77)
Rec_3 = (501,22,78,78)
Rec_4 = (502,100,77,401)
Rec_5 = (501,501,78,79)
Rec_6 = (99,502,402,77)
Rec_8 = (22,502,78,78)
Rec_9 = (22,100,77,402)
Rec_7 = (251,401,99,101)
Rec_11 = (22,580,78,200)
Rec_10 = (501,580,78,200)
      
        
### Define Functions
# Read in RSSI Values from JS
def GetRSSI():
    RSSI_file = open('data_rssi.txt','r')
    current_RSSI = RSSI_file.read()
    RSSI_file.close()
    return current_RSSI
    
# This function determines the current bin
def GetBin():
    # Initiate Variables for kNN
    data = []
    rssi = []
    test = []
    k = 3
    current_rssi = []
    # Read in RSSI values from XBee
    #current_RSSI = xbee.read()
    #To open data.txt database file and read the rssi values as floats in an array
    with open('kNN_data.txt') as file:
        for line in file:
            rssi.append([float(n) for n in line.strip().split(',')])
  
    with open('RSSI_data.txt') as file:
        for line in file:
            test.append([float(n) for n in line.strip().split(',')])
            print ('test')
            print (test)

    ###Find Euclidean distance between the vector elements in the dataset
    distance=[]
    for x in range(len(rssi)): 
        temp=[]
        for y in range(4):
            #temp.append(math.sqrt((pow((rssi[x][y]-rssi[x+1][y]),2))))
            temp.append(math.sqrt((pow((rssi[x][y]-test[0][y]),2))))
        distance.append(sum(temp))
    dist = numpy.array(distance)
    sortedind = numpy.argsort(dist)
    distance.sort()
    #print (sortedind[:3]) #Returns the index of the k closest neighbors
    #use [:k]
    position = []
    #assign position to crawler based on majority vote of k neighbors
    for x in range(k):
        position.append(rssi[sortedind[x]][4])
    #print (position)
    val = Counter(position)
#    print('val most common')
#    print(val.most_common(1)[0][0])
    return(val.most_common(1)[0][0])
# This function gets the current distance
def GetDistance():
    
    # Initialize Values
    list_beacons = []
    beacon_vals = []
    distance_vals = []
    current_RSSI = []
    #Current_Position = np.matrix()
    # Get current RSSI values
    current_RSSI_hold = GetRSSI()
    current_RSSI.append(int(current_RSSI_hold[0:2]))
    current_RSSI.append(int(current_RSSI_hold[3:5]))
    current_RSSI.append(int(current_RSSI_hold[7:9]))
    current_RSSI.append(int(current_RSSI_hold[10:12]))
#    print('len current RSSI')
#    print(len(current_RSSI))
    print('current_RSSI')
    print (current_RSSI)
    
    # Drop largest RSSI value
    max_val = max(current_RSSI)
#    print('max val')
#    print(max_val)
    
    for n in range(len(current_RSSI)):
        if current_RSSI[n] == max_val:
            pass
        else:
            list_beacons.append(n)
            print('list beacons')
            print(list_beacons)
            beacon_vals.append(current_RSSI[n])
    print('beacon vals')
    print(beacon_vals)

    # Convert RSSI values to distances
    for n in range(len(beacon_vals)):
        print('beacon_vals in loop')
        print(beacon_vals[n])
        hold = 1.5/(1-((beacon_vals[n])/70.0))
        distance_vals.append(hold)
        
    print('distance vals')
    print(distance_vals)
    mat_1_1 = Beacons[list_beacons[0]][0]-Beacons[list_beacons[1]][0]
    mat_1_2 = Beacons[list_beacons[0]][1]-Beacons[list_beacons[2]][1]
    mat_1_3 = Beacons[list_beacons[1]][0]-Beacons[list_beacons[2]][0]
    mat_1_4 = Beacons[list_beacons[1]][1]-Beacons[list_beacons[2]][1]
    
    Mat_1 = np.matrix([[mat_1_1,mat_1_2],[mat_1_3,mat_1_4]])
    
    mat_2_1 = Beacons[list_beacons[0]][0]**2-Beacons[list_beacons[2]][0]**2 + Beacons[list_beacons[0]][1]**2-Beacons[list_beacons[2]][1]**2 - distance_vals[0]**2 + distance_vals[2]**2
    mat_2_2 = Beacons[list_beacons[1]][0]**2-Beacons[list_beacons[2]][0]**2 + Beacons[list_beacons[1]][1]**2-Beacons[list_beacons[2]][1]**2 - distance_vals[1]**2 + distance_vals[2]**2
    Mat_2 = np.matrix([[mat_2_1],[mat_2_2]])
    
    Current_Position = 2*np.linalg.inv(Mat_1)*Mat_2
    return Current_Position
    
# This function redraws the board to its default values
def ResetSurface():
    screen.fill(white)
    pygame.draw.lines(screen,black,False,outer_wall_points,3)
    pygame.draw.lines(screen,black,True,inner_wall_points,3)
    pygame.draw.lines(screen,black,False,lower_wall_points,3)
    pygame.draw.rect(screen,light_green,Rec_1,0)
    pygame.draw.rect(screen,light_blue,Rec_2,0)
    pygame.draw.rect(screen,light_green,Rec_3,0)
    pygame.draw.rect(screen,light_blue,Rec_4,0)
    pygame.draw.rect(screen,light_green,Rec_5,0)
    pygame.draw.rect(screen,light_blue,Rec_6,0)
    pygame.draw.rect(screen,light_green,Rec_8,0)
    pygame.draw.rect(screen,light_blue,Rec_9,0)
    pygame.draw.rect(screen,light_red,Rec_7,0)
    pygame.draw.rect(screen,light_red,Rec_11,0)
    pygame.draw.rect(screen,light_red,Rec_10,0)
    pygame.display.flip()
pygame.init()
screen = pygame.display.set_mode((600,800))
ResetSurface()
while True:
    
    # Get Current RSSI Value
    current_RSSI = GetRSSI()
    
    # Determine Bin
    current_bin = 0
    current_bin = GetBin()
    current_bin = int(current_bin)
    print('current bin')
    print(current_bin)
    
    # Highlight Current Bin
    if current_bin == 1.0:
        ResetSurface()
        pygame.draw.rect(screen,green,Rec_1,0)
    elif current_bin == 2.0:
        ResetSurface()
        pygame.draw.rect(screen,blue,Rec_2,0)
    elif current_bin == 3.0:
        ResetSurface()
        pygame.draw.rect(screen,green,Rec_3,0)
    elif current_bin == 4.0:
        ResetSurface()
        pygame.draw.rect(screen,blue,Rec_4,0)
    elif current_bin == 5.0:
        ResetSurface()
        pygame.draw.rect(screen,green,Rec_5,0)
    elif current_bin == 6:
        ResetSurface()
        pygame.draw.rect(screen,blue,Rec_6,0)
    elif current_bin == 8:
        ResetSurface()
        pygame.draw.rect(screen,green,Rec_8,0)
    elif current_bin == 9:
        ResetSurface()
        pygame.draw.rect(screen,blue,Rec_9,0)
    elif current_bin == 7:
        ResetSurface()
        pygame.draw.rect(screen,red,Rec_7,0)
    elif current_bin == 11:
        ResetSurface()
        pygame.draw.rect(screen,red,Rec_11,0)
    elif current_bin == 10:
        ResetSurface()
        pygame.draw.rect(screen,red,Rec_10,0)
        
    # Get Current Position
#    Current_Position = GetDistance()
#    print('Current_Position')
#    print(Current_Position)
    
    
    # Draw Dot
 #   pygame.draw.circle(screen,black,Current_Position,3,0)
    pygame.display.flip()
    pygame.image.save(screen,'img.jpg')
    sleep(1)
pygame.quit()