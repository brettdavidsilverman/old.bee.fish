#ifndef FEEBEE_CAM_LIGHT
#define FEEBEE_CAM_LIGHT
#include <Adafruit_NeoPixel.h>
#include "multiplexer.h"

// Digital IO pin connected to the Red Lights.
#define LIGHT_PIN 0
#define FLASH_PIN 1

namespace FeebeeCam {

    bool initializeLight();

    class Light {
    protected:
        bool _status = false;
        bool _flashStatus = false;
    public: 

        Light()
        {
        }

        virtual bool initialize() {
            _multiplexer.pinMode(LIGHT_PIN, OUTPUT);
            _multiplexer.pinMode(FLASH_PIN, OUTPUT);
            Serial.println("Custom light initialized");
            return true;
        }

        virtual void turnOn() {
            _multiplexer.digitalWrite(LIGHT_PIN, HIGH);
            _status = true;
        }

        virtual void turnOff() {
            _multiplexer.digitalWrite(LIGHT_PIN, LOW);
            _status = false;
        }

        virtual void flashOn() {
            _multiplexer.digitalWrite(FLASH_PIN, HIGH);
            _flashStatus = true;
        }

        virtual void flashOff() {
            _multiplexer.digitalWrite(FLASH_PIN, LOW);
            _flashStatus = false;
        }

        void toggle() {
            if (_status)
                turnOff();
            else
                turnOn();
        }

        void toggleFlash() {
            if (_flashStatus)
                flashOff();
            else
                flashOn();
        }

        bool status() {
            return _status;
        }

        bool flashStatus() {
            return _flashStatus;
        }

        void flash(int rate, int cycles) {

            for (int i = 0; i < cycles; ++i) {
                turnOn();
                delay(rate);
                turnOff();
                delay(rate);
            }
        }


    };

    extern Light* light;

}

#endif