#include <Arduino.h>
#include <light.h>
#include <weather.h>

using namespace FeebeeCam;

void setup() {
  
    Serial.begin(1500000);

    while (!Serial)
        ;

    Serial.println("Starting...");

    Light light;
    light.rainbow();

}

void loop() {
    Light* light = new Light();
    light->turnOff();
    delay(1000);
    light->turnOn(0xFF, 0x00, 0x00);
    delay(1000);
    delete light;
    Weather* weather = new Weather();
    cout << weather->getWeather() << endl;
}