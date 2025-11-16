// ota.ino
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include "rtc_eeprom.h"

const char* ota_hostname = "DOTclock";
const char* ota_password = "12345678";

void setupOTA() {
  Serial.println("Setting up OTA...");
  ArduinoOTA.setHostname(ota_hostname);
  ArduinoOTA.setPassword(ota_password);

  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.printf("OTA Start (%s)\n", type.c_str());
  });
  ArduinoOTA.onEnd([]() { Serial.println("\nOTA End"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (unsigned int)((progress / (float)total) * 100));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  // ensure mDNS symbols linked
  if (MDNS.begin(ota_hostname)) Serial.println("MDNS responder started");
  else Serial.println("MDNS begin failed (ok in AP mode sometimes)");

  ArduinoOTA.begin();
  Serial.println("OTA ready.");
}

void handleOTA() {
  ArduinoOTA.handle();
}
