# -*- coding: utf8 -*-
 
import time
import RPi.GPIO as GPIO
import MFRC522
 
from controlador import checkar_credenciais

tipo_evento = "entrada"

try:
    # Inicia o módulo RC522.
    LeitorRFID = MFRC522.MFRC522()
 
    print('Aproxime seu cartão RFID')
 
    while True:
        # Verifica se existe uma tag próxima do módulo.
        status, tag_type = LeitorRFID.MFRC522_Request(LeitorRFID.PICC_REQIDL)
 
        if status == LeitorRFID.MI_OK:
            print('Cartão detectado!')
 
            # Efetua leitura do UID do cartão.
            status, uid = LeitorRFID.MFRC522_Anticoll()
 
            if status == LeitorRFID.MI_OK:
                uid = ':'.join(['%X' % x for x in uid])
                print('UID do cartão: %s' % uid)
 
                # Se o cartão está liberado exibe mensagem de boas vindas.
                checkar_credenciais(uid, tipo_evento)
 
                print('\nAproxime seu cartão RFID')
 
        time.sleep(.25)
except KeyboardInterrupt:
    # Se o usuário precionar Ctrl + C
    # encerra o programa.
    GPIO.cleanup()
    print('\nPrograma encerrado.')
