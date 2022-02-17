#include <Adafruit_NeoPixel.h>
#include <DFRobot_I2CMultiplexer.h>

#define PIXEL_PIN    SDA  // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 16  // Number of NeoPixels

#define PORT 7

class Light {
private:
    Adafruit_NeoPixel _strip;
    TwoWire _wire;
    DFRobot_I2CMultiplexer _multiplexer;

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

    Light() : _strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800), 
        _wire(0),
        _multiplexer(0x70, &_wire) 
    {
        _wire.begin(SDA, SCL);
        _multiplexer.selectPort(PORT);
        _strip.begin();
    }

    void turnOn(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness) {
  
        _multiplexer.selectPort(PORT);
        _strip.begin();

        for(int i = 0; i < _strip.numPixels(); ++i) {
            _strip.setPixelColor(i, red, green, blue);
        }

        _strip.setBrightness(brightness);

        _strip.show();
        
    }

    void turnOn(uint32_t color = RED) {

        _multiplexer.selectPort(PORT);
        _strip.begin();
        for(int i = 0; i < _strip.numPixels(); ++i) {
            _strip.setPixelColor(i, color);
        }

        _strip.show();
    }

    void setBrightness(uint8_t brightness) {
        _multiplexer.selectPort(PORT);
        _strip.begin();
        _strip.setBrightness(brightness);
        _strip.show();
    }

    void turnOff() {
        _multiplexer.selectPort(PORT);
        _strip.begin();
        _strip.clear();
        _strip.show();
    }

    void rainbow(uint16_t firstHue = 0) {
        _multiplexer.selectPort(PORT);
        _strip.begin();
        _strip.rainbow(firstHue);
        _strip.show();
    }

};