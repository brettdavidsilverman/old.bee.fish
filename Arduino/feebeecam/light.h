#pragma once

#include "i2c.h"
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN    SDA  // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 16  // Number of NeoPixels

namespace FeebeeCam {

    class Light {
    private:
        Adafruit_NeoPixel _strip;
        int _port;
    public:
        Light(int port = 7) :
            _strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800),
            _port(port) 
        {

        }

        void turnOn(uint8_t red = 0xFF, uint8_t green = 0xFF, uint8_t blue = 0xFF) {
            multiplexer.selectPort(_port);
            for(int i = 0; i < _strip.numPixels(); ++i) {
                _strip.setPixelColor(i, red, green, blue);
            }
            _strip.show();
        }

        void turnOff() {
            turnOn(0x00, 0x00, 0x00);
        }

    };
}
