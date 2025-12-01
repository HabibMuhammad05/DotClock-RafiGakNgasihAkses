/*-----------ESP8266 max7219 Panel Clock With RTC &Alarm----------------*/
/*---------------Wemos D1 - 32x8Px MAX7219 LED Matrix-------------------*/
/*-----------------Source Code by -              -----------------------*/
/*-------------Modified & Adapted by DOTClock Team (PMW)----------------*/
/*------------------------------V2.0------------------------------------*/
/*----------------------------------------------------------------------*/

// =======================================================================
// Define DEBUG to enable debugging(Serial Monitor); comment it out to disable
// =======================================================================
#define DEBUG

#ifdef DEBUG
  #define PRINT(...) Serial.print(__VA_ARGS__)
  #define PRINTLN(...) Serial.println(__VA_ARGS__)
  #define BEGIN(baud) Serial.begin(baud)
#else
  #define PRINT(...)    // Do nothing
  #define PRINTLN(...)  // Do nothing
  #define BEGIN(baud)   // Do nothing
#endif

// main.ino
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "rtc_eeprom.h"   // deklarasi extern rtc, fungsi EEPROM/RTC
#include "webpage.h"      // PROGMEM web assets (tidak wajib di main but ok)
#include <EEPROM.h>       // kami gunakan EEPROM.begin jika perlu

// === Pin definitions (opsi B: #define) ===
#define DHTPIN        D4
#define ALARM_OUT_PIN D5

// Global sensor state (definisi nyata)
float g_lastTemp = NAN;
float g_lastHum = NAN;
unsigned long g_lastSensorMillis     = 0;
const unsigned long SENSOR_INTERVAL  = 5UL * 1000UL; // 30s
unsigned long lastDisplay            = 0;
const unsigned long DISPLAY_INTERVAL = 2000; // 2 detik

// Forward declarations (implemented in other tabs)
void startAP();
void initServer();
void handleClientLoop();
void readSensorOnce();
void checkAlarmsLoop();
void setupOTA();
void handleOTA();
void initRTCAndLoadAlarms();

void setup() {
  Serial.begin(115200);
  delay(200);
  startDisplay();
  pinMode(ALARM_OUT_PIN, OUTPUT);
  digitalWrite(ALARM_OUT_PIN, LOW);

  // Start AP
  startAP();

  // init sensor once
  readSensorOnce();

  // init RTC + EEPROM (in rtc_eeprom.ino)
  initRTCAndLoadAlarms();

  // setup OTA (after WiFi started)
  setupOTA();

  // start webserver
  initServer();

  //EEPROM text
  loadTextFromEEPROM();

  Serial.println("Setup complete.");
}

void loop() {
  
  displayClock();
  
  // sensor update non-blocking
  unsigned long nowMs = millis();
  if (nowMs - g_lastSensorMillis >= SENSOR_INTERVAL) {
    g_lastSensorMillis = nowMs;
    readSensorOnce();
    getTempHum();
  }

  if (millis() - lastDisplay >= 2000) {
    lastDisplay = millis();
    printDateTime();
  }

  // check alarms
  checkAlarmsLoop();

  // handle http clients
  handleClientLoop();

  // handle OTA
  handleOTA();

  delay(10);
}
