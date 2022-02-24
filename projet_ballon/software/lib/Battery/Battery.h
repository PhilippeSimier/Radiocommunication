/* 
 * File:   Battery.h
 * Author: philippe SIMIER Touchard Washington (Le Mans)
 *
 * Created on 22 février 2022, 10:41
 */

#ifndef BATTERY_H
#define BATTERY_H

#include <driver/adc.h>
#include <Arduino.h>

class Battery {
    
public:
    
    Battery(adc1_channel_t _channel = ADC1_CHANNEL_3, int _v10 = 2305, int _v1 = 93);   // Entrée analogique tension batterie gpio 39 sur la carte ballon
    virtual ~Battery();
    float getTension();
    
private:
    
    adc1_channel_t channel;   // Le canal utilisé pour l'entrée analogique
    int v10;                 // la valeur lu pour une tension de 10 V 
    int v1;                  // la valeur lu pour une tension de 1 V
};

#endif /* BATTERY_H */

