/**
    @file DRA818.h
    @author f4goh - kb1goh
    @date 25 06 2020
    @DRA818 configuration control with AT commands
*/

#ifndef __DRA818__
#define __DRA818__


#include "Arduino.h"
#include <HardwareSerial.h>

#define RXD2 GPIO_NUM_27
#define TXD2 GPIO_NUM_26
#define PD GPIO_NUM_12
#define PTT GPIO_NUM_14

#define SERIAL_HW_NUMBER 2  //attention a cette valeur 1 ou 2 suivant la liaison série


#define DRA818_VHF_MIN    134.0
#define DRA818_VHF_MAX    174.0
#define DRA818_UHF_MIN    400.0
#define DRA818_UHF_MAX    470.0

#define CTCSS_MAX           38
#define SQUELCH_MAX         8
#define VOLUME_MAX          8
#define VOLUME_MIN          1
#define TIMEOUT             2000
#define HANDSHAKE_REPEAT    3
#define SERIAL_SPEED        9600
#define SLEEP_MODE  LOW
#define NORMAL_MODE  HIGH
#define PTT_SWITCHING_TIME  200
#define INIT_TIME 500


enum modelDra
{
  DRA818_VHF,
  DRA818_UHF
};

enum bwDra
{
  DRA818_12K5,
  DRA818_25K
};


class DRA818
{
  public:
    DRA818 (gpio_num_t _rxPin=RXD2, gpio_num_t _txPin=TXD2, gpio_num_t _pdPin=PD, gpio_num_t _pttPin=PTT);
    uint8_t configure(modelDra _model, float freq_rx, float freq_tx, uint8_t squelch, uint8_t vol, uint8_t ctcss_rx, uint8_t ctcss_tx, bwDra bandwidth, bool pre, bool high, bool low);

    // low level DRA818 function
    uint8_t group(bwDra bandwidth, float freq_tx, float freq_rx, uint8_t ctcss_tx, uint8_t squelch, uint8_t ctcss_rx);
    uint8_t handshake();
    uint8_t scan(float freq);
    uint8_t volume(uint8_t volume);
    uint8_t filters(bool pre, bool high, bool low);
    void sleep(uint8_t mode);
    void ptt(uint8_t p);
    
  private:
    gpio_num_t rxPin, txPin, pdPin, pttPin;
    modelDra model;
    HardwareSerial *serial;
    uint8_t read_response();

};

#endif

