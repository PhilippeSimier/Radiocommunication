/* 
 * File:   Led.cpp
 * Author: philippe
 * 
 * Created on 11 février 2022, 09:53
 */

#include "Led.h"



// Constructeur de Led
Led::Led(): 
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod>(NUMPIXELS, PIN_RGB)       
{
    Begin();
    
}

Led::~Led() {
}

/**
 * Allume la led avec la couleur demandée
 * @param couleur RgbColor une couleur 
 */

void Led::allumer(const RgbColor &couleur){
    SetPixelColor(0, couleur);
    Show();
}

void Led::eteindre(){
    SetPixelColor(0, RgbColor(0,0,0));
    Show();
}



