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
    serial.writeString("0")
    print("Changed State to 0")
    check = serial.readString()

@webiopi.macro
def ON():
    global State

    State = 1
    serial.writeString("1")
    print ("Changed State to 1")
    check = serial.readString()

@webiopi.macro
def RAVE():
    global State

    State = 2
    serial.writeString("2")
    print ('Changed State to 2')
    check = serial.readString()

@webiopi.macro
def Get_Value():
    check = serial.readString()
    html_str = '<html> <font color="white"> The Current State is' + check + ' </font></html>'
    html_file = open("/home/pi/Challenge_3/Webio/html/python/state.html","w")
    html_file.write(html_str)
    html_file.close()
