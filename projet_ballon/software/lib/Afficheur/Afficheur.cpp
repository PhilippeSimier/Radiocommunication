/* 
 * File:   Afficheur.cpp
 * Author: philippe SIMIER Lycée Touchard Washington
 * 
 * Created on 11 février 2022, 11:15
 */

#include "Afficheur.h"

// Constructeur de Afficheur
// Sur la carte Ballon adresse 0x3c connecté sur SDA et SCL

Afficheur::Afficheur() :
SSD1306Wire(0x3c, SDA, SCL, GEOMETRY_128_64) {
    init();
    setFont(ArialMT_Plain_24);
}

Afficheur::~Afficheur() {
}

/**
 * Méthode pour afficher un message
 * @param message String le message à afficher
 */
void Afficheur::afficher(String message) {
    clear();
    drawStringMaxWidth(0, 0, 110, message);
    display();
}

/**
 * Méthode pour afficher l'heure GPS
 * @param gps une référence sur un objet TinyGPS
 */
void Afficheur::afficherHeure(TinyGPS &gps) {

    int year;
    byte month, day, hour, minute, second, hundredths;
    unsigned long age;

    char sz[32] = "******";
    unsigned short nb = gps.satellites();
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);

    if (age != TinyGPS::GPS_INVALID_AGE) {
        snprintf(sz, sizeof (sz), "%02d:%02d:%02d  nb sat: %d", hour, minute, second, nb);
    }

    String message(sz);
    afficher(message);

}

/**
 * Méthode pour afficher la date
 * @param gps
 */
void Afficheur::afficherDate(TinyGPS &gps){
    
    int year;
    byte month, day, hour, minute, second, hundredths;
    unsigned long age;
    char mois[][12] = {"Jan", "Fev", "Mars", "Avril", "Mai", "Juin", "Juillet" , "Aout" , "Sep", "Oct", "Nov", "Dec"};

    char sz[32] = "Erreur date";
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);

    if (age != TinyGPS::GPS_INVALID_AGE && month > 0) {
        snprintf(sz, sizeof (sz), "%02d %s %04d ", day, mois[month-1], year);
    }

    String message(sz);
    afficher(message);
    
}

/**
 * Méthode pour afficher la position GPS
 * @param gps
 */
void Afficheur::afficherPosition(TinyGPS &gps) {

    float flat, flon;
    unsigned long age;
    char sz[32] = "******";
    gps.f_get_position(&flat, &flon, &age);

    if (flat != TinyGPS::GPS_INVALID_F_ANGLE) {
        snprintf(sz, sizeof(sz), "%.5f %.5f", flat, flon);
        
    }
    String message(sz);
    afficher(message);
}

/**
 * Méthode pour afficher une grandeur physique
 * @param char* message le libellé
 * @param valeur  la valeur à afficher
 * @param unite   l'unite de la grandeur
 */
void Afficheur::afficherFloat(String message, float valeur, String unite, int precision) {
    String valst(valeur,precision);
    clear();
    drawString(0,0, message );
    drawString(0,32, valst + unite);
    display();

}
