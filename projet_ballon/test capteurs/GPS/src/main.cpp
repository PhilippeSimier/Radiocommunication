/* 
 * File:   main.cpp
 * Author: philippe SIMIER
 *
 * Created on 9 février 2022, 17:11
 * 
 * Bibliothèque TinyGPS 
 * installation :  pio lib -g install 416
 * 
 * Bibliothèque :  NeoPixelBus @ 2.6.9
 * installation :  pio lib -g install 547
 * 
 * Bibliothèque Oled 
 * installation : pio lib -g install 2978
 */

#include <Arduino.h>

#include <TinyGPS.h>   // GPS
#include <HardwareSerial.h>

#include <SSD1306Wire.h>  // OLED driver for SSD1306
#include <NeoPixelBus.h>  // Led RGB WS2812

#define PIN_RGB     15   
#define NUMPIXELS   1

RgbColor rouge(8, 0, 0);
RgbColor  vert(0, 4, 0);

static void afficher_heure(TinyGPS &gps);
static bool lectureGPS(unsigned long ms);
void afficher(String message);
static void allumer(NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> &led, RgbColor couleur);

TinyGPS gps;
HardwareSerial serialGps(2); // sur hardware serial 2
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_64);
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> led(NUMPIXELS, PIN_RGB);

void setup() {
    Serial.begin(115200);
    serialGps.begin(4800, SERIAL_8N1, 16, 17);
    delay(1000);
    Serial.print("Simple TinyGPS library v. ");
    Serial.println(TinyGPS::library_version());

    display.init();
    display.setFont(ArialMT_Plain_24);
    afficher("Syn GPS");
    
    led.Begin();
    allumer(led, rouge);
    
}

void loop() {
     
    if (lectureGPS(1000)) {
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
        afficher_heure(gps);
        allumer(led, vert);
    }else{
        afficher("Syn GPS");
        allumer(led, rouge);
        
    }
}

/**
 * 
 * @param gps une référence sur le capteur GPS
 */
static void afficher_heure(TinyGPS &gps) {
    int year;
    byte month, day, hour, minute, second, hundredths;
    unsigned long age;
    char sz[32] = "******";
    unsigned short nb = gps.satellites();
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);

    if (age != TinyGPS::GPS_INVALID_AGE) {
        sprintf(sz, "%02d:%02d:%02d  nb Sat: %d", hour, minute, second, nb);
    }
    Serial.print(sz);
    String message(sz);
    afficher(message);
}

/**
 * 
 * @param ms le temps de lecture en ms
 * @return  bool indique la présence de nouvelle donnée
 */
static bool lectureGPS(unsigned long ms) {
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

/**
 * 
 * @param message String le message à afficher sur l'écran oled
 */
void afficher(String message) {
    display.clear();
    display.drawStringMaxWidth(0, 0, 110, message);
    display.display();
}

/**
 * 
 * @param led une référence sur une led RGB
 * @param couleur une couleur de type RgbColor
 */
static void allumer(NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> &led, RgbColor couleur) {
    led.SetPixelColor(0, couleur);
    led.Show();
}

