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
 * Méthode pour afficher la position GPS
 * @param gps
 */
void Afficheur::afficherPosition(TinyGPS &gps) {

    float flat, flon;
    unsigned long age;
    char sz[32] = "******";
    gps.f_get_position(&flat, &flon, &age);

    if (flat != TinyGPS::GPS_INVALID_F_ANGLE) {
        snprintf(sz, sizeof(sz), "%.6f\n%.6f", flat, flon);
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
void Afficheur::afficherFloat(String message, float valeur, String unite) {

    char sz[16] = "******";
    snprintf(sz, sizeof(sz), "%.1f", valeur);
    String valst(sz);
    afficher(message + valst + unite);

}

