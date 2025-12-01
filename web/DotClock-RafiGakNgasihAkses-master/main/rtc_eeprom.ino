// rtc_eeprom.ino
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "rtc_eeprom.h"

// Definisi nyata RTC
RTC_DS1307 rtc;

//int h, m, s, NTPday, NTPmonth, NTPyear, NTPdayOfWeek;

// Konstanta untuk EEPROM
const int EEPROM_SIZE   = 1024;
const int EEPROM_HEADER = 2;  // dua byte pertama menyimpan panjang data

// Array nama hari dalam bahasa Indonesia
const char* hari[7] = {
  "Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"
};

// -----------------------------------------------------------------------------
// Inisialisasi RTC dan memuat data alarm dari EEPROM
// -----------------------------------------------------------------------------
void initRTCAndLoadAlarms() {
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
  } else {
    Serial.println("RTC OK.");

    if (!rtc.isrunning()) {
      Serial.println("RTC belum berjalan!");
      // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // opsional: set waktu compile
    }
  }

  // Inisialisasi EEPROM
  EEPROM.begin(EEPROM_SIZE);
  Serial.println("EEPROM initialized.");

  // Tampilkan data alarm dari EEPROM (opsional)
  String s = readJsonFromEEPROM();
  if (s.length() > 10) {
    Serial.print("Loaded alarms JSON: ");
    Serial.println(s);
  }
}

// -----------------------------------------------------------------------------
// Membaca data JSON dari EEPROM
// -----------------------------------------------------------------------------
String readJsonFromEEPROM() {
  uint16_t len = ((uint16_t)EEPROM.read(0) << 8) | EEPROM.read(1);
  if (len == 0xFFFF || len == 0 || len > EEPROM_SIZE - EEPROM_HEADER) return "";

  String s;
  s.reserve(len);

  for (int i = 0; i < len; i++) {
    char c = EEPROM.read(EEPROM_HEADER + i);
    s += c;
  }
  return s;
}

// -----------------------------------------------------------------------------
// Menulis data JSON ke EEPROM
// -----------------------------------------------------------------------------
bool writeJsonToEEPROM(const String& s) {
  int len = s.length();

  if (len > EEPROM_SIZE - EEPROM_HEADER) {
    Serial.println("EEPROM: payload too big");
    return false;
  }

  EEPROM.write(0, (len >> 8) & 0xFF);
  EEPROM.write(1, len & 0xFF);

  for (int i = 0; i < len; i++) {
    EEPROM.write(EEPROM_HEADER + i, s[i]);
  }

  EEPROM.commit();
  Serial.printf("EEPROM: wrote %d bytes\n", len);
  return true;
}

// -----------------------------------------------------------------------------
// Mengatur RTC berdasarkan data string dari web (tanggal & waktu)
// -----------------------------------------------------------------------------
bool setRTCFromStrings(const char* dateStr, const char* timeStr) {
  int yyyy, mm, dd, hh, min;

  if (sscanf(dateStr, "%d-%d-%d", &yyyy, &mm, &dd) != 3) return false;
  if (sscanf(timeStr, "%d:%d", &hh, &min) != 2) return false;

  DateTime dt(yyyy, mm, dd, hh, min, 0);
  rtc.adjust(dt);

  // Ambil waktu setelah diset
  DateTime now = rtc.now();
  int hariIndex = now.dayOfTheWeek();

  Serial.println("====================================");
  Serial.println("RTC telah diset oleh web!");
  Serial.printf("Tanggal: %02d/%02d/%04d\n", now.day(), now.month(), now.year());
  Serial.printf("Jam:     %02d:%02d:%02d\n", now.hour(), now.minute(), now.second());
  Serial.printf("Hari:    %s\n", hari[hariIndex]);
  Serial.println("====================================");

  return true;
}

// -----------------------------------------------------------------------------
// Menampilkan waktu & tanggal RTC di Serial Monitor
// -----------------------------------------------------------------------------
void printDateTime() {
  DateTime now = rtc.now();
  
  h = now.hour();
  m = now.minute();
  s = now.second();
  NTPday = now.day();
  NTPdayOfWeek = now.dayOfTheWeek();
  NTPmonth = now.month();
  NTPyear = now.year();
    
  Serial.printf(
    "%s, %02d/%02d/%04d %02d:%02d:%02d\n",
    hari[now.dayOfTheWeek()],
    now.day(),
    now.month(),
    now.year(),
    now.hour(),
    now.minute(),
    now.second()
  );
  
}
