#include "Carduino.h"
#include "LiquidCrystal.h"
#include "DHT.h"
#include "Time.h"
#include "Wire.h"
#include "DS1307RTC.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "Arduino.h"


LiquidCrystal Carduino::LCD = LiquidCrystal(PIN_LCD_RS, PIN_LCD_ENABLE, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
DHT Carduino::TEMP = DHT(PIN_DHT, DHTTYPE);
tmElements_t Carduino::TIME;
OneWire Carduino::ONEWIRE(PIN_ONE_WIRE_BUS);
DallasTemperature Carduino::SENSORS(&Carduino::ONEWIRE);
DeviceAddress Carduino::PROBE01 = { 0x28, 0xFF, 0x8F, 0x34, 0x63, 0x14, 0x02, 0x80 };

byte cBar0[8]     = { 0b11011, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11011, 0b00000 };
byte cBar50[8]    = { 0b11011, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11011, 0b00000 };
byte cBar100[8]   = { 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b00000 };
byte cCelcius[8]  = { 0b01000, 0b10100, 0b01000, 0b00011, 0b00100, 0b00100, 0b00011, 0b00000 };
byte cEmpty[8]    = { 0b11111, 0b10001, 0b10111, 0b10011, 0b10111, 0b10001, 0b11111, 0b00000 };
byte cFull[8]     = { 0b11111, 0b10001, 0b10111, 0b10011, 0b10111, 0b10111, 0b11111, 0b00000 };
byte cLeisure[8]  = { 0b11111, 0b10111, 0b10111, 0b10111, 0b10111, 0b10001, 0b11111, 0b00000 };
byte cStarter[8]  = { 0b11111, 0b10001, 0b10111, 0b10001, 0b11101, 0b10001, 0b11111, 0b00000 };

float intTemp;
float extTemp;
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
  setupSensors();
  setupVoltageReaders();
}

void Carduino::loop(void) {
  intTemp = readIntTemp();
  extTemp = readExtTemp();
  // dist = readDist();
  tm = readTime();
  vStarter = readStarterVoltage();
  vLeisure = readLeisureVoltage();


  Carduino::LCD.clear();

  Carduino::LCD.setCursor(0, 0);
  Carduino::LCD.print("Int ");
  Carduino::LCD.print(intTemp, 1);
  Carduino::LCD.print((char)CHAR_CELCIUS);

  Carduino::LCD.setCursor(0, 1);
  Carduino::LCD.print("Ext ");
  Carduino::LCD.print(extTemp, 1);
  Carduino::LCD.print((char)CHAR_CELCIUS);

  Carduino::LCD.setCursor(15, 0);
  Carduino::LCD.print(tm);

  Carduino::LCD.setCursor(0, 2);
  Carduino::LCD.write((char)CHAR_STARTER);
  Carduino::LCD.print(vStarter, 2);
  Carduino::LCD.print("v");

  Carduino::LCD.setCursor(0, 3);
  Carduino::LCD.write((char)CHAR_LEISURE);
  Carduino::LCD.print(vLeisure, 2);
  Carduino::LCD.print("v");

  // distPerc = dist / 200.0;
  // if (dist != -1) {
  //   Carduino::LCD.setCursor(10, 1);
  //   Carduino::generateProgressBar(distPerc, LCD_COLUMNS/2);
  // }

  Carduino::LCD.setCursor(8, 2);
  Carduino::generateProgressBar(0.75, 12);

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

void Carduino::setupSensors() {
  Serial.println("Carduino::setupSensors");
  Carduino::SENSORS.begin();
  Carduino::SENSORS.setResolution(Carduino::PROBE01, 10);
}

void Carduino::setupVoltageReaders() {
  Serial.println("Carduino::setupVoltageReaders");
  pinMode(PIN_BATTERY_S, OUTPUT);
  pinMode(PIN_BATTERY_L, OUTPUT);
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
  int value;
  int val2;
  float temp;
  float vout;
  float vin;
  vout = analogRead(1);
  temp = vout/4.092;
  value = (int)temp;
  val2 = ((value % 100) / 10);
  vin = vout * 0.0244;
  Serial.print(vout);
  Serial.print(" || ");
  Serial.print(temp);
  Serial.print(" || ");
  Serial.print(value);
  Serial.print(" || ");
  Serial.print(vin);
  Serial.print(" || ");
  Serial.println(val2);
  return vin;
}

float Carduino::readIntTemp() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius
  return Carduino::TEMP.readTemperature();
}

float Carduino::readExtTemp() {
  Carduino::SENSORS.requestTemperatures();
  return Carduino::SENSORS.getTempC(Carduino::PROBE01);
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
  String h, m;

  RTC.read(Carduino::TIME);

  h = print2digits(Carduino::TIME.Hour);
  m = print2digits(Carduino::TIME.Minute);
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
