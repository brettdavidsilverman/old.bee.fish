#pragma once
#include <Adafruit_NeoPixel.h>
#include "web-request.h"
#include "multiplexer.h"
#include "web-server2.h"

// Digital IO pin connected to the Red Lights.
#define LIGHT_PIN 0

namespace FeebeeCam {

    bool initializeLight();

    class Light {
    protected:
        bool _status = false;
    public: 

        Light()
        {
        }

        virtual bool initialize() {
            _multiplexer.pinMode(LIGHT_PIN, OUTPUT);
            Serial.println("Custom light inittialized");
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

    bool onLight(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

    extern Light* light;

}