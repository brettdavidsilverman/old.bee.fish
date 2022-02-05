#include "light.h"
#include "weather.h"

Light light;

void setup() {
  
    Serial.begin(1500000);

    while (!Serial)
        ;

    light.turnOn();

    Serial.println("Starting...");
}

void loop() {
    light.turnOn(Light::BLUE);
    delay(50);
    light.turnOff();
    delay(50);
}