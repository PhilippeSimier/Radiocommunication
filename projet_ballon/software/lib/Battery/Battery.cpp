/* 
 * File:   Battery.cpp
 * Author: philippe Simier Touchard Washington le Mans
 * 
 * Created on 12 mars 2022, 16:53
 */

#include "Battery.h"

Battery::Battery(const float _capacite) :
    Adafruit_INA219(),
    t0(0),
    t1(0),
    i0(0),
    i1(0),
    charge(0),
    capacite(_capacite)    
{
    pinMode(13, OUTPUT);     // GPIO 13 en sortie
    digitalWrite(13, HIGH);  // Fermeture du Mosfet
}

Battery::Battery(const Battery& orig) {
}



void Battery::init(const float _charge){
    
    Serial.println("Recherche de la puce INA219 !");
    while (!begin()) {
        delay(3000);
    }
    Serial.println("Puce INA219 trouvée");
    charge = _charge;
    
}

/**
 * @brief  calcul de la charge mAh (methode des trapèzes)
 * @return float la charge de la batterie
 */
float Battery::getCharge(){
    
    t1 = millis();
    i1 = getCurrent_mA();
    charge += (i1 + i0) * (t1 - t0) / 7200000;
    t0 = t1;
    i0 = i1;
    
    // la charge ne peut pas être négative
    if (charge < 0.0) charge = 0.0;
    // la charge ne peut pas être supérieur à la capacité
    if (charge > capacite) charge = capacite;
    // la charge est terminé si 12.6 V 
    if (getBusVoltage_V() > 12.6){
       digitalWrite(13, LOW);  // Ouverture du Mosfet 
    }
    if (getBusVoltage_V() < 12.3){
       digitalWrite(13, HIGH);  // Fermeture du Mosfet 
    }
    
    return charge; 
}

float Battery::getSOC(){
    
    return 100 * charge/capacite;
}
