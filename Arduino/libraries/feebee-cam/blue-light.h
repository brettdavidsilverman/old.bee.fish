#ifndef FEEBEE_CAM_BLUE_LIGHT
#define FEEBEE_CAM_BLUE_LIGHT

#include "light.h"

// Digital IO pin connected to the Red Lights.
#define BLUE_LIGHT_PIN 2

namespace FeebeeCam {

    class BlueLight : public Light {
    public: 

        BlueLight() {

        }

        bool initialize() {

            cerr << "Initializing blue light..." << flush;

            pinMode(BLUE_LIGHT_PIN, OUTPUT);
            digitalWrite(BLUE_LIGHT_PIN, LOW);

            Light::_status = digitalRead(LIGHT_PIN);

            cerr << "Ok" << endl;

            return true;
        }

        virtual void turnOn() {
            digitalWrite(BLUE_LIGHT_PIN, HIGH);
            Light::turnOn();
        }

        virtual void turnOff() {
            digitalWrite(BLUE_LIGHT_PIN, LOW);
            Light::turnOff();
        }

    };

    extern BlueLight blueLight;

}

#endif