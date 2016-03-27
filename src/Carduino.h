#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "Time.h"
#include <DS1307RTC.h>
#include <Arduino.h>

#ifndef HEADER_CARDUINO
  #define HEADER_CARDUINO
  // LCD definitions
  #define LCD_COLUMNS       20
  #define LCD_ROWS           4
  #define LCD_ADDR        0x27

  // DHT definitions
  #define PIN_DHT            6 // Temperature and humidity pin
  #define DHTTYPE            DHT11

  #define PIN_BATTERY_S      1 // Analog
  #define PIN_BATTERY_L      2 // Analog


  class Carduino {
    static LiquidCrystal_I2C LCD;
    static DHT TEMP;
    static tmElements_t TIME;

    public:
      void begin(void);
      void loop(void);

    private:
      void setupLCD(void);
      void setupDHT(void);
      void setupVoltageReaders(void);
      float readIntTemp(void);
      float readLeisureVoltage(void);
      float readStarterVoltage(void);
      String readTime(void);
      String print2digits(int);
      time_t processSyncMessage();
  };

#endif
