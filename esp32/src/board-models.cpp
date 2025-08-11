#ifndef BOARD_MODELS
#define BOARD_MODELS

#include <Arduino.h>


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
  bool locked = true;

  static const uint8_t LEDC_CHANNEL_FOR_SPEAKER = 0;

public:
  static const uint8_t DISABLED_PIN = 0; 
  const uint8_t lockRelayPin;
  const uint8_t activityLedPin;
  const uint8_t pushButtonProgramPin;
  const uint8_t pushButtonCommandPin;
  const uint8_t speakerPin;
  const uint8_t doorSensorPin;
  const uint8_t lightSensorPin;
  const uint8_t readerRxPin;

  BoardModel(
      uint8_t lockRelayPin,
      uint8_t activityLedPin,
      uint8_t pushButtonProgramPin,
      uint8_t pushButtonCommandPin,
      uint8_t speakerPin,
      uint8_t doorSensorPin,
      uint8_t lightSensorPin,
      uint8_t readerRxPin) : lockRelayPin(lockRelayPin),
                            activityLedPin(activityLedPin),
                            pushButtonProgramPin(pushButtonProgramPin),
                            pushButtonCommandPin(pushButtonCommandPin),
                            speakerPin(speakerPin),
                            doorSensorPin(doorSensorPin),
                            lightSensorPin(lightSensorPin),
                            readerRxPin(readerRxPin) {}

  void startup()
  {
    // gpio_install_isr_service(0);

    GPIO::setup(this->lockRelayPin, OUTPUT);
    GPIO::turnOff(this->lockRelayPin);

    GPIO::setup(this->activityLedPin, OUTPUT);
    GPIO::turnOff(this->activityLedPin);

    GPIO::setup(this->pushButtonProgramPin, INPUT_PULLUP);
    GPIO::setup(this->pushButtonCommandPin, INPUT_PULLUP);

    if (this->speakerPin){
      GPIO::setup(this->speakerPin, OUTPUT);
      ledcSetup(LEDC_CHANNEL_FOR_SPEAKER, 1000, 8); // 1000 Hz frequency, 8 bits resolution
	    ledcAttachPin(this->speakerPin, LEDC_CHANNEL_FOR_SPEAKER); // attach the LED_BUILTIN pin to the channel
    }

    if (this->doorSensorPin)
      GPIO::setup(this->doorSensorPin, INPUT_PULLUP);

    if (this->lightSensorPin)
      GPIO::setup(this->lightSensorPin, INPUT_PULLUP);

    if (this->readerRxPin)
      GPIO::setup(this->readerRxPin, INPUT);
  }

  void setCommandButtonCallback(void (*cb)())
  {
    if (this->pushButtonCommandPin > 0) 
    {
      detachInterrupt(this->pushButtonCommandPin);
      attachInterrupt(this->pushButtonCommandPin, cb, HIGH);
    }
  }

  void serDoorSensorCallback(void (*cb)())
  {
    if (doorSensorPin > 0) 
    {
      detachInterrupt(doorSensorPin);
      attachInterrupt(doorSensorPin, cb, HIGH);
    }
  }

  /**
   * Emits a beep sound with the specified frequency and duration.
   * This function uses the LEDC (LED Control) peripheral to generate a PWM signal
   * that can be used to drive a speaker or buzzer.
   * 
   * While the function is active, it turns on the activity LED to indicate that a sound is being emitted.
   *
   * @param   double  delay_time  [delay_time description]
   *
   * @return  void                [return description]
   */
  void beep(u_int32_t frequency, uint32_t delay_time)
  {
    this->turnOnActivityLed();
    if (this->speakerPin > 0)
    {
      ledcWriteTone(LEDC_CHANNEL_FOR_SPEAKER, frequency);
      delay(delay_time);
      ledcWriteTone(LEDC_CHANNEL_FOR_SPEAKER, 0);
      delay(delay_time);
    }
    this->turnOffActivityLed();
  }

  void beepNotOk()
  {
    if (this->speakerPin > 0)
    {
      this->beep(440, 100);
      delay(50);
      this->beep(440, 100);
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
    GPIO::turnOff(this->lockRelayPin);
    this->locked = true;
    this->beepOk();
  }

  void unlock()
  {
    GPIO::turnOn(this->lockRelayPin);
    this->locked = false;
    this->beepOk();
 }

  void blinkActivityLed()
  {
      this->turnOnActivityLed();
      delay(30);
      this->turnOffActivityLed();
      delay(70);    
  }

  void turnOnActivityLed()
  {
    GPIO::turnOn(this->activityLedPin);
  }

  void turnOffActivityLed()
  {
    GPIO::turnOff(this->activityLedPin);
  }

  bool isLocked() { return this->locked; }
  bool isLightOn() { return (this->lightSensorPin > 0) && !GPIO::input(this->lightSensorPin); }
  bool isDoorOpened() { return (this->doorSensorPin > 0) && GPIO::input(this->doorSensorPin); }
  bool isCommandButtonPushed() { return !GPIO::input(this->pushButtonCommandPin); }
  bool isProgramButtonPushed() { return !GPIO::input(this->pushButtonProgramPin); }
  
  static BoardModel *getBoardModel(byte version)
  {
    BoardModel* boardModel;
    switch(version){
      case 1 : boardModel = new BoardModel(13, 2,18, 4,15,17, 5,16);break;
      case 2 : boardModel = new BoardModel( 5, 0, 6, 2, 1, 3, 4,20);break;
      default: throw "Version not suportted yet!";
    }
    boardModel->startup();
    return boardModel;
  }
};

#endif