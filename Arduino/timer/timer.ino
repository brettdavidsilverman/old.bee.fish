#include "battery.h"
#include "Adafruit_MCP23008.h"

class Light {

  const int pin = 0;
  Adafruit_MCP23008* port;

public:
  bool isOn;
  Light() {
    port = new Adafruit_MCP23008();
    port->begin();

    // configure pin for output
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

Light* light;

void setup() {
  bat_init();
  bat_hold_output();

  Serial.begin(115200);
  light = new Light();
  light->turnOn();
}

uint minimum =  0;
uint maximum = 10000;
uint counter = 0;

uint level = 10000;

void loop() {

  if (Serial.available() > 0) {
    
    // Read number from input
    long myInt = Serial.parseInt();

    if ((myInt < minimum) || (myInt > maximum)) {
      Serial.printf("Must be between %u and %u.\n", minimum, maximum);
    }
    else {
      Serial.printf("Setting level to %u.\n", myInt);
      level = myInt;
      counter = 0;
    }

    // Clear input buffer
    while (Serial.available()) {
      Serial.read();
    }
    
  }

  if (counter < level) {
    if (!light->isOn)
      light->turnOn();
  }
  else if (light->isOn) {
    light->turnOff();
  }

  if (++counter >= maximum)
     counter = minimum;

}
