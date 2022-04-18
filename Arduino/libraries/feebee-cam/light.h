#pragma once
#include <Adafruit_NeoPixel.h>
#include "web-request.h"
#include "multiplexer.h"

// Digital IO pin connected to the Red Lights.
#define LIGHT_PIN 0

namespace FeebeeCam {

    class Light {
    private:
        bool _status = false;
    public: 

        Light()
        {
        }

        void initialize() {
            _multiplexer.pinMode(LIGHT_PIN, OUTPUT);
        }

        void turnOn() {
            _multiplexer.digitalWrite(LIGHT_PIN, HIGH);
            _status = true;
        }

        void turnOff() {
            _multiplexer.digitalWrite(LIGHT_PIN, LOW);
            _status = false;
        }

        void toggle() {
            if (_status)
                turnOff();
            else
                turnOn();
        }

        bool status() {
            return _status;
        }

    };

    bool onLight(BeeFishWeb::WebRequest& request, WiFiClient& client);

    extern Light light;

}