#pragma once

#include "../mcp23008/mcp23008.h"
#include "../i2c/i2c.h"


class Light {
protected:
    bool _state = false;

public:
    Light() {};
    virtual ~Light() {}
    
public:

    virtual void initialize() {_state = false;}
    virtual void turnOn() {_state = true;}
    virtual void turnOff() {_state = false;}
    
    virtual void flashOn() {turnOn();}
    virtual void flashOff() {turnOff();}

    void toggle();
    
    bool state();

    virtual void show();

};

class SimpleLight : public I2CMaster, public Light {
public:
    SimpleLight();
    virtual void initialize();
    virtual void turnOn();
    virtual void turnOff();
};

extern Light* light;

Light* createLight();