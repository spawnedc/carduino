#include <LiquidCrystal.h>

#include <Carduino.h>

Carduino carduino;

void setup() {
  Serial.begin(9600);
  carduino.begin();
  delay(1000);
}

void loop() {
  carduino.loop();
  delay(1000);
}
