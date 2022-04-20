/* 
 * File:   Fx25.cpp
 * Author: philippe
 * 
 * Created on 20 décembre 2021, 10:13
 */

#include "Fx25.h"

Fx25::Fx25() :
fec(false),
attenuation(dB_0) {
    buffer = new uint8_t[AX25_MAX_LENGTH];
    leDdsI2s = new DdsI2s();
    leDdsI2s->begin();
    leRs = new Rs();
}

Fx25::Fx25(const Fx25& orig) {
}

Fx25::~Fx25() {
    delete buffer;
    delete leDdsI2s;
    delete leRs;
}

void Fx25::setFec(bool val) {
    fec = val;
}

void Fx25::txMessage(char *bufMsg) {

    frame_t frame;

    int bufLen = strlen(bufMsg);
    memcpy(buffer + AX25_HEADER_SIZE, bufMsg, bufLen);
    frameLength = AX25_HEADER_SIZE + bufLen;
    calculateCRC();

    //si fec est vrai encapsule la trame ax25 dans une trame fec
    if (fec) {
        leRs->generate(buffer, frameLength, frame.data, &frame.nBytes, 1);
        leRs->hex_dump(frame.data, frame.nBytes);
    } else {
        frame.nBytes = leRs->stuff_it(buffer, frameLength, frame.data, DATA_MAX_LENGTH);
        leRs->hex_dump(frame.data, frame.nBytes);
    }

    frame.attenuation = attenuation;
    xQueueSend(leDdsI2s->queueDds, &frame, portMAX_DELAY);
}

/**
   @brief   Ax25::begin()
   @details Permet de créer l'entête de la trame Ax25
   @param   *sourceCallsign         indicatif de la source 
 *destinationCallsign    indicatif du destinataire
 *path1 chemin1
 *path2 chemin2
 */
void Fx25::begin(const char *sourceCallsign, const char *destinationCallsign, const char *path1, const char *path2) {
    uint8_t *ptr = buffer;

    ptr = addCallsign(buffer, destinationCallsign); // ajoute la destination dans l'header AX25
    ptr = addCallsign(ptr, sourceCallsign); // puis le sourceCallsign 
    ptr = addCallsign(ptr, path1); // le premier chemin
    ptr = addCallsign(ptr, path2); // le second  chemin
    ptr[-1] |= 1; // Marqueur fin de l'entete sur le dernier ssid

    *(ptr++) = AX25_CONTROL; //ajout ctrl + protocol
    *(ptr++) = AX25_PROTOCOL;
}

/**
   @brief   Fx25::addCallsign(uint8_t *buf, char *callsign)
   @details Ajoute un callSign (addresse) dans l'entête de la trame
   @param   *buf pointeur dans le buffer pour le positionnement du callsign
 *callsign pointeur de la chaine callsign
   @return  le pointeur courant ds le buffer pour le prochain ajout
 */

uint8_t* Fx25::addCallsign(uint8_t *buf, const char *callsign) {
    char ssid;
    char i;
    for (i = 0; i < 6; i++) {
        if (*callsign && *callsign != '-') *(buf++) = *(callsign++) << 1;
        else *(buf++) = ' ' << 1;
    }

    if (*callsign == '-') ssid = atoi(callsign + 1);
    else ssid = 0;

    *(buf++) = ('0' + ssid) << 1;
    return (buf);
}

/**
   @brief Ax25::calculateCRC()
   @details calcule le crc16 de la trame complete et ajoute le crc en fin de trame en utilisant les fonctions intégrées de la rom dans l'ESP32
 * https://github.com/espressif/esp-idf/blob/master/components/esp_rom/include/esp32/rom/crc.h
 */

void Fx25::calculateCRC() {
    uint16_t crc;
    uint8_t *s;
    s = buffer + frameLength;
    crc = crc16_le(0, buffer, frameLength);
    *(s++) = crc & 0xFF;
    *(s++) = crc >> 8;
    frameLength += 2;
}

/**
   @brief Fx25::debug()
   @details permet d'afficher la trame Ax25 !!
 */

void Fx25::debug() {
    int n;
    Serial.print("longueur de la trame : ");
    Serial.print(frameLength);
    Serial.println(" Octets");
    for (n = 0; n < AX25_HEADER_SIZE - 2; n++) {
        Serial.print(buffer[n], HEX);
        Serial.print(",");

    }
    Serial.print("\n\rHeader : ");
    for (n = 0; n < AX25_HEADER_SIZE - 2; n++) {
        Serial.print((char) (buffer[n] >> 1));
    }
    Serial.print("\n\rPDU APRS : ");
    for (n = AX25_HEADER_SIZE; n < frameLength - 2; n++) {
        Serial.print((char) buffer[n]);
    }
    Serial.print("\n\rCRC : ");
    Serial.print(buffer[frameLength - 2], HEX);
    Serial.print(',');
    Serial.println(buffer[frameLength - 1], HEX);
}

