#include <weather.h>

TwoWire wire(0);

void setup() {

    Serial.begin(1500000);

    while(!Serial)
        delay(10);    // time to get serial running

    Serial.println("Starting...");

    wire.begin(SDA, SCL);

    Serial.println("Weather test");

}


void loop() { 
    using namespace FeebeeCam;
    
    Weather weather(&wire);
    BeeFishJSONOutput::Object reading = weather.getWeather();
    std::cout << reading << std::endl;
    delay(1000);
}

