#include <MFRC522.h>

int count = 0;

const int PushButton = 23;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  pinMode(PushButton, INPUT);
}

void loop()
{
  int Push_button_state = digitalRead(PushButton);
  if (count > 50)
  {
    if (Push_button_state == HIGH)
      Serial.println(1);
    else
      Serial.println(0);
    count = 0;
  }
  count++;
}
