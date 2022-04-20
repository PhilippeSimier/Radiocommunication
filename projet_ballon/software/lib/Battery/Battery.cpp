/* 
 * File:   Battery.cpp
 * Author: philippe Simier Touchard Washington le Mans
 * 
 * Created on 12 mars 2022, 16:53
 */
#include <Arduino.h>
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
    preferences.begin("battery", false); 
}

Battery::Battery(const Battery& orig) {
}


/**
 * @brief  initialise le capteur courant tension INA219
 *         et initialise la charge initiale de la batterie
 * @return bool true si init ok
 */
bool Battery::init(){
    
    setCalibration_32V_1A(); 
    charge = preferences.getFloat("charge", 3000.0);
    bool res = begin();
    return res;
    
}

/**
 * @brief  calcul de la charge mAh (methode des trapèzes)
 * @return float la charge de la batterie
 */
float Battery::getCharge(float temp){
    
    t1 = millis();
    i1 = getCurrent_mA();
    charge += (i1 + i0) * (t1 - t0) / 7200000;
    t0 = t1;
    i0 = i1;
    
    // la charge ne peut pas être négative
    if (charge < 0.0) charge = 0.0;
    // la charge ne peut pas être supérieur à la capacité
    if (charge > capacite) charge = capacite;
    // la charge est terminé si 12.6 V  ou température négative
    if (getBusVoltage_V() > 12.6 || temp < 0.0){
       digitalWrite(13, LOW);  // Ouverture du Mosfet 
    }
    // la charge reprend si la tension est indérieur à 12.3V et température positive
    if (getBusVoltage_V() < 12.3 && temp > 1.0){
       digitalWrite(13, HIGH);  // Fermeture du Mosfet 
    }
    
    return charge; 
}

/**
 * 
 * @return float le taux de charge de la batterie en %
 */
float Battery::getSOC(){
    
    return 100 * charge/capacite;
}

/**
 * @brief  Sauvegarde la charge dans la mémoire flash
 */
void  Battery::memoriserCharge(){
    preferences.putFloat("charge", charge); 
}
