#pragma once
#include <bee-fish.h>
#include <Adafruit_NeoPixel.h>
#include "web-request.h"
#include "multiplexer.h"

#define PIXEL_PIN    SDA  // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 16  // Number of NeoPixels

namespace FeebeeCam {

    class Light {
    private:
        Adafruit_NeoPixel _strip;
        int _port;
        Multiplexer _multiplexer;
        uint32_t _color;

    public:

    public: 
        // Declare our NeoPixel strip object:
        // Argument 1 = Number of pixels in NeoPixel strip
        // Argument 2 = Arduino pin number (most are valid)
        // Argument 3 = Pixel type flags, add together as needed:
        //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
        //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
        //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
        //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
        //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

        Light(int port = 7) :
            _strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800), 
            _port(port)
        {
            _multiplexer.selectPort(_port);
            _strip.begin();
            _color = 0;
        }

        void turnOn(uint8_t red = 0xFF, uint8_t green = 0xFF, uint8_t blue = 0xFF, uint8_t brightness = 255) {
    
            _multiplexer.selectPort(_port);
            _strip.begin();

            for(int i = 0; i < _strip.numPixels(); ++i) {
                _strip.setPixelColor(i, red, green, blue);
            }

            _strip.setBrightness(brightness);

            _strip.show();
            
            _color = Adafruit_NeoPixel::Color(red, green, blue);
        }
/*
        void turnOn(uint32_t color) {

            _multiplexer.selectPort(_port);
            _strip.begin();
            for(int i = 0; i < _strip.numPixels(); ++i) {
                _strip.setPixelColor(i, color);
            }

            _strip.show();

            _color = color;
        }
*/
        void setBrightness(uint8_t brightness) {
            _multiplexer.selectPort(_port);
            _strip.begin();
            _strip.setBrightness(brightness);
            _strip.show();
        }

        void turnOff() {
            _multiplexer.selectPort(_port);
            _strip.begin();
            _strip.clear();
            _strip.show();
        }

        void rainbow(uint16_t firstHue = 0) {
            _multiplexer.selectPort(_port);
            _strip.begin();
            _strip.rainbow(firstHue);
            _strip.show();
        }

        uint32_t color() {
            return _color;
        }

    };

    bool onLight(BeeFishWeb::WebRequest& request, WiFiClient& client);
}