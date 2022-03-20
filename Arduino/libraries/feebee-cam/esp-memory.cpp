#include "esp-memory.h"

namespace FeebeeCam {
    
    void printMemory() {
        Serial.print("PSRAM Size:\t");
        Serial.print(ESP.getPsramSize() / 1024.0);
        Serial.println(" kb");

        Serial.print("Used PSRAM:\t");
        Serial.print((ESP.getPsramSize() - ESP.getFreePsram()) / 1024.0);
        Serial.println(" kb");

        Serial.print("Heap Size:\t");
        Serial.print(ESP.getHeapSize() / 1024.0);
        Serial.println(" kb");

        Serial.print("Heap Used:\t");
        Serial.print(
            (ESP.getHeapSize() - ESP.getFreeHeap()) / 1024.0
        );
        Serial.println(" kb");

        Serial.print("Heap Used:\t");
        Serial.print(
            ((float)ESP.getHeapSize() - (float)ESP.getFreeHeap()) / 
            (float)ESP.getHeapSize() * 100.0
        );
        Serial.println(" %");

    }
}
