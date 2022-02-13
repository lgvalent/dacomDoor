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
  static int read(uint8_t pin);
};

inline void GPIO::setup(uint8_t pin, uint8_t mode) { pinMode(pin, mode); }
inline void GPIO::output(uint8_t pin, uint8_t val) { digitalWrite(pin, val); }
inline void GPIO::turnOff(uint8_t pin) { digitalWrite(pin, LOW); }
inline void GPIO::turnOn(uint8_t pin) { digitalWrite(pin, HIGH); }
inline int GPIO::read(uint8_t pin) { return digitalRead(pin); }

class BoardPins
{
public:
  const int lockRelayPin;
  const int activityLedPin;
  const int pushButtonProgramPin;
  const int pushButtonCommandPin;
  const int speakerPin;
  const int doorSensorPin;
  const int lightSensorPin;

  BoardPins(
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
                            lightSensorPin(lightSensorPin)
  {
  }

  void startup()
  {
    GPIO::setup(this->lockRelayPin, OUTPUT);
    GPIO::turnOff(this->lockRelayPin);

    GPIO::setup(this->activityLedPin, OUTPUT);
    GPIO::turnOff(this->activityLedPin);

    GPIO::setup(this->pushButtonProgramPin, INPUT_PULLUP);
    GPIO::setup(this->pushButtonCommandPin, INPUT_PULLUP);

    if (this->speakerPin)
      GPIO::setup(this->speakerPin, OUTPUT);

    if (this->doorSensorPin > 0)
      GPIO::setup(this->doorSensorPin, INPUT_PULLUP);

    if (this->lightSensorPin > 0)
      GPIO::setup(this->lightSensorPin, INPUT_PULLUP);
  }
};

class IBoardModel
{
private:
  time_t lastCommandButtonTime = 0;
  bool locked = true;

public:
  BoardPins *setup;

  IBoardModel(BoardPins *setup) : setup(setup) {}

  void beep(double frequency, double delay)
  {
    if (this->setup->speakerPin > 0)
    {
      // TODO
    }
  }

  void beepNotOk()
  {
    if (this->setup->speakerPin > 0)
    {
      this->beep(440, 0.05);
      delay(50);
      this->beep(440, 0.05);
    }
  }

  void beepOk()
  {
    if (this->setup->speakerPin > 0)
    {
      this->beep(2000, 0.1);
    }
  }

  void lock()
  {
    // TODO
  }

  void unlock()
  {
    // TODO
  }

  void blinkActivityLed()
  {
    GPIO::turnOn(this->setup->activityLedPin);
    delay(100);
    GPIO::turnOff(this->setup->activityLedPin);
    delay(50);
  }

  void toggleLocked() { this->locked = !this->locked; }

  bool isLocked() { return this->locked; }
  bool isLightOn()
  {
    if (this->setup->lightSensorPin > 0)
      return GPIO::read(this->setup->lightSensorPin);
    else
      return false;
  }
  bool isDoorOpened()
  {
    if (this->setup->doorSensorPin > 0)
      return GPIO::read(this->setup->doorSensorPin);
    else
      return false;
  }
  bool isCommandButtonPushed() { return !GPIO::read(this->setup->pushButtonCommandPin); }
  bool isProgramButtonPushed() { return !GPIO::read(this->setup->pushButtonProgramPin); }

  virtual void onCommandButtonPress() = 0;
  virtual void onDoorSensorActive() = 0;
};

static BoardPins *getBoardPins(int version)
{
  // TODO
  BoardPins *setup = new BoardPins(23, 24, 25, 17, -1, -1, -1);
  setup->startup();
  return setup;
}

#endif