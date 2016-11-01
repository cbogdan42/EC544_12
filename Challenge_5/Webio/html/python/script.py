import webiopi
from time import sleep
from webiopi.devices.serial import Serial 

State = 1
#ser = serial.Serial('/dev/ttyUSB0', 9600, timeout = 1)
serial = Serial("ttyUSB0", 9600)

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
def OFF():
    global State

    State = 0
    for n in range(10):
        serial.writeString("0")
    print("Changed State to 0")
    check = serial.readString()

@webiopi.macro
def ON():
    global State

    State = 1
    for n in range(10):
        serial.writeString("1")
    print ("Changed State to 1")
    check = serial.readString()
