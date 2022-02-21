#include <Arduino.h>
#include <WiFi.h>
#include <web-request.h>

void retrieveFile(const BString& path) {
    Serial.print("Getting ");
    Serial.println(path.c_str());

    FeebeeCam::BeeFishWebRequest request(path);
//    FeebeeCam::WebRequest request("www.google.com", path, "");

    request.setOnData(
        [] (const BeeFishBString::Data& data) {
            //Serial.write(data.data(), data.size());
        }
    );
    bool parsed = request.send();

    if (parsed && request.statusCode() == 200)
        Serial.println("Ok");
    else {
        Serial.print("Error: ");
        Serial.println(request.statusCode());
    }

}

void setup() {
    Serial.begin(1500000);
    while (!Serial)
        delay(10);

    WiFi.begin("Laptop", "feebeegeeb3");
    while(!WiFi.isConnected()) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("Enter path");
}

void loop() {

    while (Serial.available()) {
        
        String line = Serial.readString();
        BString path(line.c_str(), line.length());
        
        while (Serial.available())
            Serial.read();

        retrieveFile(path);
    }
}
