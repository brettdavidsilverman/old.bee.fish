#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <WiFi.h>
#include "ssl-connection.h"

void setup()
{
    Serial.begin(1500000);
    while (!Serial)
        delay(10);

    Serial.println("Starting");
    
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);

    Serial.print("Connecting to ssid Laptop");
    WiFi.begin("Laptop", "feebeegeeb3");
    while (!WiFi.isConnected())
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("Connected to wifi");

    if (FeebeeCam::SSLConnection::test("bee.fish", 443, 10))
        Serial.println("Success");
    else
        Serial.println("Failed");
}

void loop() {

}