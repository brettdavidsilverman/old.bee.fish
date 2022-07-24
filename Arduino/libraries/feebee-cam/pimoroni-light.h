#include <light.h>
#include <Adafruit_IS31FL3731.h>

namespace FeebeeCam {

    class Pimoroni_IS31FL3731_5x5RGB : public Adafruit_IS31FL3731
    {
    public:
        Pimoroni_IS31FL3731_5x5RGB(void)
        : Adafruit_IS31FL3731(5, 5) {}
        
        void _swap_int16_t(int16_t& a, int16_t&b) {
            int16_t t = a;
            a = b;
            b = t;
        }

        void drawPixel(int16_t x, int16_t y, uint16_t color)
        {
            // check rotation, move pixel around if necessary
            switch (getRotation())
            {
                case 1:
                    _swap_int16_t(x, y);
                    x = 5 - x - 1;
                    break;
                case 2:
                    x = 5 - x - 1;
                    y = 7 - y - 1;
                    break;
                case 3:
                    _swap_int16_t(x, y);
                    y = 9 - y - 1;
                    break;
            }

            // charlie wing is smaller:
            if ((x < 0) || (x >= 16) || (y < 0) || (y >= 7))
                return;

            if (x > 7)
            {
                x = 5 - x;
                y += 8;
            }
            else
            {
                y = 7 - y;
            }

            _swap_int16_t(x, y);

            if (color > 255)
                color = 255; // PWM 8bit max

            setLEDPWM(x + y * 5, color, _frame);
            return;
        }
    };

    class PimoroniLight : public Light{
    private:
        Pimoroni_IS31FL3731_5x5RGB _matrix;

    public:
        PimoroniLight() : 
            Light()
        {
        }

        virtual bool initialize() {
            
            if (_matrix.begin()) {
                Serial.println("Pimoroni light initialized");
                return true;
            }
            else {
                Serial.println("Error starting pimoroni matrix");
                return false;
            }

        }

        virtual void setPixels(uint16_t color) {
            
            initialize();

            _matrix.setFrame(0);

            for (int x = 1; x <= 5; x++)
                for (int y = 1; y <= 5; y++)
                    _matrix.setLEDPWM(x + y * 5, color, 0);
                    //_matrix.drawPixel(x, y, color);

            _matrix.displayFrame(0);
        }

        virtual void turnOn() {
            setPixels(0xF800);
            Light::turnOn();
        }

        virtual void turnOff() {
            setPixels(0x0000);
            Light::turnOff();
        }

        virtual void flashOn() {
            setPixels(0xFFFF);
            Light::flashOn();
        }

        virtual void flashOff() {
            setPixels(0x0000);
            Light::flashOff();
        }
    };
}