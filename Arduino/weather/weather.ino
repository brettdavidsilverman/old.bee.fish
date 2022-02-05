#include <weather.h>


void setup() {

    Serial.begin(1500000);

    while(!Serial)
        delay(10);    // time to get serial running

    Serial.println("Weather test");

}


void loop() { 
    Weather weather;
    weather.print(Serial);
    delay(1000);
}

