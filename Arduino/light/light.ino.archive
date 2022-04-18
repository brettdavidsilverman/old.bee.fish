#include <Arduino.h>
#include <light.h>
#include <weather.h>

using namespace FeebeeCam;
Light light;

void setup() {
  
    Serial.begin(1500000);

    while (!Serial)
        ;

    Serial.println("Starting...");

    light.rainbow();
    delay(2000);
}

void loop() {

    light.turnOn(0xFF, 0x00, 0x00);
    delay(1000);
    light.turnOn(0x00, 0xFF, 0x00);
    delay(1000);
    light.turnOn(0x00, 0x00, 0xFF);
    delay(1000);
    light.rainbow();
    delay(2000);

    Weather weather;

    cout << weather.getWeather()["temperature"] << endl;
}