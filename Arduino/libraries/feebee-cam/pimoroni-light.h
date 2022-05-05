#include <light.h>
#include <Adafruit_IS31FL3731.h>

namespace FeebeeCam {

    class PimoroniLight : public Light{
    private:
        Pimoroni_IS31FL3731_5x5RGB _matrix;

    public:
        PimoroniLight() : Light() {
        }

        virtual void initialize() {
            
            if (!_matrix.begin())
                Serial.println("Error starting pimoroni matrix");
        }

        virtual void setPixels(uint8_t red, uint8_t green, uint8_t blue) {
            
            initialize();

            _matrix.setFrame(0);

            for (int x = 1; x <= 5; x++)
                for (int y = 1; y <= 5; y++)
                _matrix.drawPixel(x, y, red, green, blue);

            _matrix.displayFrame(0);
        }

        virtual void turnOn() {
            _status = true;
            setPixels(0xFF, 0x00, 0x00);
        }

        virtual void turnOff() {
            _status = false;
            setPixels(0x00, 0x00, 0x00);
        }

        virtual void flashOn() {
            setPixels(0xFF, 0xFF, 0xFF);
        }

    };
}