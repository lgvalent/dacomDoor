# -*- coding: utf8 -*-
 
import time
import RPi.GPIO as GPIO
from doorlock import MFRC522
 
from doorlock.controller import checkUid, checkSchedule
from app.models.enums import EventTypesEnum

try:
    # Inicia o módulo RC522.
    rfidReader = MFRC522.MFRC522()
 
    print('Bring RFID card closer...')
 
    while True:
        # Verifica se existe uma tag próxima do módulo.
        status, tag_type = rfidReader.MFRC522_Request(rfidReader.PICC_REQIDL)
 
        if status == rfidReader.MI_OK:
            print ('Card detected!')
 
            # Efetua leitura do UID do cartão.
            status, uid = rfidReader.MFRC522_Anticoll()
 
            if status == rfidReader.MI_OK:
                uid = ':'.join(['%X' % x for x in uid])
                print('UID: %s' % uid)
 
                # Se o cartão está liberado exibe mensagem de boas vindas.
                checkUid(uid, EventTypesEnum.IN)
                
                print("Ready...")
            #else:
                #print("1111111")
        else:
            #print("2222222")
            time.sleep(.25)

except KeyboardInterrupt:
    # Se o usuário precionar Ctrl + C
    # encerra o programa.
    GPIO.cleanup()
    print('\nEnd of program.')
