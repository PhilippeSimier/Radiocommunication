/* 
 * File:   DdsI2s.cpp
 * Author: Anthony Le Cren & Philippe SIMIER 
 * 
 * Created on 18 août 2021, 17:33
 */

#include "DdsI2s.h"

DdsI2s::DdsI2s( dac_channel_t _dacChannel, gpio_num_t _syncLed) :
syncLed(_syncLed),
dacChannel(_dacChannel),
accumulateur(0),
frequence_tx_rx(TX_RX_FREQUENCY),        
dra(new DRA818())        
{
    anchor = this;
    pinMode(syncLed, OUTPUT);
    freq[0] = computeIncrementPhase(1200);
    freq[1] = computeIncrementPhase(2200);
    
}

void DdsI2s::marshall(void * parametres) {
    anchor->dma();
}

DdsI2s::DdsI2s(const DdsI2s& orig) {
}

DdsI2s::~DdsI2s() {
    delete dra;
}

/**
 * @brief Dds::begin()
 *
 * @details Permet d'initialiser la tache dds 
 * 
 */

void DdsI2s::begin() {
    configureI2s();
    xTaskCreatePinnedToCore(DdsI2s::marshall, "dac_i2s", 50000, &syncLed, 3, &TaskHandle_Dac, 0); // creation de la tache 
    
    uint8_t ret;
    //modelDra, freq_rx, freq_tx, squelch, vol, ctcss_rx, ctcss_tx, bandwidth, pre, high, low
    prefRadio.begin("radio", false);
    frequence_tx_rx = prefRadio.getFloat("frequence", 144.800);
    prefRadio.end();
    ret = dra->configure(DRA_TYPE, frequence_tx_rx, frequence_tx_rx, 4, 8, 0, 0, DRA818_12K5, true, true, true);
    if (!ret) {
        Serial.println("\nError while configuring DRA818");
    } else {
        Serial.println("DRA818 configuration finished");
    }
}

/**
 * @brief DdsI2s::configureI2s()
 *
 * @details Permet d'initialiser le pilote I2S avec le DMA vers le DAC
 * 
 */

void DdsI2s::configureI2s() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN), // I2S receive mode with dac
        .sample_rate = SAMPLING_FREQUENCY, // sample frequency
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // 16 bit I2S
        .channel_format  = I2S_CHANNEL_FMT_ONLY_RIGHT, // only right channel
        .communication_format = I2S_COMM_FORMAT_I2S_MSB, // MSB 1st
        .intr_alloc_flags = 0, // none
        .dma_buf_count = 4, // nb buffers DMA
        .dma_buf_len = NUM_SAMPLES, // sample size
        .use_apll = 0, // no Audio PLL
    };
    if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) == ESP_OK) { // i2s on channel 0 (no possible on channel 1 with ADC/DAC)
        i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN); //DAC ouput pin
        i2s_adc_enable(I2S_NUM_0); //dma i2s channel 0 enable for the  DAC 
    }
}

void DdsI2s::sendBit() {
    const static byte sinusTable[512] PROGMEM = {128, 129, 131, 132, 134, 135, 137, 138, 140, 141, 143, 145, 146, 148, 149, 151, 152, 154, 155, 157, 158, 160, 161, 163, 164, 166, 167, 169, 170, 172, 173, 175, 176, 178, 179, 180, 182, 183, 185, 186,
        187, 189, 190, 191, 193, 194, 195, 197, 198, 199, 201, 202, 203, 204, 206, 207, 208, 209, 210, 212, 213, 214, 215, 216, 217, 218, 219, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 230, 231, 232,
        233, 234, 235, 236, 236, 237, 238, 239, 240, 240, 241, 242, 242, 243, 244, 244, 245, 245, 246, 247, 247, 248, 248, 249, 249, 249, 250, 250, 251, 251, 251, 252, 252, 252, 253, 253, 253, 253, 254, 254,
        254, 254, 254, 254, 254, 254, 254, 254, 255, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 253, 253, 253, 253, 252, 252, 252, 251, 251, 251, 250, 250, 249, 249, 249, 248, 248, 247, 247, 246, 245,
        245, 244, 244, 243, 242, 242, 241, 240, 240, 239, 238, 237, 236, 236, 235, 234, 233, 232, 231, 230, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 219, 218, 217, 216, 215, 214, 213, 212, 210, 209,
        208, 207, 206, 204, 203, 202, 201, 199, 198, 197, 195, 194, 193, 191, 190, 189, 187, 186, 185, 183, 182, 180, 179, 178, 176, 175, 173, 172, 170, 169, 167, 166, 164, 163, 161, 160, 158, 157, 155, 154,
        152, 151, 149, 148, 146, 145, 143, 141, 140, 138, 137, 135, 134, 132, 131, 129, 127, 126, 124, 123, 121, 120, 118, 117, 115, 114, 112, 110, 109, 107, 106, 104, 103, 101, 100, 98, 97, 95, 94, 92, 91, 89,
        88, 86, 85, 83, 82, 80, 79, 77, 76, 75, 73, 72, 70, 69, 68, 66, 65, 64, 62, 61, 60, 58, 57, 56, 54, 53, 52, 51, 49, 48, 47, 46, 45, 43, 42, 41, 40, 39, 38, 37, 36, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 25, 24, 23,
        22, 21, 20, 19, 19, 18, 17, 16, 15, 15, 14, 13, 13, 12, 11, 11, 10, 10, 9, 8, 8, 7, 7, 6, 6, 6, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6,
        6, 6, 7, 7, 8, 8, 9, 10, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 36, 37, 38, 39, 40, 41, 42, 43, 45, 46, 47, 48, 49, 51, 52, 53, 54, 56, 57,
        58, 60, 61, 62, 64, 65, 66, 68, 69, 70, 72, 73, 75, 76, 77, 79, 80, 82, 83, 85, 86, 88, 89, 91, 92, 94, 95, 97, 98, 100, 101, 103, 104, 106, 107, 109, 110, 112, 114, 115, 117, 118, 120, 121, 123, 124, 126};

    uint16_t phase;
    uint16_t sinus;
    uint16_t bufferOut[NUM_SAMPLES]; //dac buffer output
    int m;
    size_t bytesWrite;
    for (m = 0; m < NUM_SAMPLES; m++) { //update bufferOut
        accumulateur += freq[flip]; // accumulateur de phase  (sur 32 bits)
        phase = (accumulateur >> 23); //ajoute la phase
        sinus = pgm_read_byte(&(sinusTable[phase])); //lecture de la valeur du sinus dans la table 
        sinus = (sinus >> attenuation) - (0x80 >> attenuation) + 0x80; //attenuation de l'amplitude la valeur moyenne reste constante
        sinus <<= 8;
        bufferOut[m ^ 1] = sinus ; //so swapp even and odd sample
    }
    i2s_write(I2S_NUM_0, (char*) bufferOut, NUM_SAMPLES * sizeof (uint16_t), &bytesWrite, portMAX_DELAY); //load dma for dac
    //digitalWrite(syncLed, digitalRead(syncLed) ^ 1);
}

void DdsI2s::flipOut() {
    
    flip ^= 1; //inversion drapeau mark <-> space
}

void DdsI2s::sendByte( uint8_t inByte, bool flag) {
    uint8_t k, bt;
    for (k = 0; k < 8; k++) {
        bt = inByte & 0x01;         //masque sur le LSB 1st
        if (bt == 0) flipOut();     //si c'est un 0 alors appel inversion mark <-> space
        inByte >>= 1; //décallage vers la droite pour le prochain bit
        sendBit(); //envoi du bit en cours
    }
}

/**
 * @brief DdsI2s::dma()
 *
 * @details tache du DDS
 * envoie une trame AX25 compléte 
 */

void DdsI2s::dma() {
        
    int i;
    frame_t frame; //commandes du dds
    
    Serial.println("Tache DDS en fonctionnement");

    while (1) {
        vTaskDelay(1); //indispensable car sinon guru ?!
        if (xQueueReceive(queueDds, &frame, portMAX_DELAY) == pdPASS) {
  
            flip = 1; //fréquence space activée
            attenuation = frame.attenuation;
            dra->ptt(1);
            i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
            sendBit();
            for (i = 0; i < AX25_PREAMBULE_LEN; i++) sendByte( AX25_PREAMBULE_BYTE, 1); //préambule de début
            for (i = 0; i < frame.nBytes; i++)       sendByte( frame.data[i], 0); //envoi du packet
            for (i = 0; i < AX25_PREAMBULE_LEN; i++) sendByte( AX25_PREAMBULE_BYTE, 1); //préambule de fin
            i2s_set_dac_mode(I2S_DAC_CHANNEL_DISABLE);
            dra->ptt(0);
        }
    }
}

/**
 * @brief Dds::computeIncrementPhase(float freq)
 *
 * @details calcul le mot utilisé par le DDS pour générer une sinusoide à fréquence donnée
 * @param   float la frequence en hz
 */

uint32_t DdsI2s::computeIncrementPhase(float freq) {
    return pow(2, 32) * freq / SAMPLING_FREQUENCY;
}

QueueHandle_t DdsI2s::queueDds = xQueueCreate(3, sizeof (frame_t));
DdsI2s* DdsI2s::anchor = NULL;

