#include "Carduino.h"
// #include "LiquidCrystal.h"
#include "Adafruit_GFX.h" // Core graphics library
#include "Adafruit_ST7735.h" // Hardware-specific library
#include "SPI.h"
#include "DHT.h"
#include "Time.h"
#include "Wire.h"
#include "DS1307RTC.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "Arduino.h"


// LiquidCrystal Carduino::LCD = LiquidCrystal(PIN_LCD_RS, PIN_LCD_ENABLE, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
Adafruit_ST7735 Carduino::TFT = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
DHT Carduino::TEMP = DHT(PIN_DHT, DHTTYPE);
tmElements_t Carduino::TIME;
OneWire Carduino::ONEWIRE(PIN_ONE_WIRE_BUS);
DallasTemperature Carduino::SENSORS(&Carduino::ONEWIRE);
DeviceAddress Carduino::PROBE01 = { 0x28, 0xFF, 0x8F, 0x34, 0x63, 0x14, 0x02, 0x80 };

// byte cBar0[8]     = { 0b11011, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11011, 0b00000 };
// byte cBar50[8]    = { 0b11011, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11011, 0b00000 };
// byte cBar100[8]   = { 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b00000 };
// byte cCelcius[8]  = { 0b01000, 0b10100, 0b01000, 0b00011, 0b00100, 0b00100, 0b00011, 0b00000 };
// byte cEmpty[8]    = { 0b11111, 0b10001, 0b10111, 0b10011, 0b10111, 0b10001, 0b11111, 0b00000 };
// byte cFull[8]     = { 0b11111, 0b10001, 0b10111, 0b10011, 0b10111, 0b10111, 0b11111, 0b00000 };
// byte cLeisure[8]  = { 0b11111, 0b10111, 0b10111, 0b10111, 0b10111, 0b10001, 0b11111, 0b00000 };
// byte cStarter[8]  = { 0b11111, 0b10001, 0b10111, 0b10001, 0b11101, 0b10001, 0b11111, 0b00000 };

float intTemp;
float extTemp;
int dist;
float distPerc;
float vStarter;
float vStarterPerc;
float vLeisure;
float vLeisurePerc;
float R1 = 30000.0;
float R2 = 7500.0;
float RFactor = R2 / (R1 + R2);
float AFactor = 4.7010 / 1024; //4.7547
float voltageFactor = AFactor / RFactor;
float battMin = 11.89;
float battMax = 12.66;
float battRange = battMax - battMin;

String tm;

void Carduino::begin(void) {
  Serial.println("Carduino::begin");

  // setupLCD();
  setupTFT();
  setupDHT();
  setupDist();
  setupSensors();
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
  extTemp = readExtTemp();
  // dist = readDist();
  tm = readTime();
  vStarter = readStarterVoltage();
  vLeisure = readLeisureVoltage();

  if (vStarter > battMin) {
    vStarterPerc = (vStarter - battMin) / battRange;
  } else {
    vStarterPerc = 0.0;
  }

  if (vLeisure > battMin) {
    vLeisurePerc = (vLeisure - battMin) / battRange;
  } else {
    vLeisurePerc = 0.0;
  }


  Carduino::TFT.fillScreen(ST7735_BLACK);
  Carduino::TFT.setTextColor(ST7735_WHITE);

  Carduino::TFT.setCursor(0, 0);
  Carduino::TFT.setTextSize(4);
  Carduino::TFT.setCursor(22, 0);
  Carduino::TFT.print(tm);

  Carduino::TFT.setTextSize(2);
  Carduino::TFT.print("Int:");
  Carduino::TFT.print(intTemp, 0);
  Carduino::TFT.println("c");

  Carduino::TFT.print("Ext:");
  Carduino::TFT.print(extTemp, 0);
  Carduino::TFT.println("c");

  Carduino::TFT.print("S:");
  Carduino::TFT.print(vStarter, 2);
  Carduino::TFT.println("v");

  Carduino::TFT.print("L:");
  Carduino::TFT.print(vLeisure, 2);
  Carduino::TFT.print("v");

  // distPerc = dist / 200.0;
  // if (dist != -1) {
  //   Carduino::TFT.setCursor(10, 1);
  //   Carduino::generateProgressBar(distPerc, LCD_COLUMNS/2);
  // }

  // Carduino::TFT.setCursor(8, 2);
  // Carduino::generateProgressBar(vStarterPerc, 12);

  // Carduino::TFT.setCursor(8, 3);
  // Carduino::generateProgressBar(vLeisurePerc, 12);
}

// void Carduino::setupLCD() {
//   Serial.println("Carduino::setupLCD");
//   Carduino::LCD.begin(LCD_COLUMNS, LCD_ROWS);
//   Carduino::LCD.createChar(CHAR_BAR_0, cBar0);
//   Carduino::LCD.createChar(CHAR_BAR_50, cBar50);
//   Carduino::LCD.createChar(CHAR_BAR_100, cBar100);
//   Carduino::LCD.createChar(CHAR_CELCIUS, cCelcius);
//   Carduino::LCD.createChar(CHAR_EMPTY, cEmpty);
//   Carduino::LCD.createChar(CHAR_FULL, cFull);
//   Carduino::LCD.createChar(CHAR_LEISURE, cLeisure);
//   Carduino::LCD.createChar(CHAR_STARTER, cStarter);
//   Carduino::LCD.clear();
//   Carduino::LCD.setCursor(6, 1);
//   Carduino::LCD.print("CARDUINO");
// }

void Carduino::setupTFT() {
  Serial.println("Carduino::setupTFT");
  Carduino::TFT.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab
  Carduino::TFT.setTextSize(3);
  Carduino::TFT.setRotation(3);
  Carduino::TFT.fillScreen(ST7735_BLACK);
  Carduino::TFT.setCursor(3, 0);
  Carduino::TFT.print("CARDUINO");
  Carduino::TFT.setTextSize(2);
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
  pinMode(PIN_BATTERY_S, INPUT);
  pinMode(PIN_BATTERY_L, INPUT);
}

void Carduino::generateProgressBar(float perc, int cols) {
  cols -= 2;
  float value = cols * perc;
  int numSolids = cols * perc;
  int numPartials = (value - numSolids) * 10;

  // Carduino::LCD.write(char(CHAR_EMPTY));

  // for (int i=0; i< numSolids; i++) {
  //   Carduino::LCD.write(char(CHAR_BAR_100));
  // }
  // if (numPartials >= 5) {
  //   Carduino::LCD.write(char(CHAR_BAR_50));
  //   numSolids++;
  // }
  // for (int i=numSolids; i< cols; i++) {
  //   Carduino::LCD.write(char(CHAR_BAR_0));
  // }
  // Carduino::LCD.write(char(CHAR_FULL));

}

float Carduino::readLeisureVoltage() {
  int value = analogRead(PIN_BATTERY_L);
  return value * voltageFactor;
}

float Carduino::readStarterVoltage() {
  int value = analogRead(PIN_BATTERY_S);
  Serial.println(value);
  return value * voltageFactor;
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

time_t Carduino::processSyncMessage() {
  unsigned long pctime = 0L;

  if (Serial.find("T")) {
     pctime = Serial.parseInt();
  }
  return pctime;
}
