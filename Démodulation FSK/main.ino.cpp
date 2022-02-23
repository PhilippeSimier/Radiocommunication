/*
 * Created 23/2/2022
   Anthony LE CREN f4goh@orange.fr
 * Decode AX25 message test
 * send result on Serial with KISS protocol https://en.wikipedia.org/wiki/KISS_(TNC)
 * attention :
 * ret = dra->configure(DRA_TYPE, TX_RX_FREQUENCY, TX_RX_FREQUENCY, 4, 8, 0, 0, DRA818_12K5, false, false, false);
 * desactiver les filtres internes du DRA  pre, high, low => false
 * le traitement du crc n'est pas géré
 * testé avec la carte ESP32 Ruche
 */

#include "Tnc.h"

#define ledPin 2     //led for sync oscillo


Tnc Decode;


void setup() {
  Serial.begin(115200);

  Decode.begin(ledPin);
  Serial.print("ready\n\n");
}


void loop (void)
{
}

