#pragma once

#include "../mcp23008/mcp23008.h"
#include "../i2c/i2c.h"

class Light : public I2CMaster {
private:
    bool _state = false;
    void initialize();

public:
    Light();
public:

    void turnOn();
    void turnOff();
    void toggle();
    
    bool state();

};

extern Light* light;
