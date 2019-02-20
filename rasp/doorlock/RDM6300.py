import time
import serial
import RPi.GPIO as GPIO
from doorlock.abstractSensor import AbstractSensor

class RDM6300(AbstractSensor):
	FLAG_BEGIN = '\x02'
	FLAG_END = '\x03'

	def __init__(self, activityPin, serialStr):
		#self.serial = serial.Serial('/dev/ttyAMA0',9600)
		self.serial = serial.Serial(serialStr,9600)
		self.serial.timeout = 0.5
		self.serial.close()
		self.serial.open()

		self.activityPin = activityPin

	def readFullUid(self):
		ID = ""
		read_byte = self.serial.read()
		if read_byte == b'\x02':
			GPIO.output(self.activityPin, True)
			for Counter in range(12):
				read_byte=self.serial.read()
				ID = ID + read_byte.decode("utf-8")
			self.serial.read(1000) #flush()
			GPIO.output(self.activityPin, False)
		return ID

	def readUid(self):
		ID = self.readFullUid()
		return ID[2:10]

def _test():
	#Test
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(23,GPIO.OUT)
	GPIO.setup(24,GPIO.OUT)
	GPIO.output(23,False)
	GPIO.output(24,False)
	test = RDM6300(24);
	while True:
		GPIO.output(24, True)
		time.sleep(0.01)
		GPIO.output(24, False)

		id = test.readUid();

		if id:
			GPIO.output(24, True)
			time.sleep(0.1)
			GPIO.output(24, False)
			print(id)

#__test()
