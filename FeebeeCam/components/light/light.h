#pragma once

#include "../mcp23008/mcp23008.h"
#include "../i2c/i2c.h"

class Light : public I2CMaster {
protected:
    bool _state = false;

public:
    Light();
    virtual ~Light() {}
    
public:

    virtual void initialize();
    virtual void turnOn();
    virtual void turnOff();
    
    virtual void flashOn() {turnOn();}
    virtual void flashOff() {turnOff();}

    void toggle();
    
    bool state();

    virtual void show();

};

extern Light* light;

Light* createLight();