#ifndef APP_BOARD_MODELS
#define APP_BOARD_MODELS
#include "commons.cpp"

class BoardModel
{
private:
  bool locked = true;

protected:
  int lockRelayPin;
  int activityLedPn;
  int pushButtonProgramPin;
  int pushButtonCommandPin;
  int speakerPin;
  int doorSensorPin;
  int lightSensorPin;
  String serialStr;

public:
  BoardModel(
      int lockRelayPin,
      int activityLedPn,
      int pushButtonProgramPin,
      int pushButtonCommandPin,
      int speakerPin,
      int doorSensorPin,
      int lightSensorPin,
      String serialStr)
  {
    this->lockRelayPin = lockRelayPin;
    this->activityLedPn = activityLedPn;
    this->pushButtonProgramPin = pushButtonProgramPin;
    this->pushButtonCommandPin = pushButtonCommandPin;
    this->speakerPin = speakerPin;
    this->doorSensorPin = doorSensorPin;
    this->lightSensorPin = lightSensorPin;
    this->serialStr = serialStr;
  }

  void setup()
  {
    // TODO
  }

  void beep(double frequency, double delay)
  {
    if (this->speakerPin > 0)
    {
      // TODO
    }
  }

  void beepNotOk()
  {
    if (this->speakerPin > 0)
    {
      this->beep(440, 0.05);
      delay(50);
      this->beep(440, 0.05);
    }
  }

  void beepOk()
  {
    if (this->speakerPin > 0)
      this->beep(2000, 0.1);
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
    // TODO
  }

  void toggleLocked() { this->locked = !this->locked; }

  bool isLocked() { return this->locked; }
  bool isLightOn()
  {
    // TODO
    return true;
  }
  bool isDoorOpened()
  {
    // TODO
    return true;
  }
  bool isCommandButtonPushed()
  {
    // TODO
    return true;
  }
  bool isProgramButtonPushed()
  {
    // TODO
    return true;
  }
};

BoardModel *getBoardModel(int version)
{
  // TODO
  BoardModel *board = new BoardModel(23, 24, 25, 17, -1, -1, -1, "/dev/serial0");
  board->setup();
  return board;
}

#endif