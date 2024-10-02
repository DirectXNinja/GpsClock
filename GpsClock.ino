#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>
#include <RTClib.h>
#include <Adafruit_GPS.h>

#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_ADDR   0x3C
#define RTC_ADDR    0x68
#define BMP280_ADDR 0x77
#define DELAY       10000
#define GPSSerial Serial1
#define GPSECHO true
#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)
#define GPSECHO true

uint32_t timer = millis();

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire);
RTC_DS3231 rtc;
Adafruit_BMP280 bmp;
Adafruit_GPS GPS(&GPSSerial);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  unsigned status;
  bmp.begin();
  rtc.begin();
  delay(2000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("GPS Start Screen");
  display.println("Hello from Core-1");
  display.display();
  delay(5000);


  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

}

void setup1() {
  Serial.begin(115200);
  GPS.begin(9600);
  delay(2000);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(DELAY);
  GPSSerial.println(PMTK_Q_RELEASE);
}

void bmpPrint() {
  display.clearDisplay();
  display.setCursor(0,0);

  display.print("Temp : ");
  display.println(bmp.readTemperature());
  display.print("Pres : ");
  display.println(bmp.readPressure()/100);
  display.print("Alti : ");
  display.println(bmp.readAltitude(1013.25));
  display.display();
}

void rtcPrint() {
  DateTime now = rtc.now();
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(now.year(), DEC);
  display.print('/');
  display.print(now.month(), DEC);
  display.print('/');
  display.println(now.day(), DEC);
  display.print(now.hour(), DEC);
  display.print(':');
  display.print(now.minute(), DEC);
  display.print(':');
  display.print(now.second(), DEC);
  display.display();
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.print("SAT : ");
  display.println((int)GPS.satellites);
  display.print("LON : ");
  display.println(GPS.longitudeDegrees);
  display.print("LAT : ");
  display.println(GPS.latitudeDegrees);
  display.print("Met : ");
  display.println(GPS.altitude);
  display.print("Kmh : ");
  display.println(GPS.speed * 1.852);

  display.display();
  delay(DELAY);

  bmpPrint();
  delay(DELAY);

  rtcPrint();

}

void loop1()
{
  char c = GPS.read();
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer

    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);

      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
    }
  }
}