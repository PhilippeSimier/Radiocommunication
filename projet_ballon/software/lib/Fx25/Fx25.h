/* 
 * File:   Fx25.h
 * Author: Anthony le Cren & philippe SIMIER
 *
 * Created on 19 septembre 2021, 09:00
 */

#ifndef FX25_H
#define FX25_H

#include <Arduino.h>
#include <WString.h>
#include "../DdsI2s/DdsI2s.h"
#include <rom/crc.h>
#include "../Rs/Rs.h"

#define AX25_HEADER_SIZE 30    //longueur de l'entete pour 4 callsign (fixe)
#define AX25_CONTROL     0x03  //APRS-UI frame
#define AX25_PROTOCOL    0xF0  //Protocol Identifier
#define AX25_MAX_LENGTH  512

class Fx25 {
public:

    Fx25(float _frequence_tx_rx = 144.800);
    Fx25(const Fx25& orig);
    virtual ~Fx25();

    void begin(char *sourceCallsign,
            char *destinationCallsign,
            char *path1,
            char *path2);

    void txMessage(char *bufMsg);
    void setFec(bool val);
    void debug();

private:

    uint8_t* addCallsign(uint8_t *buf, char *callsign);
    void calculateCRC();
    bool fec;
    
    uint8_t* buffer;
    int frameLength; //longeur de la trame à envoyer
    attenuation_t attenuation;

    DdsI2s *leDdsI2s;
    Rs *leRs;

};

#endif /* FX25_H */

