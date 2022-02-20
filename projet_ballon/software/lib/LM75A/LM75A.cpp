/* 
 * File:   LM75A.cpp
 * Author: philippe SIMIER Touchard Washington Le Mans
 *         Quentin Comte-Gaz <quentin@comte-gaz.com>
 * 
 * Created on 11 février 2022, 16:12
 */

#include "LM75A.h"

using namespace LM75AConstValues;

/**
 * 
 * @param A0 adresse i2c
 * @param A1 adresse i2c
 * @param A2 adresse i2c
 * @paran _offset float valeur de offset en degré celsius
 * @param _scale  float valeur de l'echelle normalement 0.125 °C par unite de registre
 */
LM75A::LM75A(bool A0, bool A1, bool A2, float _offset, float _scale) :
i2c_device_address(BASE_ADDRESS),
        offset(_offset),
        scale(_scale)
{
    if (A0) i2c_device_address += 1;
    if (A1) i2c_device_address += 2;
    if (A2) i2c_device_address += 4;

    Wire.begin();
}

LM75A::~LM75A() {
}

float LM75A::getTemperature() const {
    uint16_t real_result = INVALID_LM75A_TEMPERATURE;
    uint16_t i2c_received = 0;

    // Lecture du registre de température
    Wire.beginTransmission(i2c_device_address);
    Wire.write(REG_ADDR_TEMP);
    if (Wire.endTransmission()) 
        return real_result;
    

    // Lecture du contenu du registre
    if (Wire.requestFrom(i2c_device_address, 2)) {
        Wire.readBytes((uint8_t*) & i2c_received, 2);
    } else {
        // Erreur de lecture temperature
        return real_result;
    }

    // Modify the value (only 11 MSB are relevant if swapped)
    int16_t refactored_value;
    uint8_t* ptr = (uint8_t*) & refactored_value;

    // Swap bytes
    *ptr = *((uint8_t*) & i2c_received + 1);
    *(ptr + 1) = *(uint8_t*) & i2c_received;

    // Shift data (left-aligned)
    refactored_value >>= 5;

    float real_value;
    if (refactored_value & 0x0400) {
        // When sign bit is set, set upper unused bits, then 2's complement
        refactored_value |= 0xf800;
        refactored_value = ~refactored_value + 1;
        real_value = (float) refactored_value * (-1) * scale;
    } else {
        real_value = (float) refactored_value * scale;
    }

    return real_value + offset;

}

