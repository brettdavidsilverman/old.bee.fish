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
//            _multiplexer.selectPort(_port);
//            _strip.begin();
//            _color = 0;
        }

        void turnOn(uint8_t red = 0xFF, uint8_t green = 0xFF, uint8_t blue = 0xFF, uint8_t brightness = 255) {
    
            Serial.println("Turn On");
            _multiplexer.selectPort(_port);
            _strip.begin();

            for(int i = 0; i < _strip.numPixels(); ++i) {
                _strip.setPixelColor(i, red, green, blue);
            }

//           _strip.setBrightness(brightness);

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
            Serial.println("Turn Off");
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

    /*
    #define DATA0 PORTC &= ~(1 << PORTC3)
    #define DATA1 PORTC |= (1 << PORTC3)
    #define NOP __asm__ __volatile__ ("nop");

    void Out1()
    {
        DATA1;
        NOP;
        NOP;
        NOP;
        DATA0;
        NOP;
    }

    void Out0()
    {
        DATA1;
        NOP;
        DATA0;
        NOP;
        NOP;
    }

    void ws2812out(uint8_t data)
    {
        (data & 0x80) ? Out1() : Out0();
        (data & 0x40) ? Out1() : Out0();
        (data & 0x20) ? Out1() : Out0();
        (data & 0x10) ? Out1() : Out0();
        (data & 0x08) ? Out1() : Out0();
        (data & 0x04) ? Out1() : Out0();
        (data & 0x02) ? Out1() : Out0();
        (data & 0x01) ? Out1() : Out0();
    }
    
    You call ws2812out() for each color component.
    That is three times for each led.
    Also this is for 8 MHz I think.
    */

}