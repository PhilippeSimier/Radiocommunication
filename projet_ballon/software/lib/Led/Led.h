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

#define ROUGE   RgbColor(2,0,0)
#define RED 	RgbColor(2,0,0)

#define BLEU    RgbColor(0,0,2)
#define BLUE    RgbColor(0,0,2)

#define VERT    RgbColor(0,2,0)
#define GREEN   RgbColor(0,2,0)

class Led : public NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> // Spécialisation de NeoPixelBus 
{
public:
    Led();
    virtual ~Led();
    
    void allumer(const RgbColor &couleur);
    void eteindre();
  
private:

};

#endif /* LED_H */

