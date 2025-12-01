float utcOffset = 7;                      // Time Zone setting

#define DIN_PIN 15                        // Data Pin - Nodemcu D8
#define CS_PIN  13                        // Chip Select - Nodemcu D7
#define CLK_PIN 12                        // Clock In - Nodemcu D6

//set false to not display the text before date scrolling.
bool enableTeks = true;
bool enableDHT = true;
char teksBuf[80] = "DOTclock ";

int h, m, s, NTPday, NTPmonth, NTPyear, NTPdayOfWeek;
float temp;
float hum;
char txtDHT[30];
char monthNames[][5] = {"JAN","FEB","MAR","APR","MEI","JUN","JUL","AGU","SEP","OKT","NOV","DES"};
char namaHari[][7] =   {"SENIN", "SELASA", "RABU", "KAMIS", "JUMAT", "SABTU", "MINGGU"}; // array hari, dihitung mulai dari senin, hari senin angka nya =0,
char txtScroll[80];

#include "fonts.h"
#include "max7219.h"

void startDisplay(){
  initMAX7219();
  sendCmdAll(CMD_SHUTDOWN,1);
  sendCmdAll(CMD_INTENSITY, 0);
}


void displayClock(){
    //if big Clock has been displayed for 30s
  if(millis()-clkTime > 30000 && !del && dots) {
    printDateTime();
    printStringWithShift("  ",40);
    delay(5);
    if(enableTeks) printStringWithShift(teksBuf,40);
    delay(5);
    sprintf(txtDHT, "T:%dC -- H:%d%% - ", (int)temp, (int)hum);
    if(enableDHT) printStringWithShift(txtDHT,40);
    delay(5);
    sprintf(txtScroll, "%s, %d-%s-%d", namaHari[NTPdayOfWeek - 1], NTPday, monthNames[NTPmonth-1], NTPyear);
    printStringWithShift(txtScroll,40);
    delay(10);
    printStringWithShift("           ",40);
    delay(10);
    clkTime = millis();
  }
  
  //dot blinking every 500ms
  if(millis()-dotTime > 500) {
    dots = !dots;
    dotTime = millis();
  }
  showAnimClock();
    
  // Adjust LED intensity(0-15).
  // 12am to 6am
  if ( (h == 0) || ((h >= 1) && (h <= 6)) ) sendCmdAll(CMD_INTENSITY, 0);
  // 6pm to 11pm
  else if ( (h >=18) && (h <= 23) ) sendCmdAll(CMD_INTENSITY, 2);
  // during bright daylight
  else sendCmdAll(CMD_INTENSITY, 5);

}

void loadTextFromEEPROM() {
  String json = readJsonFromEEPROM(); // implementasi di rtc_eeprom.h
  if (json.length() == 0) return;

  DynamicJsonDocument doc(512);
  if (deserializeJson(doc, json) != DeserializationError::Ok) return;

  const char* newText = doc["custom_text"];
  if (newText) {
    strncpy(teksBuf, newText, sizeof(teksBuf)-1);
    teksBuf[sizeof(teksBuf)-1] = '\0';
  }
}
 
