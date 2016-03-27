#include "Carduino.h"
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "Time.h"
#include <DS1307RTC.h>
#include <Arduino.h>

LiquidCrystal_I2C Carduino::LCD = LiquidCrystal_I2C(LCD_ADDR, LCD_COLUMNS, LCD_ROWS);
DHT Carduino::TEMP = DHT(PIN_DHT, DHTTYPE);
tmElements_t Carduino::TIME;

float intTemp;
int dist;
float distPerc;
float vStarter;
float vLeisure;
float R1 = 30000.0;
float R2 = 7500.0;
float RFactor = R2 / (R1 + R2);
float AFactor = 4.7010 / 1024; //4.7547
float voltageFactor = AFactor / RFactor;

String tm;

void Carduino::begin(void) {
  Serial.println("Carduino::begin");

  setupLCD();
  setupDHT();
  setupVoltageReaders();
}

void Carduino::loop(void) {

  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      RTC.set(t);   // set the RTC and the system time to the received value
      setTime(t);
      Serial.println("Time set");
    }
  }

  intTemp = readIntTemp();
  tm = readTime();
  vStarter = readStarterVoltage();
  vLeisure = readLeisureVoltage();

  Carduino::LCD.clear();

  Carduino::LCD.setCursor(15, 0);
  Carduino::LCD.print(tm);

  Carduino::LCD.setCursor(0, 0);
  Carduino::LCD.print("Int:");
  Carduino::LCD.print(intTemp, 0);
  Carduino::LCD.print("c");

  Carduino::LCD.setCursor(0, 2);
  Carduino::LCD.print("S:");
  Carduino::LCD.print(vStarter, 2);
  Carduino::LCD.print("v");

  Carduino::LCD.setCursor(0, 3);
  Carduino::LCD.print("L:");
  Carduino::LCD.print(vLeisure, 2);
  Carduino::LCD.print("v");

}

void Carduino::setupLCD() {
  Serial.println("Carduino::setupLCD");
  Carduino::LCD.begin(LCD_COLUMNS, LCD_ROWS);
  Carduino::LCD.backlight();
  Carduino::LCD.clear();
  Carduino::LCD.setCursor(6, 1);
  Carduino::LCD.print("CARDUINO");
}

void Carduino::setupDHT() {
  Serial.println("Carduino::setupDHT");
  Carduino::TEMP.begin();
}

void Carduino::setupVoltageReaders() {
  Serial.println("Carduino::setupVoltageReaders");
  pinMode(PIN_BATTERY_S, INPUT);
  pinMode(PIN_BATTERY_L, INPUT);
}

float Carduino::readLeisureVoltage() {
  int value = analogRead(PIN_BATTERY_L);
  float voltage = value * voltageFactor;
  Serial.print("Starter voltage: ");
  Serial.println(voltage);
  return voltage;
}

float Carduino::readStarterVoltage() {
  int value = analogRead(PIN_BATTERY_S);
  float voltage = value * voltageFactor;
  Serial.print("Leisure voltage: ");
  Serial.println(voltage);
  return voltage;
}

float Carduino::readIntTemp() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius
  float temp = Carduino::TEMP.readTemperature();
  Serial.print("Temp: ");
  Serial.println(temp);
  return temp;
}

String Carduino::readTime() {
  String timeToShow;
  String h, m;

  RTC.read(Carduino::TIME);

  Serial.print("Hour:");
  Serial.print(Carduino::TIME.Hour);

  Serial.print("  Minute:");
  Serial.println(Carduino::TIME.Minute);

  h = print2digits(Carduino::TIME.Hour);
  m = print2digits(Carduino::TIME.Minute);
  timeToShow = h + ':' + m;

  Serial.println("Time to show: " + timeToShow);

  return timeToShow;
}

String Carduino::print2digits(int number) {
  String s;
  if (number >= 0 && number < 10) {
    s = '0' + String(number);
  } else {
    s = String(number);
  }
  return s;
}

time_t Carduino::processSyncMessage() {
  unsigned long pctime = 0L;
  char *timeStr = (char*)"T";

  if (Serial.find(timeStr)) {
     pctime = Serial.parseInt();
  }
  return pctime;
}
