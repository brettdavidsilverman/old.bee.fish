#include <Arduino.h>
#include <WiFi.h>

#include <web-request.h>

void setup() {
    Serial.begin(1500000);
    while (!Serial)
        delay(10);

    WiFi.begin("Laptop", "feebeegeeb3");
    while(!WiFi.isConnected()) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("Getting beehive/setup.html");

    FeebeeCam::BeeFishWebRequest request("/beehive/setup.html");
    request.setOnData(
        [] (const BeeFishBString::Data& data) {
            const std::string str((char*)data.c_str(), data.size());
            Serial.println(str.c_str());
        }
    );
    request.send();
}

void loop() {

}
