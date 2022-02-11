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

#include <Afficheur.h>    // Afficheur SSD1306
#include <Led.h>          // Led RGB    
#include <LM75A.h>

static bool lectureGPS(unsigned long ms);


TinyGPS gps;
HardwareSerial serialGps(2); // sur hardware serial 2

Afficheur *afficheur;
Led *led;
LM75A CapteurLM75A;

int page;

void setup() {
    Serial.begin(115200);
    serialGps.begin(4800, SERIAL_8N1, 16, 17);
    pinMode(34, INPUT); // BP en entrée
    delay(100);

    afficheur = new Afficheur;
    afficheur->afficher("Syn GPS");

    led = new Led;
    led->allumerRouge();

    page = 0;

}

void loop() {

    
    if (++page == 6)
        page = 0;
    
    if (lectureGPS(1000)) {
        switch (page) {
            case 0:
                afficheur->afficherHeure(gps);
                break;
            case 2:
                afficheur->afficherPosition(gps);
                break;
            case 4:
                afficheur->afficherFloat("Temp : ", CapteurLM75A.getTemperature());
                break;
        }


        led->allumerVert();

    } else {
        afficheur->afficher("Syn GPS");
        led->allumerRouge();

    }
    Serial.println(CapteurLM75A.getTemperature());
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





