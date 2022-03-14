/* 
 * File:   Tnc.h
 * Author: ale
 *
 * VHF TNC DRA818 APRS Arduino library
      Created 23/2/2022
   Anthony LE CREN f4goh@orange.fr
   Modified
   Use this library freely
   AX25 RX routines from  Robert Marshall, KI4MCW
 */

#ifndef TNC_H
#define TNC_H
#include <Arduino.h>
#include <driver/adc.h>
#include <driver/i2s.h>
#include <driver/dac.h>
#include "soc/syscon_reg.h"
#include "soc/syscon_struct.h"



#include "DRA818.h"

#define DRA_TYPE DRA818_VHF  // or DRA818_UHF
#define TX_RX_FREQUENCY 144.800

#define NUM_SAMPLES 128
#define SAMPLING_FREQUENCY 13200
//#define DATA_MAX_LENGTH 512


#define PIN_ADC ADC1_CHANNEL_0//gpio 39

#define MIN_PACKET_LEN   10
#define PACKET_SIZE      200
#define AX25_MARK        0
#define AX25_SPACE       1
#define MAX_SYNC_ERRS    5

class Tnc {
public:
    Tnc();
    Tnc(const Tnc& orig);
    virtual ~Tnc();
    void begin(int p_led);

    
    
    unsigned char digiTX;
    unsigned char taille;
    int led;
private:
    void configureI2s();
    TaskHandle_t TaskHandle_Dac;
    void dma();
    
    DRA818  *dra;
    
    static void marshall(void *);
    

    static Tnc* anchor;
    
    void decode(int16_t rawadc);
    void decode_ax25(void);

    int16_t mult_cb[7], // circular buffer for adc*delay values
    mult_sum, // sum of mult_cb values
    bias_sum, // sum of last 128 ADC readings
    //rawadc, // value read directly from ADCH register
    adcval; // zero-biased ADC input

    uint8_t
    since_last_chg, // samples since the last MARK/SPACE symbol change
    phase_err, // symbol transition timing error, in samples
    current_symbol, // MARK or SPACE
    last_symbol, // MARK or SPACE from previous reading
    last_symbol_inaframe, // MARK or SPACE from one bit-duration ago
    inaframe, // rec'd start-of-frame marker
    bittimes, // bit durations that have elapsed
    bitqlen, // number of rec'd bits in bitq
    popbits, // number of bits to pop (drop) off the end
    byteval, // rec'd byte, read from bitq
    cb_pos, // position within the circular buffer
    msg[PACKET_SIZE + 1], // rec'd data
    msg_pos, // bytes rec'd, next array element to fill
    x, // misc counter
    test, // temp variable
    decode_state, // section of rec'd data being parsed
    bias_cnt, // number of ADC samples collected (toward goal of 128)
    adc_bias, // center-value for ADC, self-adjusting
    hb12; // heartbeat (1 or 0)

    uint8_t sync_err_cnt, // number of sync errors in this frame (so far)
    bit_timer, // countdown one bit duration
    thesebits; // number of elapsed bit durations

    int8_t adc_delay[6]; // delay line for adc readings

    uint32_t bitq; // rec'd bits awaiting grouping into bytes
    
};

#endif /* TNC_H */

