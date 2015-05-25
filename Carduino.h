#include "LiquidCrystal.h"
#include "DHT.h"
#include "Time.h"
#include "Wire.h"
#include "DS1307RTC.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "Arduino.h"

#ifndef HEADER_CARDUINO
  #define HEADER_CARDUINO

  // LCD definitions
  #define PIN_LCD_D7         2
  #define PIN_LCD_D6         3
  #define PIN_LCD_D5         4
  #define PIN_LCD_D4         5
  #define PIN_LCD_ENABLE    11
  #define PIN_LCD_RS        12
  #define LCD_COLUMNS       20
  #define LCD_ROWS           4

  // DHT definitions
  #define PIN_DHT            6 // Temperature and humidity pin
  #define DHTTYPE            DHT11

  // Distance sensor 1 pin definitions
  #define PIN_DIST1_ECHO     9 // Echo pin for the distance sensor 1.
  #define PIN_DIST1_TRIG    10 // Trigger pin for the distance sensor 1.

  // Custom LCD characters
  #define CHAR_BAR_0         1
  #define CHAR_BAR_50        2
  #define CHAR_BAR_100       3
  #define CHAR_CELCIUS       4
  #define CHAR_EMPTY         5
  #define CHAR_FULL          6
  #define CHAR_LEISURE       7
  #define CHAR_STARTER       8

  #define PIN_ONE_WIRE_BUS   7


  class Carduino {
    static LiquidCrystal LCD;
    static DHT TEMP;
    static tmElements_t TIME;
    static OneWire ONEWIRE;
    static DallasTemperature SENSORS;
    static DeviceAddress PROBE01;

    public:
      void begin(void);
      void loop(void);

    private:
      void setupLCD(void);
      void setupDHT(void);
      void setupDist(void);
      void setupSensors(void);
      void generateProgressBar(float, int);
      float readIntTemp(void);
      float readExtTemp(void);
      float readLeisureVoltage(void);
      float readStarterVoltage(void);
      int readDist(void);
      String readTime(void);
      String print2digits(int);
      void custom0(int);
      void custom1(int);
      void custom2(int);
      void custom3(int);
      void custom4(int);
      void custom5(int);
      void custom6(int);
      void custom7(int);
      void custom8(int);
      void custom9(int);
      void printNumber(int, int);
  };

#endif
