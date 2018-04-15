# -*- coding: utf8 -*-
 
import time
import RPi.GPIO as GPIO
from doorlock import RDM6300
 
from doorlock.controller import beep, beeps, learnUid, checkUid, checkSchedule
from app.models.enums import EventTypesEnum

ACTIVITY_LED_PIN = 24
PUSH_BUTTON_PIN = 17

GPIO.setmode(GPIO.BCM)
GPIO.setup(PUSH_BUTTON_PIN,GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(ACTIVITY_LED_PIN,GPIO.OUT)
GPIO.output(ACTIVITY_LED_PIN,False)

def blinkActivityLed():
    GPIO.output(ACTIVITY_LED_PIN, True)
    time.sleep(0.01)
    GPIO.output(ACTIVITY_LED_PIN, False)
    time.sleep(0.01)

try:
    # Inicia o módulo
    rfidReader = RDM6300.RDM6300(ACTIVITY_LED_PIN)
 
    print('Bring RFID card closer...')
    while True:
        learnMode = not GPIO.input(PUSH_BUTTON_PIN)
        if learnMode:
            beep(); blinkActivityLed(); blinkActivityLed(); blinkActivityLed()

        blinkActivityLed()
        # Verifica se existe uma tag próxima do módulo.
        uid = rfidReader.readUid()

        if uid:
            print('UID: %s' % uid)
            if learnMode:
                print("Learning %s..." % uid)
                if learnUid(uid):
                    print("Learned")
                    beep()
                else:
                    print("Repeated.")
                    beeps()
            else:
                # Se o cartão está liberado exibe mensagem de boas vindas.
                if checkUid(uid, EventTypesEnum.IN):
                    beep()
                else:
                    beeps()
            print("Ready...")
        else:
            time.sleep(.25)

except KeyboardInterrupt:
    # Se o usuário precionar Ctrl + C
    # encerra o programa.
    GPIO.cleanup()
    print('\nEnd of program.')

