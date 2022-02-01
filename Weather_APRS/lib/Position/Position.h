/* 
 * File:   Position.h
 * Author: philippe
 *
 * Created on 7 août 2021, 08:00
 */

#ifndef POSITION_H
#define POSITION_H

#include <WString.h>
#include <stdio.h>
#include <math.h>


class Position {
public:
    
    Position(const double _latitude, 
             const double _longitude, 
             String _comment = "", 
             const char _symboleTable = '/',
             const char _symbole= '>'
             );
    
    Position(const Position& orig);
    virtual ~Position();
    
    char* getPduAprs(bool compressed = false);
    void setLatitude(const double _latitude);
    void setLongitude(const double _longitude);
    void setComment(String _comment);
    void setAltitude(const double _alt);
    
protected:
    void latitude_to_str();
    void longitude_to_str();
    void convBase91(int x, char* base91);
    char    slat[9];
    char    slong[10];
    double  latitude;   
    double  longitude;
    char    symboleTable;
    char    symbole;
    char    pdu[100];
    
private:

    int     alt;
    String  comment;
   
    void latitude_to_comp_str();
    void longitude_to_comp_str();    
};

#endif /* POSITION_H */

