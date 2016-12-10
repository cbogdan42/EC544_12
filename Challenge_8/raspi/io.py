import RPi.GPIO as GPIO
import time
output_pin = 11

GPIO.setmode(GPIO.BOARD)
GPIO.setup(output_pin, GPIO.OUT)
GPIO.cleanup()
GPIO.setmode(GPIO.BOARD)
GPIO.setup(output_pin, GPIO.OUT)

a = 1
while a==1:
        print("Inside loop")
        GPIO.output(output_pin, GPIO.HIGH)
        time.sleep(1)
        print("After sleep")
        GPIO.output(output_pin, GPIO.LOW)
        time.sleep(1)