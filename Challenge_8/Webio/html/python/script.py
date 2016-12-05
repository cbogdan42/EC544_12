import webiopi
from time import sleep
from webiopi.devices.serial import Serial
import RPi.GPIO as GPIO

State = 1
#ser = serial.Serial('/dev/ttyUSB0', 9600, timeout = 1)
serial = Serial("ttyUSB0", 9600)

STATE_PIN = 1           # This pin controls whether the robot is in manual(1) or automatic (0)
LEFT_RIGHT_PIN = 2      # This pin controls whether the robot turns right or left
FWD_BWD_PIN = 3         # This pin controls whether the robot goes forward or backward

# Setup pins
GPIO.setup(STATE_PIN,GPIO.OUT)
GPIO.setup(LEFT_RIGHT_PIN,GPIO.OUT)
GPIO.setup(FWD_BWD_PIN, GPIO.OUT)



def setup():
    global State
    
    # set default
    State = 0
    serial.writeString("0")
    print('Set Defaults')

def loop():
    global read_value
    read_value = serial.readString()    

@webiopi.macro
def MANUAL():
    GPIO.output(STATE_PIN,GPIO.LOW)

@webiopi.macro
def AUTONOMOUS():
    GPIO.output(STATE_PIN,GPIO.HIGH)

@webiopi.macro
def LEFT():
    GPIO.output(LEFT_RIGHT_PIN,GPIO.HIGH)

@webiopi.macro
def RIGHT():
    GPIO.output(LEFT_RIGHT_PIN,GPIO.LOW)

@webiopi.macro
def FORWARD():
    GPIO.output(FWD_BWD_PIN,GPIO.HIGH)

@webiopi.macro
def BACKWARD():
    GPIO.output(FWD_BWD_PIN,GPIO.LOW)
