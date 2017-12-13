import RPi.GPIO as GPIO
import time     

pino = 12
segundos = 5

def abrir():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(pino, GPIO.OUT)
    GPIO.output(pino, 1)
    time.sleep(segundos)
    GPIO.output(pino, 0)