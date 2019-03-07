from enum import Enum
import RPi.GPIO as GPIO
import time

from doorlock import RDM6300

class BoardModel:

    def __init__(self, activityLedPin, pushButtonProgramPin, pushButtonCommandPin, lockRelayPin, lockRelayDelay, serialStr):
        self.activityLedPin = activityLedPin
        self.pushButtonProgramPin = pushButtonProgramPin
        self.pushButtonCommandPin = pushButtonCommandPin
        self.lockRelayPin = lockRelayPin
        self.lockRelayDelay = lockRelayDelay
        self.serialStr = serialStr

    def __del__(self):
        GPIO.cleanup()

    def setup(self):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.pushButtonCommandPin,GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(self.pushButtonProgramPin,GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(self.lockRelayPin,GPIO.OUT)
        GPIO.output(self.lockRelayPin,False)
        GPIO.setup(self.activityLedPin,GPIO.OUT)
        GPIO.output(self.activityLedPin,False)

        self.rfidReader =  RDM6300.RDM6300(self.activityLedPin, self.serialStr)

    def blinkActivityLed(self): 
        GPIO.output(self.activityLedPin, True)
        time.sleep(0.1)
        GPIO.output(self.activityLedPin, False)
    
    def isProgramButtonPushed(self):
        return not GPIO.input(self.pushButtonProgramPin)

    def isCommandButtonPushed(self):
        return not GPIO.input(self.pushButtonCommandPin)

    def openDoor(self):
        print("Opening the lock")
        GPIO.setup(self.lockRelayPin, GPIO.OUT)
        GPIO.output(self.lockRelayPin, 1)
        time.sleep(self.lockRelayDelay)
        GPIO.output(self.lockRelayPin, 0)

class BoardModels(Enum):
    V2 = BoardModel(24,25,17,23,3, '/dev/serial0')
    V3 = BoardModel(25,16,12,23,3,'/dev/serial0')
