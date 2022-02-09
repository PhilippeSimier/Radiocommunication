/* 
 * File:   main.cpp
 * Author: philippe SIMIER
 *
 * Created on 9 février 2022, 17:11
 * Bibliothèque TinyGPS 
 * installation :  pio lib -g install 416
 */

#include <Arduino.h>

#include <TinyGPS.h>   // GPS
#include <HardwareSerial.h>

#include <SSD1306Wire.h>  // OLED driver for SSD1306

static void print_date(TinyGPS &gps);
static bool lectureCapteur(unsigned long ms);
void afficher(String message);

TinyGPS gps;
HardwareSerial serialGps(2); // sur hardware serial 2
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_64);

void setup() {
    Serial.begin(115200);
    serialGps.begin(4800, SERIAL_8N1, 16, 17);
    delay(1000);
    Serial.print("Simple TinyGPS library v. ");
    Serial.println(TinyGPS::library_version());

    display.init();
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 0, "Rech GPS");
    display.drawString(0, 32, "en cours");
    display.display();
}

void loop() {
     
    if (lectureCapteur(1000)) {
        float flat, flon;
        unsigned long age;
        gps.f_get_position(&flat, &flon, &age);
        Serial.print(" Latitude = ");
        Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
        Serial.print(" Longitude = ");
        Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
        Serial.print(" Altitude = ");
        Serial.println(gps.f_altitude());
        Serial.print(" Nb Satellites = ");
        Serial.println(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
        print_date(gps);
    }
}

static void print_date(TinyGPS &gps) {
    int year;
    byte month, day, hour, minute, second, hundredths;
    unsigned long age;
    char sz[32] = "******";

    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);

    if (age != TinyGPS::GPS_INVALID_AGE) {
        sprintf(sz, "%02d:%02d:%02d", hour, minute, second);
    }
    Serial.print(sz);
    String message(sz);
    afficher(message);
}

static bool lectureCapteur(unsigned long ms) {
    bool newData = false;
    unsigned long start = millis();
    do {
        while (serialGps.available())
            if (gps.encode(serialGps.read())) {
                newData = true;
            }
    } while (millis() - start < ms);
    return newData;
}

void afficher(String message) {
    display.clear();
    display.drawStringMaxWidth(0, 0, 110, message);

    display.display();
}

