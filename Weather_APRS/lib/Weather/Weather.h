/* 
 * File:   Weather.h
 * Author: philippe
 *
 * Created on 11 août 2021, 15:40
 */

#ifndef WEATHER_H
#define WEATHER_H
#include "../Position/Position.h"
#include <cmath>   // pour avoir NAN


class Weather : public Position {

    public:
    Weather(const double _latitude, 
            const double _longitude );
    

    virtual ~Weather();
    
    void setWind(int  _wind);
    void setSpeed(double _speed);
    void setGust(double _gust);
    void setRain(double  _rain);
    void setRain24(double  _rain24);
    void setRainMid(double  _rainMid);
    void setTemp(double  _temp);
    void setHumidity(double _humidity);
    void setPressure(double _pressure);
    char* getPduAprs();

    private:
    int     wind;         // direction du vent
    double  speed;        // vitesse du vent
    double  gust;         // Pointe de vitesse
    double  temp;         // température en °F
    double  rain;         // Pluie en mm
    double  rain24;       // Pluie sur 24h
    double  rainMid;      // Pluie depuis minuit
    double  humidity;     // Humidité en %
    double  pressure;     // Pression barométrique    
        

};

#endif /* WEATHER_H */
