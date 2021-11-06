#include <Arduino.h>
#include <parser.h>

void setup() {
    Serial.begin(115200);

    while (!Serial)
        ;

    Serial.println("Hello World");
}

void loop() {
    
}