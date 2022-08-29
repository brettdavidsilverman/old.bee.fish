#include <Arduino.h>
#include <WiFi.h>
#include <web-request.h>
#include <web-storage.h>

#include <bee-fish.h>

void retrieveFile(const BString& path) {
    Serial.print("Getting ");
    Serial.println(path.c_str());

    FeebeeCam::BeeFishWebRequest request(path);

    size_t size = 0;

    request.setOnData(
        [&size] (const BeeFishBString::Data& data) {
            Serial.write(data._data, data.size());
            size += data.size();
        }
    );
    
    bool parsed = request.send();

    if (parsed && request.statusCode() == 200) {
        Serial.print("Ok ");
        Serial.println(size);
    }
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

    Serial.print("Path: ");
}

void loop() {


    if (Serial.available()) {
        String path = Serial.readStringUntil('\r');
        retrieveFile(path.c_str());
        Serial.println();
        Serial.print("Path: ");
    }

    return;

    Serial.print("Key: ");

    while (!Serial.available())
        delay(500);
        
    String key = Serial.readString();
    BString _key(key.c_str(), key.length());

    Serial.println();
    Serial.print("Value: ");

    while (!Serial.available())
        delay(500);

    String value = Serial.readString();
    BString _value(value.c_str(), value.length());

    Serial.println();

    BeeFishBScript::Object object {
        {"value", _value}
    };

    FeebeeCam::BeeFishStorage storage;

    bool sent = storage.setItem("/client/storage/", _key, object);

    if (sent)
        Serial.println("Ok");
    else
        Serial.println("Error");
}
