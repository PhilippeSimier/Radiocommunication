/* 
 * File:   DdsI2s.h
 * Author: Anthony Le Cren & Philippe SIMIER 
 *
 * Created on 18 août 2021, 17:33
 * 
 * 26400/1200=22  buffer DMA 1 buffer = 1 bit à 1200 bauds
 * PGCD(1200, 2200) = 200
 * Frequence d'échantillonage conseillé multiple de 200
 */

#ifndef DDSI2S_H
#define DDSI2S_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <driver/dac.h>
#include <Preferences.h>
#include "../DRA818/DRA818.h"


#define DAC_CHANNEL DAC_CHANNEL_1   //numéro du dac
#define SYNC GPIO_NUM_2             //gpio pour led
#define NUM_SAMPLES 32
#define SAMPLING_FREQUENCY 38400
#define DATA_MAX_LENGTH 512
#define AX25_PREAMBULE_BYTE 0x7E  //valeur de l'octet de préambule
#define AX25_PREAMBULE_LEN 100    //longeur du préambule en octets

#define DRA_TYPE DRA818_VHF  // or DRA818_UHF
#define TX_RX_FREQUENCY 144.800

typedef enum {
    dB_0 = 0x0, /* 1/1 Default value */
    dB_6 = 0x1, /* 1/2 */
    dB_12 = 0x2, /* 1/4 */
    dB_18 = 0x3, /* 1/8 */
    dB_24 = 0x4, /* 1/16 */
} attenuation_t;

//structure de la trame AX25 à envoyer au DMA

typedef struct {
    attenuation_t attenuation;
    uint16_t nBytes;
    uint8_t data[DATA_MAX_LENGTH];    
} frame_t;

class DdsI2s {
public:
    DdsI2s( dac_channel_t _dacChannel = DAC_CHANNEL_1,
            gpio_num_t _syncLed = GPIO_NUM_2
            );

    DdsI2s(const DdsI2s& orig);
    virtual ~DdsI2s();

    void begin();    

    static QueueHandle_t queueDds;
    
private:
    void configureI2s();
    TaskHandle_t TaskHandle_Dac;
    void dma();
    void flipOut();
    void sendBit();
    void sendByte( uint8_t inByte, bool flag);

    uint32_t computeIncrementPhase(float freq);
    static DdsI2s* anchor;
    static void marshall(void *);
    
    
    gpio_num_t      syncLed;
    dac_channel_t   dacChannel;
    uint32_t        accumulateur;
    float           frequence_tx_rx;
    uint8_t         flip;
    attenuation_t   attenuation;
    uint32_t        freq[2];
    
    DRA818          *dra;
    Preferences     prefRadio;
};

#endif /* DDSI2S_H */

