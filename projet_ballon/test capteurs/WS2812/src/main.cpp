/* 
 * File:   main.cpp
 * Author: philippe
 *
 * Created on 10 février 2022, 16:07
 * test de la LED RGB WS2812
 * Installation bibliothèque :  NeoPixelBus @ 2.6.9
 * pio lib -g install 547
 *  
 */

#include <Arduino.h>
#include <NeoPixelBus.h>

#define PIN_RGB     15
#define NUMPIXELS   1
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> led(NUMPIXELS, PIN_RGB);

RgbColor rouge(8, 0, 0);
RgbColor vert(0, 4, 0);
RgbColor jaune(4, 4, 0);
RgbColor bleu(0, 0, 8);

static void allumer(NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> &led, RgbColor couleur);

void setup() {
    led.Begin();

}

void loop() {

    allumer(led, rouge);
    delay(1000);
    allumer(led, vert);
    delay(1000);
    allumer(led, bleu);
    delay(1000);
    allumer(led, jaune);
    delay(1000);
}

static void allumer(NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> &led, RgbColor couleur) {
    led.SetPixelColor(0, couleur);
    led.Show();
}