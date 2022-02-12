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
 * 
 * Bibliothèque Max31855 Thermocouple
 * installation : pio lib -g install 84
 * 
 * Bibliothèque BME280 @ 3.0.0
 * installation : pio lib -g install 901
 */

#include <Arduino.h>

#include <TinyGPS.h>   // GPS
#include <HardwareSerial.h>

#include <Afficheur.h>          // Afficheur SSD1306
#include <Led.h>                // Led RGB    
#include <LM75A.h>              // LM75A température intérieur
#include <Adafruit_MAX31855.h>  // Température thermocouple extérieur
#include <BME280I2C.h>          // Capteur Pression humidité température

static bool lectureGPS(unsigned long ms);


TinyGPS gps;
HardwareSerial serialGps(2); // sur hardware serial 2

Afficheur *afficheur;
Led *led;
LM75A CapteurLM75A(false, false, false, -1.5); // la valeur de l'offset est déterminée par la calibration
Adafruit_MAX31855 thermocouple(4);

BME280I2C::Settings parametrage(
        BME280::OSR_X1,
        BME280::OSR_X1,
        BME280::OSR_X1,
        BME280::Mode_Forced,
        BME280::StandbyTime_1000ms,
        BME280::Filter_Off,
        BME280::SpiEnable_False,
        BME280I2C::I2CAddr_0x77 // I2C address pour BME 280.
        );

BME280I2C bme(parametrage);

int page;

void setup() {
    Serial.begin(115200);
    serialGps.begin(4800, SERIAL_8N1, 16, 17);
    pinMode(34, INPUT); // BP en entrée
    delay(100);

    led = new Led;
    led->allumerRouge();

    afficheur = new Afficheur;

    afficheur->afficher("Could not find bme");
    while (!bme.begin()) {
        delay(1000);
    }


    afficheur->afficher("Syn GPS");



    page = 0; // Le numéro de la page à afficher

}

void loop() {


    if (++page == 9)
        page = 0;

    if (lectureGPS(1000)) {
        switch (page) {
            case 0: case 1: case 2:
                afficheur->afficherHeure(gps);
                break;
            case 3:
                afficheur->afficherPosition(gps);
                break;
            case 5:
                afficheur->afficherFloat("Temp int ", CapteurLM75A.getTemperature(), " °C");
                break;
            case 7:
                afficheur->afficherFloat("Temp ext ", (float) thermocouple.readCelsius(), " °C");
                break;

        }


        led->allumerVert();

    } else {
        afficheur->afficher("Syn GPS");
        led->allumerRouge();

    }

    Serial.print("Temp int: ");
    Serial.println(CapteurLM75A.getTemperature());
    float tempExt = (float) thermocouple.readCelsius();
    if (isnan(tempExt)) {
        Serial.println("Il y a un probleme!");
    } else {
        Serial.print("Thermocouple: ");
        Serial.println(tempExt);
    }
    
    float temp(NAN), hum(NAN), pres(NAN);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_Pa);

    bme.read(pres, temp, hum, tempUnit, presUnit);
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" °C");
    Serial.print("\tHumidité: ");
    Serial.print(hum);
    Serial.print("%");
    Serial.print("\tPression: ");
    Serial.print(pres/100);
    Serial.println(" hPa");

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





