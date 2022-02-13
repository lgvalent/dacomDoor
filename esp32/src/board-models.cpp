#ifndef APP_BOARD_MODELS
#define APP_BOARD_MODELS
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
  const int lockRelayPin;
  const int activityLedPin;
  const int pushButtonProgramPin;
  const int pushButtonCommandPin;
  const int speakerPin;
  const int doorSensorPin;
  const int lightSensorPin;

  BoardModel(
      int lockRelayPin,
      int activityLedPin,
      int pushButtonProgramPin,
      int pushButtonCommandPin,
      int speakerPin,
      int doorSensorPin,
      int lightSensorPin) : lockRelayPin(lockRelayPin),
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
      int channel = 1;
      int frequency = 1000;
      int resolution = 10;
      ledcAttachPin(this->activityLedPin, channel);
      ledcSetup(channel, frequency, resolution);
    }

    GPIO::setup(this->pushButtonProgramPin, INPUT_PULLUP);
    GPIO::setup(this->pushButtonCommandPin, INPUT_PULLUP);

    if (this->speakerPin)
      GPIO::setup(this->speakerPin, OUTPUT);

    if (this->doorSensorPin > 0)
      GPIO::setup(this->doorSensorPin, INPUT_PULLUP);

    if (this->lightSensorPin > 0)
      GPIO::setup(this->lightSensorPin, INPUT_PULLUP);
  }

  void setCommandButtonCallback(void (*cb)())
  {
    int pin = this->pushButtonCommandPin;
    if (pin > 0) 
    {
      detachInterrupt(pin);
      attachInterrupt(pin, cb, HIGH);
    }
  }

  void serDoorSensorCallback(void (*cb)())
  {
    int pin = this->doorSensorPin;
    if (pin > 0) 
    {
      detachInterrupt(pin);
      attachInterrupt(pin, cb, HIGH);
    }
  }

  void beep(double frequency, uint32_t delay_time)
  {
    int channel = this->speakerPin;
    int resolution = 8;
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
      int channel = 1;
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

static BoardModel *getBoardModel(int version)
{
  int disabled = -1;
  int internalActivityLedPin = 2;
  int fakeButtonPin = 23;
  BoardModel *boardModel = new BoardModel(
      10,                     // OUTPUT         lockRelayPin
      internalActivityLedPin, // OUTPUT         activityLedPin
      fakeButtonPin,          // INPUT_PULLUP   pushButtonProgramPin
      25,                     // INPUT_PULLUP   pushButtonCommandPin
      disabled,               // OUTPUT       ? speakerPin
      disabled,               // INPUT_PULLUP ? doorSensorPin
      disabled                // INPUT_PULLUP ? lightSensorPin
  );
  boardModel->startup();
  return boardModel;
}

#endif