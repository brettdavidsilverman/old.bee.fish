#pragma once

#include "i2c.h"
#include <mcp23008.h>

class Light : private I2CMaster {
private:
    bool _state = false;
    mcp23008_t _mcp23008;

public:
    Light();
public:
    void turnOn();
    void turnOff();
    void toggle();
    
    bool state();

};

extern Light* light;
