// alarm.ino
#include <Arduino.h>
#include <ArduinoJson.h>
#include "rtc_eeprom.h"

// External variables from other files
extern float g_lastTemp;
extern float g_lastHum;

// Prevent alarm from triggering again in the same minute
static int lastTriggeredMinute = -1;

bool alarmStat = false;

// Non-blocking alarm timing
bool alarmActive = false;
unsigned long alarmEndTime = 0;

// -----------------------------------------------------------------------------
// checkAlarmsLoop()
// Reads alarm list from EEPROM, checks current time, triggers alarm (non-blocking)
// -----------------------------------------------------------------------------
void checkAlarmsLoop() {
  // Read JSON alarm data from EEPROM
  String s = readJsonFromEEPROM();
  if (s.length() < 5) return;

  // Parse JSON
  DynamicJsonDocument doc(1024);
  DeserializationError err = deserializeJson(doc, s);
  if (err) {
    Serial.println("Alarm JSON parse error");
    return;
  }

  if (!doc.containsKey("alarms")) return;

  // Current RTC time
  DateTime now = rtc.now();
  int hh = now.hour();
  int mm = now.minute();
  int currentMinuteKey = hh * 60 + mm;

  // Avoid duplicate triggering in the same minute
  if (lastTriggeredMinute == currentMinuteKey) {
    // Still need to handle alarm timing even if no new trigger
    if (alarmActive && millis() > alarmEndTime) {
      alarmActive = false;
      alarmStat = false;
      digitalWrite(ALARM_OUT_PIN, LOW);
    }
    return;
  }

  // Check each alarm entry
  const JsonArray alarms = doc["alarms"].as<JsonArray>();
  for (JsonObject alarm : alarms) {
    const char* timeStr = alarm["alarm_time"];     // "HH:MM"
    bool active = alarm["is_active"] | true;
    if (!timeStr) continue;

    int ah = 0, am = 0;
    if (sscanf(timeStr, "%d:%d", &ah, &am) != 2) continue;
    if (!active) continue;

    // Match current time
    if (ah == hh && am == mm) {
      Serial.printf("Alarm triggered at %02d:%02d\n", hh, mm);

      alarmStat = true;           // tell displayClock() to show ALARM screen
      alarmActive = true;         // internal alarm is running
      alarmEndTime = millis() + 1000;   // alarm runs for 1 second

      digitalWrite(ALARM_OUT_PIN, HIGH);

      lastTriggeredMinute = currentMinuteKey;
      break;
    }
  }

  // End alarm when duration expires
  if (alarmActive && millis() > alarmEndTime) {
    alarmActive = false;
    alarmStat = false;
    digitalWrite(ALARM_OUT_PIN, LOW);
  }
}
