#pragma once
#include "multiplexer.h"

namespace FeebeeCam {

    class Light {
    private:
        int _port;
        Multiplexer _multiplexer;
        int     _pixelCount;
        uint32_t _color;

    public:

        
        void begin() {
            Serial.println("Light begin");  
            pinMode(PIXEL_PIN, OUTPUT);
            reset();
            for (int pixel = 0; pixel < _pixelCount; ++pixel) {
                ws2812out(0x00);
                ws2812out(0xFF);
                ws2812out(0x00);
            }
            reset();
            Serial.println("Light ~begin()");  
        }

        void reset() {
            digitalWrite(PIXEL_PIN, LOW);
            __NOP;
            __NOP;
            __NOP;
            __NOP;
            __NOP;
            __NOP;
        }


        void Out1()
        {
            DATA1;
            __NOP;
            __NOP;
            __NOP;
            DATA0;
            __NOP;
        }

        void Out0()
        {
            DATA1;
            __NOP;
            __NOP;
            DATA0;
            __NOP;
            __NOP;
        }

        // You call ws2812out() for each color component.
        // That is three times for each led.
        // Also this is for 8 MHz I think.
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

        Light(int port = 7, int pixelCount = PIXEL_COUNT) :
            _port(port),
            _pixelCount(pixelCount)
        {
            _multiplexer.selectPort(_port);
            _color = 0;
            begin();
        }


/*
        void turnOn(uint32_t color) {

            _multiplexer.selectPort(_port);
            _strip.begin();
            for(int i = 0; i < _strip.pixelCount(); ++i) {
                _strip.setPixelColor(i, color);
            }

            _strip.show();

            _color = color;
        }
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
*/
    };

//    bool onLight(BeeFishWeb::WebRequest& request, WiFiClient& client);

}
