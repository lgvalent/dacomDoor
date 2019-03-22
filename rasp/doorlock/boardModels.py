import pygame
from enum import Enum
import RPi.GPIO as GPIO
import time

from doorlock import RDM6300

class BoardModel:

    def __del__(self):
        GPIO.cleanup()

    def setup(self):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.lockRelayPin,GPIO.OUT)
        GPIO.output(self.lockRelayPin,False)
        GPIO.setup(self.activityLedPin,GPIO.OUT)
        GPIO.output(self.activityLedPin,False)
        GPIO.setup(self.pushButtonProgramPin,GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(self.pushButtonCommandPin,GPIO.IN, pull_up_down=GPIO.PUD_UP)
        if self.speakerPin>0:
            GPIO.setup(self.speakerPin,GPIO.OUT)
            GPIO.output(self.speakerPin,False)

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
        self.beepOk()


    def beepOk(self):
        if self.speakerPin>0:
            self.beep(2000, 0.1)
        else:
            pygame.init()
            pygame.mixer.music.load(os.path.dirname(__file__) + "/beep.wav")
            pygame.mixer.music.play()

    def beepNoOk(self):
        if self.speakerPin>0:
            self.beep(440, 0.05)
            time.sleep(0.05)
            self.beep(440, 0.05)
        else:
            pygame.init()
            pygame.mixer.music.load(os.path.dirname(__file__) + "/beeps.wav")
            pygame.mixer.music.play()

    def beep(self, frequency, delay):
        if self.speakerPin>0:
            speakerPwm = GPIO.PWM(self.speakerPin, frequency)
            speakerPwm.start(10)
            time.sleep(delay)
            speakerPwm.stop()

    def __init__(self, lockRelayPin, lockRelayDelay, activityLedPin, pushButtonProgramPin, pushButtonCommandPin, speakerPin, serialStr):
        self.lockRelayPin = lockRelayPin
        self.lockRelayDelay = lockRelayDelay
        self.activityLedPin = activityLedPin
        self.pushButtonProgramPin = pushButtonProgramPin
        self.pushButtonCommandPin = pushButtonCommandPin
        self.speakerPin = speakerPin
        self.serialStr = serialStr

class BoardModels(Enum):
    V1 = BoardModel(23,3,24,25,17,-1, '/dev/serial0')
    V2 = BoardModel(23,3,24,25,17,-1, '/dev/serial0')
    V3 = BoardModel(23,.1,25,16,12,-1,'/dev/serial0')
    V4 = BoardModel(23,.1,24,16,25,12,'/dev/serial0')
