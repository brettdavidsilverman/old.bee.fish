#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN    SCL  // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 16  // Number of NeoPixels

class Light {
private:
    Adafruit_NeoPixel _strip;

public: 
    static const uint32_t RED   = 0xFF0000;
    static const uint32_t GREEN = 0x00FF00;
    static const uint32_t BLUE  = 0x0000FF;
    static const uint32_t WHITE = 0xFFFFFF;
    static const uint32_t BLACK = 0x000000;

    // Declare our NeoPixel strip object:
    // Argument 1 = Number of pixels in NeoPixel strip
    // Argument 2 = Arduino pin number (most are valid)
    // Argument 3 = Pixel type flags, add together as needed:
    //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
    //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
    //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
    //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
    //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

    Light() : _strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ400) {
        _strip.begin();
    }

    void turnOn(uint32_t color = RED) {

        for(int i = 0; i < _strip.numPixels(); ++i) {
            _strip.setPixelColor(i, color);
        }

        _strip.show();
    }

    void turnOff() {

        for(int i = 0; i < _strip.numPixels(); ++i) {
            _strip.setPixelColor(i, BLACK);
        }
        
        _strip.show();
    }


};