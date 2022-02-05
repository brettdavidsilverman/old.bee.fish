#ifndef FEEBEECAM_LIGHT
#define FEEBEECAM_LIGHT

#include "Adafruit_MCP23008.h"

class Light {

  const int pin = 0;
  Adafruit_MCP23008* port;

public:
  bool isOn;
  Light() {
    port = new Adafruit_MCP23008();
    port->begin();

    // configure pin for output and default to off
    port->digitalWrite(pin, LOW);
    port->pinMode(pin, OUTPUT);
    isOn = false;
  }

  ~Light() {
    delete port;
  }

  void turnOn() {
    port->digitalWrite(pin, HIGH);
    isOn = true;
  } 

  void turnOff() {
    port->digitalWrite(pin, LOW);
    isOn = false;
  }

  void toggle() {
    if (isOn)
      turnOff();
    else
      turnOn();
  }

  void print() {
    if (isOn)
      Serial.print("Light on");
    else
      Serial.print("Light off");

  }

};

Light light;

#endif