# -*- coding: utf-8 -*-
"""
Created on Mon Oct 31 20:04:19 2016

@author: cbogdan
"""

import pygame
import time
import random
import serial

xbee = serial.Serial('/dev/ttyUSB0',9600)
 
pygame.init()
 
display_width = 1000
display_height = 600
 
black = (0,0,0)
white = (255,255,255)
red = (200,0,0)
green = (0,200,0)

bright_red = (255,0,0)
bright_green = (0,255,0)
block_color = (53,115,255)
 
 
gameDisplay = pygame.display.set_mode((display_width,display_height))
pygame.display.set_caption('EC544_Group12')
clock = pygame.time.Clock()
 
 
 
 

def text_objects(text, font):
    textSurface = font.render(text, True, black)
    return textSurface, textSurface.get_rect()
 
def message_display(text):
    largeText = pygame.font.Font('freesansbold.ttf',115)
    TextSurf, TextRect = text_objects(text, largeText)
    TextRect.center = ((display_width/2),(display_height/2))
    gameDisplay.blit(TextSurf, TextRect)
 
    pygame.display.update()
 
    time.sleep(2)
 
    game_loop()
    
    


def button(msg,x,y,w,h,ic,ac,action=None):
    mouse = pygame.mouse.get_pos()
    click = pygame.mouse.get_pressed()
    print(mouse)
    if x+w > mouse[0] > x and y+h > mouse[1] > y:
        pygame.draw.rect(gameDisplay, ac,(x,y,w,h))

        if click[0] == 1 and action != None:
            action()         
    else:
        pygame.draw.rect(gameDisplay, ic,(x,y,w,h))

    smallText = pygame.font.SysFont("comicsansms",20)
    textSurf, textRect = text_objects(msg, smallText)
    textRect.center = ( (x+(w/2)), (y+(h/2)) )
    gameDisplay.blit(textSurf, textRect)
def turn_on():
    print("turn on the car")
    for n in range(10):
        xbee.write("1")
        
def turn_off():
    print("turn off the car")
    for n in range(10):
        xbee.write("0")

def game_intro():

    intro = True

    while intro:
        for event in pygame.event.get():
            #print(event)
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
                
        gameDisplay.fill(white)
        largeText = pygame.font.SysFont("comicsansms",115)
        TextSurf, TextRect = text_objects("EC544_Group12", largeText)
        TextRect.center = ((display_width/2),(display_height/2))
        gameDisplay.blit(TextSurf, TextRect)

        button("ON",150,450,100,50,green,bright_green,turn_on)
        button("OFF",550,450,100,50,red,bright_red,turn_off)

        pygame.display.update()
        clock.tick(15)        
        
    
    

    


game_intro()
pygame.quit()
quit()