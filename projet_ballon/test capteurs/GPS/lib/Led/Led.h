/* 
 * File:   Led.h
 * Author: philippe SIMIER
 *
 * Created on 11 février 2022, 09:53
 * Classe pour gérer la led RGB
 * 
 * Bibliothèque :  NeoPixelBus @ 2.6.9
 * installation :  pio lib -g install 547
 * 
 */

#ifndef LED_H
#define LED_H
#include <NeoPixelBus.h>  // Led RGB WS2812

#define PIN_RGB     15    // carte Ballon sonde APRS
#define NUMPIXELS   1

class Led : public NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> // Spécialisation de NeoPixelBus 
{
public:
    Led();
    virtual ~Led();
    
    void allumerRouge();
    void allumerVert();

private:

};

#endif /* LED_H */

