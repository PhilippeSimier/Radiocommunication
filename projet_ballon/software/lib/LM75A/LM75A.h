/* 
 * File:   LM75A.h
 * Authors: Philippe SIMIER Touchard Washington 
 *          Quentin Comte-Gaz <quentin@comte-gaz.com>
 * 
 * Created on 11 février 2022, 16:12
 * Ajout des attributs pour la calibration du capteur
 */

#ifndef LM75A_H
#define LM75A_H
#define INVALID_LM75A_TEMPERATURE 1000

#include "Wire.h"

namespace LM75AConstValues {

    const int BASE_ADDRESS = 0x48;
    const int REG_ADDR_TEMP = 0;

}

class LM75A {
public:

    LM75A(  bool A0 = false,
            bool A1 = false,
            bool A2 = false,
            float _offset = 0.0,
            float _scale = 0.125);

    virtual ~LM75A();
    float getTemperature() const;

private:
    int i2c_device_address;
    float offset;
    float scale;
};

#endif /* LM75A_H */

