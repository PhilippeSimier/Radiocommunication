/* 
 * File:   Battery.h
 * Author: philippe
 *
 * Created on 12 mars 2022, 16:53
 */

#ifndef BATTERY_H
#define BATTERY_H

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Preferences.h>


class Battery : public Adafruit_INA219
{
public:
    Battery(const float _capacite);
    Battery(const Battery& orig);
    
    bool  init();
    float getCharge(float temp);
    float getSOC();
    void  memoriserCharge();
    
private:
    unsigned long t0;
    unsigned long t1;
    float i0;
    float i1;
    float charge;
    float capacite;
    Preferences preferences;
    

};

#endif /* BATTERY_H */

