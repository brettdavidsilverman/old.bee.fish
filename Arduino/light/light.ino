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
    light.turnOff();
    Weather weather;
    weather.print(Serial);
    light.turnOff();
    delay(1000);
}