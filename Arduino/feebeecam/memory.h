#include <Arduino.h>

void logMemory() {
    Serial.print("Used PSRAM: ");
    Serial.println(ESP.getPsramSize() - ESP.getFreePsram());
}