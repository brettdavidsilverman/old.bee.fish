#ifndef FEEBEE_CAM_LIGHT
#define FEEBEE_CAM_LIGHT

#include <iostream>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_MCP23008.h"
#include "two-wire.h"

#include "web-client.h"

// Digital IO pin connected to the Red Lights.
#define LIGHT_PIN 0
#define FLASH_PIN 1

namespace FeebeeCam {

    bool initializeLight();
    bool onLight(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

    class Light {
    protected:
        bool _status = false;
        bool _flashStatus = false;
        Adafruit_MCP23008 _multiplexer;
    public: 

        Light() {
            
            using namespace std;

            if (initialize())
                cerr << "Ok";
            else
                cerr << "Error";
            
            cerr << endl;
        }

        bool initialize() {

            cerr << "Initializing light..." << flush;

            if (!FeebeeCam::initializeTwoWire(0, SDA, SCL))
                return false;

            if (!_multiplexer.begin(0x20, twoWire)) {
                cerr << "Multiplexer not found" << endl;
                return false;
            }

            _multiplexer.pinMode(LIGHT_PIN, OUTPUT);
            _multiplexer.pinMode(FLASH_PIN, OUTPUT);

            _status      = _multiplexer.digitalRead(LIGHT_PIN);
            _flashStatus = _multiplexer.digitalRead(FLASH_PIN);

            cerr << "Ok" << endl;

            return true;
        }

        virtual void turnOn() {
            _multiplexer.digitalWrite(LIGHT_PIN, HIGH);
            _status = true;
        }

        virtual void turnOff() {
            std::cerr << "Turning lights off " << std::flush;
            _multiplexer.digitalWrite(LIGHT_PIN, LOW);
            _multiplexer.digitalWrite(FLASH_PIN, LOW);
            _status = false;
            std::cerr << "OK" << std::endl;
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

}

#endif