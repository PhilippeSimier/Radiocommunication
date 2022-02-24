/* 
 * File:   Battery.cpp
 * Author: philippe SIMIER Touchard Washington (Le Mans)
 * 
 * Created on 22 février 2022, 10:42
 */

#include "Battery.h"

Battery::Battery(adc1_channel_t _channel, int _v10, int _v1) :
channel(_channel),
v10(_v10),
v1(_v1)        
{

    adc1_config_width(ADC_WIDTH_BIT_12);   // de 12 bits de 0 à 4096
    adc1_config_channel_atten(_channel, ADC_ATTEN_DB_11);  // de 0 à 3V
}

Battery::~Battery() {
}

/**
 * @brief  Méthode pour obtenir la tension de la batterie
 * @return float la tension en V de la batterie
 */
float Battery::getTension(){
    
    int value = adc1_get_raw(channel);
    Serial.println(value);
    value = map(value, v1, v10, 100, 1000);   // pont diviseur  12K + 3.3K
    return (float) value/100;
}

