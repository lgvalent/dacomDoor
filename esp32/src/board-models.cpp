#ifndef BOARD_MODELS
#define BOARD_MODELS
#include "commons.cpp"

class GPIO
{
public:
  static void setup(uint8_t pin, uint8_t mode);
  static void output(uint8_t pin, uint8_t val);
  static void turnOn(uint8_t pin);
  static void turnOff(uint8_t pin);
  static int input(uint8_t pin);
};

inline void GPIO::setup(uint8_t pin, uint8_t mode) { pinMode(pin, mode); }
inline void GPIO::output(uint8_t pin, uint8_t val) { digitalWrite(pin, val); }
inline void GPIO::turnOff(uint8_t pin) { digitalWrite(pin, LOW); }
inline void GPIO::turnOn(uint8_t pin) { digitalWrite(pin, HIGH); }
inline int GPIO::input(uint8_t pin) { return digitalRead(pin); }

class BoardModel
{
private:
  time_t lastCommandButtonTime = 0;
  bool locked = true;

public:
  static const uint8_t DISABLED_PIN = 0; 
  const uint8_t lockRelayPin;
  const uint8_t activityLedPin;
  const uint8_t pushButtonProgramPin;
  const uint8_t pushButtonCommandPin;
  const uint8_t speakerPin;
  const uint8_t doorSensorPin;
  const uint8_t lightSensorPin;

  BoardModel(
      uint8_t lockRelayPin,
      uint8_t activityLedPin,
      uint8_t pushButtonProgramPin,
      uint8_t pushButtonCommandPin,
      uint8_t speakerPin,
      uint8_t doorSensorPin,
      uint8_t lightSensorPin) : lockRelayPin(lockRelayPin),
                            activityLedPin(activityLedPin),
                            pushButtonProgramPin(pushButtonProgramPin),
                            pushButtonCommandPin(pushButtonCommandPin),
                            speakerPin(speakerPin),
                            doorSensorPin(doorSensorPin),
                            lightSensorPin(lightSensorPin) {}

  void startup()
  {
    GPIO::setup(this->lockRelayPin, OUTPUT);
    GPIO::turnOff(this->lockRelayPin);

    GPIO::setup(this->activityLedPin, OUTPUT);
    GPIO::turnOff(this->activityLedPin);
    if (this->activityLedPin == 2)
    {
      uint8_t channel = 1;
      uint32_t frequency = 1000;
      uint8_t resolution = 10;
      ledcAttachPin(this->activityLedPin, channel);
      ledcSetup(channel, frequency, resolution);
    }

    GPIO::setup(this->pushButtonProgramPin, INPUT_PULLUP);
    GPIO::setup(this->pushButtonCommandPin, INPUT_PULLUP);

    if (this->speakerPin)
      GPIO::setup(this->speakerPin, OUTPUT);

    if (this->doorSensorPin)
      GPIO::setup(this->doorSensorPin, INPUT_PULLUP);

    if (this->lightSensorPin)
      GPIO::setup(this->lightSensorPin, INPUT_PULLUP);
  }

  void setCommandButtonCallback(void (*cb)())
  {
    uint8_t pin = this->pushButtonCommandPin;
    if (pin > 0) 
    {
      detachInterrupt(pin);
      attachInterrupt(pin, cb, HIGH);
    }
  }

  void serDoorSensorCallback(void (*cb)())
  {
    uint8_t pin = this->doorSensorPin;
    if (pin > 0) 
    {
      detachInterrupt(pin);
      attachInterrupt(pin, cb, HIGH);
    }
  }

  void beep(double frequency, uint32_t delay_time)
  {
    uint8_t channel = this->speakerPin;
    uint8_t resolution = 8;
    if (channel > 0)
    {
      ledcSetup(channel, frequency, resolution);
      ledcWriteNote(channel, NOTE_Cs, frequency);
      delay(delay_time);
    }
  }

  void beepNotOk()
  {
    if (this->speakerPin > 0)
    {
      this->beep(440, 50);
      delay(50);
      this->beep(440, 50);
    }
  }

  void beepOk()
  {
    if (this->speakerPin > 0)
    {
      this->beep(2000, 100);
    }
  }

  void lock()
  {
    GPIO::turnOn(this->lockRelayPin);
    this->beepOk();
  }

  void unlock()
  {
    GPIO::turnOff(this->lockRelayPin);
    this->beepOk();
  }

  void blinkActivityLed()
  {
    if (this->activityLedPin == 2)
    {
      uint8_t channel = 1;
      ledcWrite(channel, 1000);
      delay(100);
      ledcWrite(channel, 0);
      delay(50);
    }
    else
    {
      GPIO::turnOn(this->activityLedPin);
      delay(100);
      GPIO::turnOff(this->activityLedPin);
      delay(50);
    }
  }

  void toggleLocked() { this->locked = !this->locked; }

  bool isLocked() { return this->locked; }
  bool isLightOn() { return (this->lightSensorPin > 0) && GPIO::input(this->lightSensorPin); }
  bool isDoorOpened() { return (this->doorSensorPin > 0) && GPIO::input(this->doorSensorPin); }
  bool isCommandButtonPushed() { return !GPIO::input(this->pushButtonCommandPin); }
  bool isProgramButtonPushed() { return !GPIO::input(this->pushButtonProgramPin); }
};

static BoardModel *getBoardModel(byte version)
{
    BoardModel* boardModel;
    switch(version){
      case 1 : boardModel = new BoardModel(23,24,25,17, 0, 0, 0);break;
      case 2 : boardModel = new BoardModel(23,24,25,17, 0, 0, 0);break;
      case 3 : boardModel = new BoardModel(23,25,16,12, 0, 0, 0);break;
      case 4 : boardModel = new BoardModel(23,24,16,25,12, 0, 0);break;
      case 5 : boardModel = new BoardModel(24,23,12,25,18,16, 0);break;
      case 6 : boardModel = new BoardModel(21,16,18,23,12,25,24);break;
      default: throw "Version not suportted yet!";
    }

  boardModel->startup();
  return boardModel;
}

#endif