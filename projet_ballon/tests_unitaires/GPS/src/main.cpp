/* 
 * File:   main.cpp
 * Author: philippe SIMIER Touchard Washington Le Mans
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
 * Bibliothèque : SparkFun MAX31855K Thermocouple Digitizer @ 1.0.0
 * installation : pio lib -g install 866
 * 
 * Bibliothèque : BME280 @ 3.0.0
 * installation : pio lib -g install 901
 * 
 * Bibliothèque : RadiationWatch @ 0.6.4 
 * installation : pio lib -g install 1523
 * 
 */

#include <Arduino.h>

#include <TinyGPS.h>   // GPS
#include <HardwareSerial.h>

#include <Afficheur.h>          // Afficheur SSD1306
#include <Led.h>                // Led RGB    
#include <LM75A.h>              // LM75A température intérieure
#include <SparkFunMAX31855k.h>  // Température thermocouple extérieure
#include <BME280I2C.h>          // Capteur Pression humidité température
#include <RadiationWatch.h>     // Capteur de radiation
#include <MsdCard.h>            // Carte SD
#include <Battery.h>            // Batterie tension 

#define SD_CS 5                 //Chip select SD Card
#define TM_CS 4                 //Chip select Thermocouple

typedef struct {
    byte heure;
    byte minute;
    byte seconde;
    float latitude;
    float longitude;
    float altitude;
    unsigned short nbSat;
    float tempBME;
    float tempInt;
    float tempExt;
    float humidite;
    float pression;
    float cpm;
    float uSvh;
    float uBatt;

} typeData;

typeData data;

// Les prototypes
static bool lectureGPS(unsigned long ms);
void onRadiation();
void onNoise();
void tacheRadiations(void* parameter);


TinyGPS gps;
HardwareSerial serialGps(2); // GPS sur hardware serial 2

Afficheur *afficheur;
Led *led;
LM75A CapteurLM75A(false, false, false, 0); // la valeur de l'offset est déterminée par la calibration

SparkFunMAX31855k thermocouple(TM_CS, 2, 2);
MsdCard carteSD; // Avec l'affectation des broches standard de la liaison SPI SD_CS 5
Battery batterie(ADC1_CHANNEL_3);  // tension batterie mesurée sur ADC1_CHANNEL_3

BME280I2C *bme;
RadiationWatch radiationWatch(32, 33);

int page;
char ligneCSV[200];
int year;
byte month, day, hundredths;
unsigned long age;

void setup() {

    Serial.begin(115200);
    serialGps.begin(4800, SERIAL_8N1, 16, 17);
    pinMode(34, INPUT); // BP en entrée
    digitalWrite(2, LOW); // extinction des led sur GPIO2

    led = new Led;
    led->allumer(ROUGE); // rouge
    afficheur = new Afficheur;

    afficheur->afficher("Erreur CarteSD"); // test de la carte SD
    while (!carteSD.begin()) {
        delay(3000);
    }
    carteSD.fwrite("/dataBallon.csv",
            "Time,Nb_Sat,Latitude,Longitude,Altitude,Temp_Int,Temp_BME,Temp_Ext,Pression,Humidité,Dose_uSvh,Cpm,U_batterie\n");

    afficheur->afficher("Erreur BME280"); // test du capteur BME280
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

}

void loop() {


    bme->read(data.pression, // Lecture du capteur BME
            data.tempBME,
            data.humidite,
            BME280::TempUnit_Celsius,
            BME280::PresUnit_hPa);

    data.tempInt = CapteurLM75A.getTemperature(); // Lecture du capteur LM75A
    data.tempExt = thermocouple.readTempC(); // Leture du thermocouple
    data.uSvh = radiationWatch.uSvh(); // Lecture de la dose de radiation
    data.cpm = radiationWatch.cpm(); // lecture du nombre de déclenchement
    data.uBatt = batterie.getTension(); // lecture de la tension batterie

    if (lectureGPS(1000)) {
        led->allumer(VERT); // Vert
        data.nbSat = gps.satellites(); // Lecture du nb de satellites
        // Lecture de l'heure GPS
        gps.crack_datetime(&year,
                &month,
                &day,
                &data.heure,
                &data.minute,
                &data.seconde,
                &hundredths,
                &age);
        // Lecture de la position
        gps.f_get_position(&data.latitude, &data.longitude, &age);
        data.altitude = gps.f_altitude();

        switch (data.seconde) {
            case 0: case 1: case 2: case 3: case 4: case 5:
                afficheur->afficherHeure(gps);
                break;

            case 6:
                afficheur->afficherPosition(gps);
                break;

            case 12:
                afficheur->afficherFloat("Temp int ", data.tempInt, " °C");
                break;

            case 18:
                afficheur->afficherFloat("Temp ext ", data.tempExt, " °C");
                break;

            case 24:
                afficheur->afficherFloat("Pression ", data.pression, "hPa");
                break;

            case 30:
                afficheur->afficherFloat("Humidite ", data.humidite, " %");
                break;

            case 36:
                afficheur->afficherFloat("Radiation ", data.uSvh, " uSvh");
                break;

            case 42:
                afficheur->afficherFloat("Radiation ", data.cpm, " cpm");
                break;
                
            case 48:
                afficheur->afficherFloat("tension ", data.uBatt, " V");
                break;    
                
            case 54: case 55: case 56: case 57: case 58: case 59: 
                afficheur->afficherHeure(gps);
                break;

        }
        // toutes les 10 secondes log Data sur carte SD 
        if (!(data.seconde % 10)) {
            snprintf(ligneCSV,
                    sizeof (ligneCSV),
                    "%02d:%02d:%02d,%d,%.6f,%.6f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n",
                    data.heure,
                    data.minute,
                    data.seconde,
                    data.nbSat,
                    data.latitude,
                    data.longitude,
                    data.altitude,
                    data.tempInt,
                    data.tempBME,
                    data.tempExt,
                    data.pression,
                    data.humidite,
                    data.uSvh,
                    data.cpm,
                    data.uBatt);
            Serial.println(ligneCSV);
            carteSD.fputs("/dataBallon.csv", ligneCSV);

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



