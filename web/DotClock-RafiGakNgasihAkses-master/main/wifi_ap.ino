// wifi_ap.ino
#include <ESP8266WiFi.h>

// SSID & password sesuai permintaan
const char* ap_ssid = "DOTclock";
const char* ap_pass = "12345678";

void startAP() {
  Serial.println("Starting Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP SSID: ");
  Serial.println(ap_ssid);
  Serial.print("AP IP: ");
  Serial.println(ip);
}
