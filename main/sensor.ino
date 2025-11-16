// sensor.ino
#include <DHT.h>
#include "rtc_eeprom.h" // for externs if needed

extern float g_lastTemp;
extern float g_lastHum;
extern unsigned long g_lastSensorMillis;
extern const unsigned long SENSOR_INTERVAL;

DHT dht(DHTPIN, DHT11);


void readSensorOnce() {
  static bool inited = false;
  if (!inited) { dht.begin(); inited = true; }

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t)) g_lastTemp = t;
  if (!isnan(h)) g_lastHum = h;

  Serial.printf("Sensor read: T=%.2f H=%.0f\n", g_lastTemp, g_lastHum);
}

void getTempHum(){  
  temp = dht.readTemperature();
  hum = dht.readHumidity();
}
