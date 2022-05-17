/* 
 * File:   main.cpp Programme pour le ballon stratosphérique
 * Author: philippe SIMIER Touchard Washington Le Mans
 *
 * Created on 9 février 2022, 17:11
 * 
 * Bibliothèque : TinyGPS 
 * installation : pio lib -g install 416
 * 
 * Bibliothèque : NeoPixelBus @ 2.6.9
 * installation : pio lib -g install 547
 * 
 * Bibliothèque : Oled 
 * installation : pio lib -g install 2978
 * 
 * Bibliothèque : SparkFun MAX31855K Thermocouple Digitizer @ 1.0.0
 * installation : pio lib -g install 866
 * 
 * Bibliothèque : BME280 @ 3.0.0
 * installation : pio lib -g install 901
 *      Modifier la bibliothèque 901 fichier BME280.cpp
 *      ajouter return true aux methodes
 *        bool BME280::InitializeFilter() 
 *        bool BME280::WriteSettings()
 * 
 * Bibliothèque : RadiationWatch @ 0.6.4 
 * installation : pio lib -g install 1523
 * 
 * Bibliothèque : INA219 Current Sensor
 * installation : pio lib -g install 160
 * 
 * Bibliothèque : ESP32Time @ 1.0.4
 * installation : pio lib -g install 11703
 * 
 * Bibliothèque : Console
 * installation : pio lib -g install "akoro/Console@^1.2.1"
 * 
 */

#include <Arduino.h>

#include <TinyGPS.h>   // GPS
#include <HardwareSerial.h>

#include <Afficheur.h>          // Afficheur SSD1306
#include <Led.h>                // Led RGB  
#include <Battery.h>            // Battery 
#include <LM75A.h>              // LM75A température intérieure
#include <SparkFunMAX31855k.h>  // Température thermocouple extérieure
#include <BME280I2C.h>          // Capteur Pression humidité température
#include <RadiationWatch.h>     // Capteur de radiation
#include <MsdCard.h>            // Carte SD
#include <Fx25.h>               // Protocole Fx25
#include <Position.h>           // Trame Position APRS 
#include <Battery.h>            // Batterie tension courant charge SOC
#include <ESP32Time.h>          // RTC pour esp32
#include <Preferences.h>        // Classe pour écrire et lire la configuration
#include <Menu.h>               // Classe commandes via la console

#define SD_CS 5                 // Chip select SD Card
#define TM_CS 4                 // Chip select Thermocouple

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
    float tensionBat;
    float courentBat;
    float SOCBat;
    float chargeBat;

} typeData;

// Les prototypes
static bool lectureGPS(unsigned long ms);
void onRadiation();
void onNoise();
void tacheRadiations(void* parameter);

typeData data;


TinyGPS gps;
HardwareSerial serialGps(1); // GPS sur hardware serial 1

ESP32Time rtc;

Afficheur *afficheur;
Led *led;
Battery *laBatterie;
Menu *leMenu;
LM75A CapteurLM75A(false, false, false, 0); // la valeur de l'offset est déterminée par la calibration

SparkFunMAX31855k thermocouple(TM_CS, 2, 2);
MsdCard carteSD; // Avec l'affectation des broches standard de la liaison SPI SD_CS 5

BME280I2C *bme;
RadiationWatch radiationWatch(32, 33);

Fx25* fx25;
Position pos(48.010237, 0.206267, "Ballon SNIR", '/', 'O'); // icon ballon
Preferences configuration;

char ligneCSV[200];
char commentAPRS[100];
int year;
byte month, day, hundredths;
unsigned long age;
float offsetThermo = 1.0;

void setup() {

    Serial.begin(115200);
    serialGps.begin(4800, SERIAL_8N1, 17, 16); // première carte 16 17)
    pinMode(34, INPUT); // BP en entrée
    digitalWrite(2, LOW); // extinction des led sur GPIO2

    led = new Led;
    led->allumer(ROUGE); // rouge
    afficheur = new Afficheur;
    
    afficheur->afficher("Menu configuration"); 
    leMenu = new Menu;  // Menu de configuration
    leMenu->setup();
    
    afficheur->afficher("Erreur Battery"); // test de la carte battery
    laBatterie = new Battery(3000); //  instanciation d'une batterie de capacité 3000 mAh
    while (!laBatterie->init()) {
        delay(3000);
    }
    configuration.begin("battery", false); 
    float charge = configuration.getFloat("charge", 3000.0);
    afficheur->afficherFloat("charge0", charge, " mAh");
    configuration.end();
    delay(5000);
    
    afficheur->afficher("Erreur CarteSD"); // test de la carte SD
    while (!carteSD.begin()) {
        delay(1000);
    }
    
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

    afficheur->afficher("Synchro GPS"); // test du capteur GPS
    while (!lectureGPS(1000)) {
        delay(1000);
    }
    // Lecture de l'heure GPS
    gps.crack_datetime(&year,
            &month,
            &day,
            &data.heure,
            &data.minute,
            &data.seconde,
            &hundredths,
            &age);
    // Mise à date et heure de la rtc esp32 
    rtc.setTime(data.seconde, data.minute, data.heure, day, month, year);
    Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
    afficheur->afficherHeure(gps);
    delay(5000);
    afficheur->afficherDate(gps);
    delay(5000);
    // Ecriture de la première ligne du fichier csv
    carteSD.fwrite("/dataBallon.csv",
            "Time,Nb_Sat,Latitude,Longitude,Altitude,Temp_Int,Temp_BME,Temp_Ext,Pression,Humidité,Dose_uSvh,Cpm,U_batt,I_batt,Charge_batt\n");

    

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

    

    // Communication APRS sur FX25
    configuration.begin("radio", false); 
    fx25 = new Fx25();        // fx25 = new Fx25(144.200);
    
    fx25->begin(configuration.getString("srcCallsign", "F4KMN-13").c_str(), 
                configuration.getString("dstCallsign", "F4KMN-1").c_str(), 
                configuration.getString("path1", "WIDE1-1").c_str(), 
                configuration.getString("path2", "WIDE2-2").c_str());
    
    fx25->setFec(configuration.getBool("fec")); // La trame Ax25 est encapsulée dans une trame Fec
    configuration.end();
    
    afficheur->afficher("Setup done");
}

void loop() {


    bme->read(data.pression, // Lecture du capteur BME
            data.tempBME,
            data.humidite,
            BME280::TempUnit_Celsius,
            BME280::PresUnit_hPa);
    
    data.tempInt = CapteurLM75A.getTemperature(); // Lecture du capteur LM75A
    data.tempExt = thermocouple.readTempC() + offsetThermo; // Leture du thermocouple
    data.uSvh = radiationWatch.uSvh(); // Lecture de la dose de radiation
    data.cpm = radiationWatch.cpm(); // lecture du nombre de déclenchement
    data.tensionBat = laBatterie->getBusVoltage_V();
    data.courentBat = laBatterie->getCurrent_mA();
    data.chargeBat = laBatterie->getCharge(data.tempBME);
    data.SOCBat = laBatterie->getSOC();

    if (lectureGPS(990)) {
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

            case 10:
                afficheur->afficherFloat("Temp int ", data.tempInt, " °C");
                break;

            case 15:
                afficheur->afficherFloat("Temp ext ", data.tempExt, " °C");
                break;

            case 20:
                afficheur->afficherFloat("Pression ", data.pression, " hPa", 0);
                break;

            case 25:
                afficheur->afficherFloat("Humidite ", data.humidite, " %");
                break;

            case 30:
                afficheur->afficherFloat("Radiation ", data.uSvh, " uSvh", 2);
                break;

            case 35:
                afficheur->afficherFloat("Radiation ", data.cpm, " cpm");
                break;

            case 40:
                afficheur->afficherFloat("Batterie ", data.tensionBat, " V", 2);
                break;

            case 45:
                afficheur->afficherFloat("SOC ", data.SOCBat, " %");
                break;

            case 50:
                afficheur->afficherFloat("Courant ", data.courentBat, " mA");
                break;

            case 54: case 55: case 56: case 57: case 58: case 59:
                afficheur->afficherHeure(gps);
                break;

        }
        // toutes les 10 secondes log Data sur carte SD à (05 15 25 35 45 55)
        if ((data.seconde % 10) == 5) {
            snprintf(ligneCSV,
                    sizeof (ligneCSV),
                    "%02d:%02d:%02d,%d,%.6f,%.6f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n",
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
                    data.tensionBat,
                    data.courentBat,
                    data.chargeBat);
            //Serial.println(ligneCSV);
            carteSD.fputs("/dataBallon.csv", ligneCSV);

        }

        // toutes les 2 minutes transmission de la position en APRS  
        if ((data.minute % 2)==1 && (data.seconde == 57)) {

            pos.setLatitude(data.latitude);
            pos.setLongitude(data.longitude);
            pos.setAltitude(data.altitude);
            snprintf(commentAPRS,
                    sizeof (commentAPRS),
                    "%.0f %.0f %.0f %.1f %.0f %+.1f",
                    data.tempInt,
                    data.tempExt,
                    data.cpm,
                    data.tensionBat,
                    data.SOCBat,
                    data.courentBat
                    );
            pos.setComment(commentAPRS);

            fx25->txMessage(pos.getPduAprs(false)); // transmission sans compression
            Serial.println(pos.getPduAprs(false)); // Affichage dans la console du PDU aprs position

        }
        // Toutes les minutes sauvegarde de la charge batterie dans la mémoire flash
        if (!data.seconde) {
            laBatterie->memoriserCharge();
            Serial.print("Charge memorisée : ");
            Serial.print(data.chargeBat);
            Serial.println(" mAh");
        }
    } else {
        afficheur->afficher("Syn GPS");
        led->allumer(ROUGE); // Rouge 
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
    led->allumer(RgbColor(2, 2, 2)); // Blanc
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



