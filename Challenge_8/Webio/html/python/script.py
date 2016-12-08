import webiopi
from time import sleep
#from webiopi.devices.serial import Serial
import RPi.GPIO as GPIO

State = 1
#ser = serial.Serial('/dev/ttyUSB0', 9600, timeout = 1)
#serial = Serial("ttyUSB0", 9600)

# Set GPIO numbering scheme
GPIO.setmode(GPIO.BOARD)

# Declare useful pins
LPIN = 1		# Left Pin
RPIN = 2		# Right Pin
FPIN = 3		# Forward Pin
BPIN = 4		# Backward Pin
MPIN = 5		# Mode Pin
ASPIN = 6		# Automatic Start Pin when in Automatic Mode

# Setup pins
GPIO.setup(LPIN, GPIO.OUT)
GPIO.setup(RPIN, GPIO.OUT)
GPIO.setup(FPIN, GPIO.OUT)
GPIO.setup(BPIN, GPIO.OUT)
GPIO.setup(MPIN, GPIO.OUT)
GPIO.setup(ASPIN, GPIO.OUT)


def setup():
    global State
    
    # set default
    State = 0
    #serial.writeString("0")
    print('Set Defaults')

def loop():
    global read_value
    #read_value = serial.readString()    

@webiopi.macro
def MANUAL():
    # Set Mode pin to Low
    GPIO.output(MPIN,GPIO.LOW)

@webiopi.macro
def AUTONOMOUS():
    # Set Mode pin to High
    GPIO.output(MPIN,GPIO.HIGH)

@webiopi.macro
def LEFT():
    # Set LPIN HIGH
    GPIO.output(LPIN,GPIO.HIGH)
    # Set RPIN LOW
    GPIO.output(RPIN,GPIO.LOW)

@webiopi.macro
def RIGHT():
    # Set RPIN HIGH
    GPIO.output(RPIN,GPIO.HIGH)
    # Set LPIN HIGH
    GPIO.output(LPIN,GPIO.LOW)
    
@webiopi.macro
def FORWARD():
    # Set FPIN HIGH
    GPIO.output(FPIN,GPIO.HIGH)
    # Set BPIN LOW
    GPIO.output(RPIN,GPIO.LOW)

@webiopi.macro
def BACKWARD():
    # Set BPIN HIGH
    GPIO.output(BPIN,GPIO.HIGH)
    # Set FPIN LOW
    GPIO.output(FPIN,GPIO.LOW)
