#include "Carduino.h"

Carduino carduino;

void setup() {
  Serial.begin(9600);
  carduino.begin();
  delay(1000);
}

void loop() {
  carduino.loop();
  Serial.println("=================================");
  delay(2000);
}
