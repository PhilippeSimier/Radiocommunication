/* 
 * File:   Weather.cpp
 * Author: philippe S (Touchard Washington)
 * 
 * Created on 11 août 2021, 15:40
 */


#include <Print.h>

#include "Weather.h"

Weather::Weather(const double _latitude, const double _longitude) :
Position(_latitude, _longitude, "", '/', '_'),
wind(-1),
speed(NAN),
gust(NAN),
temp(NAN),
rain(NAN),
rain24(NAN),
rainMid(NAN),
humidity(NAN),
pressure(NAN) {
}

Weather::~Weather() {
}

void Weather::setWind(int  _wind){
    wind = _wind;    
}

/**
 * 
 * @param _speed  vitesse du vent en m/s
 */
void Weather::setSpeed(double _speed){
    speed = 2.2369 * _speed;     // 1 m/s = 2.2369 mph
}

void Weather::setGust(double _gust){
    gust = 2.2369 * _gust;
}

void Weather::setTemp(double _temp) {
    temp = _temp * 9 / 5 + 32; // °F = °C x 9/5 + 32
}
/**
 * 
 * @param _rain     rain in last hour  (mm)
 */
void Weather::setRain(double  _rain){
    rain = _rain * 3.937;   // 1mm = 3.937 centièmes de pouce
}

void Weather::setRain24(double  _rain24){
    rain24 = _rain24 * 3.937;   // 1mm = 3.937 centièmes de pouce
}

void Weather::setRainMid(double  _rainMid){
    rainMid = _rainMid * 3.937;   // 1mm = 3.937 centièmes de pouce
}

void Weather::setHumidity(double _humidity){
    humidity = _humidity;
}

void Weather::setPressure(double _pressure){
    pressure = _pressure * 10;
}

char* Weather::getPduAprs() {

    char data[100];
    char swind[4]     = "...";
    char sspeed[4]    = "...";
    char sgust[5]     = "g...";
    char stemp[5]     = "t...";
    char srain[5]     = "r...";
    char srain24[5]   = "p...";
    char srainMid[5]  = "P...";
    char shumidity[4] = "h..";
    char spressure[7] = "b.....";
    
    if (wind != -1 )      snprintf(swind,     sizeof (swind),     "%03d",     wind);
    if (!std::isnan(speed))    snprintf(sspeed,    sizeof (sspeed),    "%03.0f",  speed);
    if (!std::isnan(gust))     snprintf(sgust,     sizeof (sgust),     "g%03.0f", gust);
    if (!std::isnan(temp))     snprintf(stemp,     sizeof (stemp),     "t%03.0f", temp);
    if (!std::isnan(rain))     snprintf(srain,     sizeof (srain),     "r%03.0f", rain);
    if (!std::isnan(rain24))   snprintf(srain24,   sizeof (srain24),   "p%03.0f", rain24);
    if (!std::isnan(rainMid))  snprintf(srainMid,  sizeof (srainMid),  "P%03.0f", rainMid);
    if (!std::isnan(humidity)) snprintf(shumidity, sizeof (shumidity), "h%02.0f", humidity);
    if (!std::isnan(pressure)) snprintf(spressure, sizeof (spressure), "b%05.0f", pressure);
    
    snprintf(data, sizeof (data), "%s/%s%s%s%s%s%s%s%s", 
            swind, sspeed, sgust, stemp, srain, srain24, srainMid, shumidity, spressure);

    latitude_to_str();
    longitude_to_str();
    snprintf(pdu, sizeof (pdu), "!%s%c%s%c%s", slat, symboleTable, slong, symbole, data);

    return pdu;
}

