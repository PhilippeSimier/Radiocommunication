/* 
 * File:   LM75A.h
 * Author: philippe SIMIER
 *         Quentin Comte-Gaz <quentin@comte-gaz.com>
 * 
 * Created on 11 février 2022, 16:12
 */

#ifndef LM75A_H
#define LM75A_H
#define INVALID_LM75A_TEMPERATURE 1000

#include "Wire.h"

namespace LM75AConstValues {

    const int   BASE_ADDRESS = 0x48;
    const float DEGREES_RESOLUTION = 0.125;
    const int   REG_ADDR_TEMP = 0;

}

class LM75A {
public:

    LM75A(bool A0 = false, bool A1 = false, bool A2 = false);

    virtual ~LM75A();
    float getTemperature() const;

private:
    int i2c_device_address;
};

#endif /* LM75A_H */

