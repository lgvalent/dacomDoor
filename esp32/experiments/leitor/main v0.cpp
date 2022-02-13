#include <Arduino.h>

long it = 0;
int count = 0;
int f = 512;

void setup() {
  // Take some time to open up the Serial Monitor
  Serial.begin(115200);
  delay(1000);

  Serial.println("Setup complete");
}

void loop() {}
