import os
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
        GPIO.setup(self.activityLedPin, GPIO.OUT)
        GPIO.output(self.activityLedPin, False)
        GPIO.setup(self.pushButtonProgramPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.setup(self.pushButtonCommandPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        if self.speakerPin>0:
            GPIO.setup(self.speakerPin, GPIO.OUT)
            GPIO.output(self.speakerPin, False)

        if self.doorSensorPin>0:
                GPIO.setup(self.doorSensorPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

        if self.lightSensorPin>0:
                GPIO.setup(self.lightSensorPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    
        self.rfidReader =  RDM6300.RDM6300(self.activityLedPin, self.serialStr)

    def blinkActivityLed(self):
        GPIO.output(self.activityLedPin, True)
        time.sleep(0.1)
        GPIO.output(self.activityLedPin, False)
        time.sleep(0.05)

    def isProgramButtonPushed(self):
        return not GPIO.input(self.pushButtonProgramPin)

    def isCommandButtonPushed(self):
        return not GPIO.input(self.pushButtonCommandPin)

    def isDoorOpened(self):
        if self.doorSensorPin>0:
            return GPIO.input(self.doorSensorPin)
        else:
            return False

    def isLightOn(self):
        if self.lightSensorPin>0:
            return not GPIO.input(self.lightSensorPin)
        else:
            return True

    def __execCommandButtonCallback(self, pinNumber):
        #Lucio 20190521: Avoid 'switch bounce'
        if time.time() - self.lastCommandButtonTime > 1:
           self.commandButtonCallback()
           self.lastCommandButtonTime = time.time()

    def setCommandButtonCallback(self, callbackFunction):
        self.commandButtonCallback = callbackFunction
        self.lastCommandButtonTime = time.time()
        if self.pushButtonCommandPin>0:
           GPIO.remove_event_detect(self.pushButtonCommandPin)
           GPIO.add_event_detect(self.pushButtonCommandPin, GPIO.FALLING, callback=self.__execCommandButtonCallback, bouncetime=500)

    def __execDoorSensorCallback(self, pinNumber):
        #Lucio 20190521: Avoid 'switch bounce'
        if time.time() - self.lastDoorSensorTime > 1:
           self.doorSensorCallback()
           self.lastCommandButtonTime = time.time()

    def setDoorSensorCallback(self, callbackFunction):
        self.doorSensorCallback = callbackFunction
        self.lastDoorSensorTime = time.time()
        GPIO.remove_event_detect(self.doorSensorPin)
        GPIO.add_event_detect(self.doorSensorPin, GPIO.BOTH, callback=self.__execDoorSensorCallback)

    def unlock(self):
        GPIO.output(self.lockRelayPin, 1)
        self.beepOk()

    def lock(self):
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
        
    def __init__(self, lockRelayPin, activityLedPin, pushButtonProgramPin, pushButtonCommandPin, speakerPin, doorSensorPin, lightSensorPin, serialStr):
        self.lockRelayPin = lockRelayPin
        self.activityLedPin = activityLedPin
        self.pushButtonProgramPin = pushButtonProgramPin
        self.pushButtonCommandPin = pushButtonCommandPin
        self.speakerPin = speakerPin
        self.doorSensorPin = doorSensorPin
        self.lightSensorPin = lightSensorPin
        self.serialStr = serialStr
        self.locked = True

        self.commandButtonCallback = None
        self.doorSensorCallback = None


class BoardModels(Enum):
    V1 = BoardModel(23,24,25,17,-1,-1,-1,'/dev/serial0')
    V2 = BoardModel(23,24,25,17,-1,-1,-1,'/dev/serial0')
    V3 = BoardModel(23,25,16,12,-1,-1,-1,'/dev/serial0')
    V4 = BoardModel(23,24,16,25,12,-1,-1,'/dev/serial0')
    V5 = BoardModel(24,23,12,25,18,16,-1,'/dev/serial0')
    V6 = BoardModel(21,16,18,23,12,25,24,'/dev/serial0')

