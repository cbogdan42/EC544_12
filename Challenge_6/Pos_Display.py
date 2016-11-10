# -*- coding: utf-8 -*-
"""
Created on Thu Nov 10 10:00:38 2016

@author: cbogdan
"""

import sys, pygame
from time import sleep

pygame.init()
screen = pygame.display.set_mode((600,800))
position=[30,30]
white=[255,255,255]
black = [0,0,0]
light_red = [239,129,129]
light_blue= [0,128,255]
light_green = [0,204,0]
screen.fill(white)

# Setup Background Map - 1 pixel = 5 cm
outer_wall_points = [(20,780),(20,20),(580,20),(580,780)]
inner_wall_points = [(100,100),(500,100),(500,500),(350,500),(350,400),(250,400),(250,500),(100,500)]
lower_wall_points = [(100,780),(100,580),(500,580),(500,780)]
pygame.draw.lines(screen,black,False,outer_wall_points,3)
pygame.draw.lines(screen,black,True,inner_wall_points,3)
pygame.draw.lines(screen,black,False,lower_wall_points,3)

# Rectangle Areas
Rec_1 = (22,22,78,78)
Rec_2 = (100,22,401,77)
Rec_3 = (501,22,78,78)
Rec_4 = (502,100,77,401)
Rec_5 = (501,501,78,79)
Rec_6 = (99,502,402,77)
Rec_7 = (22,502,78,78)
Rec_8 = (22,100,77,402)
Rec_9 = (251,401,99,101)
Rec_10 = (22,580,78,200)
Rec_11 = (501,580,78,200)

pygame.draw.rect(screen,light_green,Rec_1,0)
pygame.draw.rect(screen,light_blue,Rec_2,0)
pygame.draw.rect(screen,light_green,Rec_3,0)
pygame.draw.rect(screen,light_blue,Rec_4,0)
pygame.draw.rect(screen,light_green,Rec_5,0)
pygame.draw.rect(screen,light_blue,Rec_6,0)
pygame.draw.rect(screen,light_green,Rec_7,0)
pygame.draw.rect(screen,light_blue,Rec_8,0)
pygame.draw.rect(screen,light_red,Rec_9,0)
pygame.draw.rect(screen,light_red,Rec_10,0)
pygame.draw.rect(screen,light_red,Rec_11,0)
pygame.display.flip()


while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            loop=False
        if event.type==pygame.KEYDOWN:
            position[0]+=20

    pygame.draw.rect(screen,light_green,Rec_1,0)
    pygame.draw.rect(screen,light_blue,Rec_2,0)
    pygame.draw.rect(screen,light_green,Rec_3,0)
    pygame.draw.rect(screen,light_blue,Rec_4,0)
    pygame.draw.rect(screen,light_green,Rec_5,0)
    pygame.draw.rect(screen,light_blue,Rec_6,0)
    pygame.draw.rect(screen,light_green,Rec_7,0)
    pygame.draw.rect(screen,light_blue,Rec_8,0)
    pygame.draw.rect(screen,light_red,Rec_9,0)
    pygame.draw.rect(screen,light_red,Rec_10,0)
    pygame.draw.rect(screen,light_red,Rec_11,0)
    pygame.draw.circle(screen,black,position,5,0)
    pygame.display.flip()

sleep(10)
pygame.quit()