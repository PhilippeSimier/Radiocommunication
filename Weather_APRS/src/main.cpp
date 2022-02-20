#include "../lib/Fx25/Fx25.h"
#include "../lib/Position/Position.h"
#include "../lib/Weather/Weather.h"

Fx25* fx25;

Position p2(48.010237, 0.206267, "test unitaire p2", '/', '['); // icon Human
Weather  w3(48.010237, 0.206267);                                // a Weather station

void setup() {
    Serial.begin(115200);
    fx25 = new Fx25();
    char srcCallsign[] = "F1ZMM-5";
    char dstCallsign[] = "F1ZMM-2";
    char path1[] = "WIDE1-1";
    char path2[] = "WIDE2-2";
    fx25->begin(srcCallsign, dstCallsign, path1, path2);
    fx25->setFec(true);  // La trame Ax25 est encapsulée dans une trame Fec

}

void loop() {

    char c;
    char trameAPRS[] = "!4753.41N/00016.61E>test";
    
    if (Serial.available() > 0) {
        c = Serial.read();
        switch (c) {
            case 'f':
                
                fx25->txMessage(trameAPRS);                
                break;
                
            case 'p':
                
                Serial.println("Position p2 trame compressée avec altitude");
                p2.setAltitude(80);
                fx25->txMessage(p2.getPduAprs(true));
                break;
                
            case 'w':
                Serial.println("Test weather report");
                w3.setWind(90);          // Direction du vent 
                w3.setSpeed(2.5);        // Vitesse moyenne du vent en m/s
                //w3.setGust(5.1);         // Vitesse en rafale en m/s
                w3.setTemp(17.5);        // Température en degré celsius
                w3.setRain(0);           // Pluie en mm/1h
                w3.setRain24(0);         // Pluie en mm/24h
                w3.setRainMid(0);        // Pluie en mm/depuit minuit
                w3.setHumidity(60);      // Hmidité relative en %
                w3.setPressure(1008.5);  // Pression barométrique en hPa
                
                fx25->txMessage(w3.getPduAprs());
                break;    
        }
    }
}

