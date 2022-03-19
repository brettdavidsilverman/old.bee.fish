#include <weather.h>
#include <light.h>

FeebeeCam::Light* light;
FeebeeCam::Weather* weather;


void setup() {

    Serial.begin(1500000);

    while(!Serial)
        delay(10);    // time to get serial running

    Serial.println("Starting...");
    light = new FeebeeCam::Light();
    weather = new FeebeeCam::Weather();
}


void loop() { 
    using namespace FeebeeCam;
    light->turnOff();
    BeeFishJSONOutput::Object reading = weather->getWeather();
    std::cout << reading << std::endl;
    delay(1000);
    light->rainbow();    
    delay(1000);
}

