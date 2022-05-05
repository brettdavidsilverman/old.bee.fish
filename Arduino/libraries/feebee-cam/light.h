#pragma once
#include <Adafruit_NeoPixel.h>
#include "web-request.h"
#include "multiplexer.h"

// Digital IO pin connected to the Red Lights.
#define LIGHT_PIN 0

namespace FeebeeCam {

    class Light {
    protected:
        bool _status = false;
    public: 

        Light()
        {
        }

        virtual void initialize() {
            _multiplexer.pinMode(LIGHT_PIN, OUTPUT);
        }

        virtual void turnOn() {
            _multiplexer.digitalWrite(LIGHT_PIN, HIGH);
            _status = true;
        }

        virtual void turnOff() {
            _multiplexer.digitalWrite(LIGHT_PIN, LOW);
            _status = false;
        }

        void toggle() {
            if (_status)
                turnOff();
            else
                turnOn();
        }

        virtual void flashOn() {
            turnOn();
        }

        virtual void flashOff() {
            turnOff();
        }

        bool status() {
            return _status;
        }

    };

    bool onLight(BeeFishWeb::WebRequest& request, WiFiClient& client);

    extern Light& light;

}