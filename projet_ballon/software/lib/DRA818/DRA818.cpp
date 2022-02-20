/**
    @file DRA818.cpp
    @author f4goh - kb1goh
    @date 25 06 2020
    @DRA818 configuration control with AT commands
 */



#include "DRA818.h"

#define LOG(action, msg) Serial.action(msg)
#define CHECK(a, op, b) if (a op b) a = b
#define SEND(msg) serial->write(msg)

DRA818::DRA818(gpio_num_t _rxPin, gpio_num_t _txPin, gpio_num_t _pdPin, gpio_num_t _pttPin) :
rxPin(_rxPin),
txPin(_txPin),
pdPin(_pdPin),
pttPin(_pttPin) {
    serial = new HardwareSerial(SERIAL_HW_NUMBER);
    rxPin = _rxPin;
    txPin = _txPin;
    pdPin = _pdPin;
    pttPin = _pttPin;
    pinMode(pdPin, OUTPUT);
    sleep(NORMAL_MODE);
    pinMode(pttPin, OUTPUT);
    ptt(0);    
}

uint8_t DRA818::configure(modelDra _model, float freq_rx, float freq_tx, uint8_t squelch, uint8_t vol, uint8_t ctcss_rx, uint8_t ctcss_tx, bwDra bandwidth, bool pre, bool high, bool low) {
    uint8_t ret;
    serial->begin(SERIAL_SPEED, SERIAL_8N1, rxPin, txPin);
    model = _model;
    handshake();
    ret = group(bandwidth, freq_tx, freq_rx, ctcss_tx, squelch, ctcss_tx);
    if (ret) ret = volume(vol);
    if (ret) ret = filters(pre, high, low);
    return ret;
}

uint8_t DRA818::read_response() {
    char ack[3];
    ack[2] = 0;
    long start = millis();
    do {
        if (this->serial->available()) {
            ack[0] = ack[1];
            ack[1] = ack[2];
            ack[2] = serial->read();
            LOG(write, ack[2]);
        }
    } while (ack[2] != 0xa && (millis() - start) < TIMEOUT);
    //if (ack[2] != 0xa) LOG(write, F("\r\n"));
    LOG(print, F("Returned value="));
    LOG(println, ack[0] == '0');
    return (ack[0] == '0');
}

uint8_t DRA818::group(bwDra bw, float freq_tx, float freq_rx, uint8_t ctcss_tx, uint8_t squelch, uint8_t ctcss_rx) {
    char buffer[49];
    char buf_rx[9];
    char buf_tx[9];

    CHECK(bw, <, DRA818_12K5);
    CHECK(bw, >, DRA818_25K);

    CHECK(freq_rx, <, (model == DRA818_VHF ? DRA818_VHF_MIN : DRA818_UHF_MIN));
    CHECK(freq_tx, <, (model == DRA818_VHF ? DRA818_VHF_MIN : DRA818_UHF_MIN));

    CHECK(freq_rx, >, (model == DRA818_VHF ? DRA818_VHF_MAX : DRA818_UHF_MAX));
    CHECK(freq_tx, >, (model == DRA818_VHF ? DRA818_VHF_MAX : DRA818_UHF_MAX));

    CHECK(ctcss_rx, >, CTCSS_MAX);
    CHECK(ctcss_tx, >, CTCSS_MAX);

    CHECK(squelch, >, SQUELCH_MAX);

    dtostrf(freq_tx, 8, 4, buf_rx);
    dtostrf(freq_rx, 8, 4, buf_tx);

    sprintf(buffer, "AT+DMOSETGROUP=%01d,%s,%s,%04d,%c,%04d\r\n", bw, buf_tx, buf_rx, ctcss_tx, squelch + '0', ctcss_rx);

    LOG(println, F("DRA818::group"));
    LOG(print, F("-> "));
    SEND(buffer);

    return read_response();
}

uint8_t DRA818::handshake() {
    char i = HANDSHAKE_REPEAT;

    while (i-- > 0) {
        LOG(println, F("DRA818::handshake"));
        LOG(print, F("-> "));

        SEND("AT+DMOCONNECT\r\n");
        if (read_response()) {
            return 1;
        }
    }
    return 0;
}

uint8_t DRA818::scan(float freq) {
    char buf[9];

    dtostrf(freq, 8, 4, buf);

    LOG(println, F("DRA818::scan"));
    LOG(print, F("-> "));

    SEND("S+");
    SEND(buf);
    SEND("\r\n");

    return read_response();
}

uint8_t DRA818::volume(uint8_t volume) {
    CHECK(volume, >, VOLUME_MAX);
    CHECK(volume, <, VOLUME_MIN);

    LOG(println, F("DRA818::volume"));
    LOG(print, F("-> "));

    SEND("AT+DMOSETVOLUME=");
    SEND(volume + '0');
    SEND("\r\n");

    return read_response();
}

uint8_t DRA818::filters(bool pre, bool high, bool low) {
    LOG(println, F("DRA818::filters"));
    LOG(print, F("-> "));

    SEND("AT+SETFILTER=");
    SEND('0' + (char) pre);
    SEND(",");
    SEND('0' + (char) high);
    SEND(",");
    SEND('0' + (char) low);
    SEND("\r\n");

    return read_response(); // SCAN function return 0 if there is a signal, 1 otherwise
}

void DRA818::sleep(uint8_t mode) {
    digitalWrite(pdPin, mode);
    if (mode == NORMAL_MODE) {      //laisse le temps au dra de se réveiller
        delay(INIT_TIME);
    }
}

void DRA818::ptt(uint8_t p) {
    digitalWrite(pttPin, p);        
    delay(PTT_SWITCHING_TIME);           //attends avant d'envoyer la modulation
}


