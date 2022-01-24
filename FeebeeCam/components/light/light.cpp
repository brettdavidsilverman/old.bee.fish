
#include <bee-fish.h>
#include "light.h"

static const char* TAG = "SimpleLight";

#define MCP23008_IODIR      0x00
#define MCP23008_IPOL       0x01
#define MCP23008_GPINTEN    0x02
#define MCP23008_DEFVAL     0x03
#define MCP23008_INTCON     0x04
#define MCP23008_IOCON      0x05
#define MCP23008_GPPU       0x06
#define MCP23008_INTF       0x07
#define MCP23008_INTCAP     0x08
#define MCP23008_GPIO       0x09
#define MCP23008_OLAT       0x0A

Light* light = nullptr;

SimpleLight::SimpleLight() : I2CMaster(1, 0x20) {

}

void SimpleLight::initialize() {

    writeRegister(MCP23008_IODIR, 0b00000000);
    writeRegister(MCP23008_GPIO,  0b00000000);

    Light::initialize();

    INFO(TAG,  "SimpleLight initialized");
}

void SimpleLight::turnOn() {

    writeRegister(MCP23008_GPIO, 0b00000001);

    uint8_t gpio = 0;
    readRegister(MCP23008_GPIO, &gpio, 1);

    if (gpio == 0b00000001) {
        INFO(TAG,  "Correctly read register");
    }
    else {
        INFO(TAG,  "Error reading register");
    }

    INFO(TAG,  "SimpleLight turned on");

    Light::turnOn();
}


void SimpleLight::turnOff() {

    writeRegister(MCP23008_GPIO, 0b00000000);
    
    INFO(TAG,  "SimpleLight turned off");

    Light::turnOff();
}

void Light::toggle() {
    if (_state)
        turnOff();
    else
        turnOn();
}

bool Light::state() {
    return _state;
}

void Light::show() {
    if (_state)
        turnOn();
    else
        turnOff();
}