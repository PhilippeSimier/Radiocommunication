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

void Led::allumerRouge(){
    RgbColor rouge(8, 0, 0);
    SetPixelColor(0, rouge);
    Show();
}

void Led::allumerVert(){
    RgbColor vert(0, 4, 0);
    SetPixelColor(0, vert);
    Show();
}



