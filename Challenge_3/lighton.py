import RPi.GPIO as GPIO
from time import sleep

GPIO.setmode(GPIO.BOARD)
GPIO.setup(12, GPIO.OUT)
GPIO.output(12,1)
sleep(1)
GPIO.output(12,0)
sleep(1)
GPIO.cleanup()
