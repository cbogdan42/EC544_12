#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Nov  7 13:11:58 2016

@author: Ritika
"""
import math
import operator
import numpy
from collections import Counter

data = []
rssi = []
k = 8

###To open data.txt database file and read the rssi values as floats in an array
with open('data.txt') as file:
    for line in file:
        rssi.append([float(n) for n in line.strip().split(',')])
        
###Find Euclidean distance between the vector elements in the dataset
distance=[]
for x in range(len(rssi)-1): #make this len(rssi) for actual test
    temp=[]
    for y in range(4):
        temp.append(math.sqrt((pow((rssi[x][y]-rssi[x+1][y]),2))))
        #temp.append(math.sqrt((pow((rssi[x][y]-test[y]),2))))
    distance.append(sum(temp))
dist = numpy.array(distance)
sortedind = numpy.argsort(dist)
distance.sort()
print (distance)
print (sortedind[:3]) #Returns the index of the k closest neighbors
#use [:k]

position = []
#assign position to crawler based on majority vote of k neighbors
for x in range(len(sortedind)):
    position.append(rssi[sortedind[x]][4])
print (position)

val = Counter(position)
print(val.most_common(1)[0][0])
