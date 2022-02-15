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
 * Bibliothèque : Oled 
 * installation : pio lib -g install 2978
 * 
 * Bibliothèque Max31855 Thermocouple
 * installation : pio lib -g install 84
 * 
 * Bibliothèque : BME280 @ 3.0.0
 * installation : pio lib -g install 901
 * 
 * Bibliothèque : RadiationWatch @ 0.6.4 
 * installation : pio lib -g install 1523
 */

#include <Arduino.h>

#include <TinyGPS.h>   // GPS
#include <HardwareSerial.h>

#include <Afficheur.h>          // Afficheur SSD1306
#include <Led.h>                // Led RGB    
#include <LM75A.h>              // LM75A température intérieure
#include <Adafruit_MAX31855.h>  // Température thermocouple extérieure
#include <BME280I2C.h>          // Capteur Pression humidité température
#include <RadiationWatch.h>     // Capteur de radiation


// Les prototypes
static bool lectureGPS(unsigned long ms);
void onRadiation();
void onNoise();
void tacheRadiations(void* parameter);


TinyGPS gps;
HardwareSerial serialGps(2); // sur hardware serial 2

Afficheur *afficheur;
Led *led;
LM75A CapteurLM75A(false, false, false, -1.5); // la valeur de l'offset est déterminée par la calibration
Adafruit_MAX31855 thermocouple(4);

BME280I2C *bme;
RadiationWatch radiationWatch(32, 33);

int page;

void setup() {
    Serial.begin(115200);
    serialGps.begin(4800, SERIAL_8N1, 16, 17);
    pinMode(34, INPUT); // BP en entrée
    delay(100);

    led = new Led;
    led->allumer(RgbColor(8, 0, 0)); // rouge

    afficheur = new Afficheur;

    afficheur->afficher("Could not find bme");
    BME280I2C::Settings setBme(
            BME280::OSR_X1,
            BME280::OSR_X1,
            BME280::OSR_X1,
            BME280::Mode_Forced,
            BME280::StandbyTime_1000ms,
            BME280::Filter_Off,
            BME280::SpiEnable_False,
            BME280I2C::I2CAddr_0x77 // I2C address pour BME 280.
            );
    bme = new BME280I2C(setBme);
    while (!bme->begin()) {
        delay(1000);
    }

    radiationWatch.setup();
    radiationWatch.registerRadiationCallback(&onRadiation);
    radiationWatch.registerNoiseCallback(&onNoise);
    xTaskCreate(
            tacheRadiations, /* Task function. */
            "tacheRadiations", /* name of task. */
            10000, /* Stack size of task */
            NULL, /* parameter of the task */
            1, /* priority of the task */
            NULL); /* Task handle to keep track of created task */

    afficheur->afficher("Syn GPS");

    page = 0; // Le numéro de la page à afficher

}

void loop() {

    //radiationWatch.loop();

    if (++page == 17)
        page = 0;

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_Pa);
    float temp(NAN), hum(NAN), pres(NAN);
    bme->read(pres, temp, hum, tempUnit, presUnit);

    if (lectureGPS(1000)) {
        led->allumer(RgbColor(0, 4, 0)); // Vert
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
            case 9:
                afficheur->afficherFloat("Pression ", pres / 100, "hPa");
                break;
            case 11:
                afficheur->afficherFloat("Humidite ", hum, " %");
                break;
            case 13:
                afficheur->afficherFloat("Radiation ", radiationWatch.uSvh(), " uSvh");
                break;
            case 15:
                afficheur->afficherFloat("Radiation ", radiationWatch.cpm(), " cpm");
                break;

        }




    } else {
        afficheur->afficher("Syn GPS");
        led->allumer(RgbColor(8, 0, 0)); // Rouge 

    }

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

void onRadiation() {
    led->allumer(RgbColor(8, 8, 8)); // Blanc
}

void onNoise() {
    led->allumer(RgbColor(4, 4, 0)); // jaune
}

/**
 * Tâche périodique de 160ms pour le capteur de radiation
 * 
 * @param parameter
 */
void tacheRadiations(void* parameter) {

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        radiationWatch.loop();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(160)); // reveille toutes les 160ms
    }
}



