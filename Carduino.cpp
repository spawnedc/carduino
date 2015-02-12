#include "Carduino.h"
#include "LiquidCrystal.h"
#include "DHT.h"
#include "Time.h"
#include "Wire.h"
#include "DS1307RTC.h"
#include "Arduino.h"


LiquidCrystal Carduino::LCD = LiquidCrystal(PIN_LCD_RS, PIN_LCD_ENABLE, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
DHT Carduino::TEMP = DHT(PIN_DHT, DHTTYPE);
tmElements_t Carduino::TIME;

byte cBar0[8]    = { 0b11011, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11011, 0b00000 };
byte cBar50[8]   = { 0b11011, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11011, 0b00000 };
byte cBar100[8]  = { 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b00000 };
byte cCelcius[8] = { 0b01000, 0b10100, 0b01000, 0b00011, 0b00100, 0b00100, 0b00011, 0b00000 };
byte cEmpty[8]   = { 0b11111, 0b10001, 0b10111, 0b10011, 0b10111, 0b10001, 0b11111, 0b00000 };
byte cFull[8]    = { 0b11111, 0b10001, 0b10111, 0b10011, 0b10111, 0b10111, 0b11111, 0b00000 };
byte cLeisure[8] = { 0b11111, 0b10111, 0b10111, 0b10111, 0b10111, 0b10001, 0b11111, 0b00000 };
byte cStarter[8] = { 0b11111, 0b10001, 0b10111, 0b10001, 0b11101, 0b10001, 0b11111, 0b00000 };


float temp;
int dist;
float distPerc;
float vStarter;
float vLeisure;
String tm;

void Carduino::begin(void) {
  Serial.println("Carduino::begin");

  setupLCD();
  setupDHT();
  setupDist();
}

void Carduino::loop(void) {
  temp = readTemp();
  dist = readDist();
  tm = readTime();
  vStarter = readStarterVoltage();
  vLeisure = readLeisureVoltage();

  distPerc = dist / 200.0;

  Carduino::LCD.clear();
  Carduino::LCD.print("Int ");
  Carduino::LCD.print(temp, 1);
  Carduino::LCD.print((char)CHAR_CELCIUS);

  Carduino::LCD.setCursor(15, 0);
  Carduino::LCD.print(tm);

  Carduino::LCD.setCursor(0, 2);
  Carduino::LCD.write(CHAR_STARTER);
  Carduino::LCD.print(vStarter, 2);
  Carduino::LCD.print("v");

  Carduino::LCD.setCursor(0, 1);
  Carduino::generateProgressBar(distPerc, LCD_COLUMNS);

  Carduino::LCD.setCursor(8, 2);
  Carduino::generateProgressBar(0.75, 12);

  Carduino::LCD.setCursor(0, 3);
  Carduino::LCD.write(CHAR_LEISURE);
  Carduino::LCD.print(vLeisure, 2);
  Carduino::LCD.print("v");

  Carduino::LCD.setCursor(8, 3);
  Carduino::generateProgressBar(0.92, 12);
}

void Carduino::setupLCD() {
  Serial.println("Carduino::setupLCD");
  Carduino::LCD.begin(LCD_COLUMNS, LCD_ROWS);
  Carduino::LCD.createChar(CHAR_BAR_0, cBar0);
  Carduino::LCD.createChar(CHAR_BAR_50, cBar50);
  Carduino::LCD.createChar(CHAR_BAR_100, cBar100);
  Carduino::LCD.createChar(CHAR_CELCIUS, cCelcius);
  Carduino::LCD.createChar(CHAR_EMPTY, cEmpty);
  Carduino::LCD.createChar(CHAR_FULL, cFull);
  Carduino::LCD.createChar(CHAR_LEISURE, cLeisure);
  Carduino::LCD.createChar(CHAR_STARTER, cStarter);
  Carduino::LCD.clear();
  Carduino::LCD.setCursor(6, 1);
  Carduino::LCD.print("CARDUINO");
}

void Carduino::setupDHT() {
  Serial.println("Carduino::setupDHT");
  Carduino::TEMP.begin();
}

void Carduino::setupDist() {
  Serial.println("Carduino::setupDist");
  pinMode(PIN_DIST1_TRIG, OUTPUT);
  pinMode(PIN_DIST1_ECHO, INPUT);
}

void Carduino::generateProgressBar(float perc, int cols) {
  cols -= 2;
  float value = cols * perc;
  int numSolids = cols * perc;
  int numPartials = (value - numSolids) * 10;

  Carduino::LCD.write(char(CHAR_EMPTY));

  for (int i=0; i< numSolids; i++) {
    Carduino::LCD.write(char(CHAR_BAR_100));
  }
  if (numPartials >= 5) {
    Carduino::LCD.write(char(CHAR_BAR_50));
    numSolids++;
  }
  for (int i=numSolids; i< cols; i++) {
    Carduino::LCD.write(char(CHAR_BAR_0));
  }
  Carduino::LCD.write(char(CHAR_FULL));

}

float Carduino::readLeisureVoltage() {
  return 12.63;
}

float Carduino::readStarterVoltage() {
  return 12.33;
}

float Carduino::readTemp() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius
  return Carduino::TEMP.readTemperature();
}

int Carduino::readDist() {
  long distance;
  int duration;

  digitalWrite(PIN_DIST1_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_DIST1_TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(PIN_DIST1_TRIG, LOW);

  duration = pulseIn(PIN_DIST1_ECHO, HIGH);

  // Calculate the distance (in cm) based on the speed of sound.
  distance =  duration / 58.2;

  if (distance >= 200 || distance <= 0) {
    distance = -1;
  }

  return distance;
}

String Carduino::readTime() {
  String timeToShow;
  String h, m, s, d, M, y;

  RTC.read(Carduino::TIME);

  h = print2digits(Carduino::TIME.Hour);
  m = print2digits(Carduino::TIME.Minute);
  d = print2digits(Carduino::TIME.Day);
  M = print2digits(Carduino::TIME.Month);
  y = print2digits(tmYearToCalendar(Carduino::TIME.Year));
  timeToShow = h + ':' + m;

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
