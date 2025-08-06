#ifndef READER_RFID
#define READER_RFID
/**
 * 1617728 bytes <-RDM6300.h
 * 1622752 bytes <- plerup/EspSoftwareSerial @ ^8.2.0
 */
#include <rdm6300.h>

#include "board-models.cpp"
#include "models.cpp"

class Reader
{
private:
  BoardModel *boardModel;
  Rdm6300 rdm6300; 

public:
  Reader(BoardModel *boardModel): boardModel(boardModel) {}
  void startup()
  {
    Serial.printf("[READ] Starting RDM6300 at %d...\n", boardModel->readerRxPin);
    this->rdm6300.begin(boardModel->readerRxPin);
  }

  Uid readUid()
  {
    if (!this->rdm6300.get_new_tag_id()) 
      return UID_NULL; // No new tag detected
    else
      Serial.println(F("[READ] New tag detected!"));
    return this->rdm6300.get_tag_id();
  }

  int getUartNumber()
  {
    switch(boardModel->readerRxPin){
      case 3: return 0; // UART0 ESP32
      case 16: return 2; // UART2 ESP32
      case 20: return 0; // UART0 ESP32C3
      default: return -1; // No hardware pin for UART
    };
  }
};





// #include <Arduino.h>
// #include <SoftwareSerial.h>
// #include "board-models.cpp"
// #include "models.cpp"


// class Reader
// {
// private:
//   EspSoftwareSerial::UART serial;
//   BoardModel *boardModel;

// public:
//   Reader(BoardModel *boardModel): boardModel(boardModel) {}
 
//   void startup()
//   {
//     // this->serial = new SoftwareSerial(this->boardModel->readerRxPin,-1); // RX and TX
//     this->serial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, this->boardModel->readerRxPin, -1);
//   }

//   Uid readUid()
//   {
//     char c;
//     String text = "";
//     while (this->serial.available() > 0) {
//       delay(5);
//       c = this->serial.read();
//       text += c;
//     }
//     if (text.length() > 1) {
//       Serial.println("REMOVE! Read UID: " + text);
//       // Assuming the UID is in hex format, convert it to Uid type
//       Uid uid = strtoul(text.c_str(), nullptr, 16);
//       return uid;
//     }
//     return UID_NULL; // Return null UID if no valid UID is read
//   }
// };





#endif