#include <Arduino.h>

void logMemory() {
    Serial.print("PSRAM Size: ");
    Serial.print(ESP.getPsramSize() / 1000);
    Serial.println("kb");

    Serial.print("Used PSRAM: ");
    Serial.print((ESP.getPsramSize() - ESP.getFreePsram()) / 1000);
    Serial.println("kb");
}