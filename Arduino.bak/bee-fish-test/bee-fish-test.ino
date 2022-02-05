#include <Arduino.h>
#include <bee-fish.h>

bool ok;

void setup() {
    Serial.begin(115200);

    while (!Serial)
        ;

    Serial.print(__cplusplus);
    Serial.println();

    BeeFishParser::Character a('a');
    
    Parser parser(a);
      
    ok = parser.read("a");
    ok &= (parser.result() == true);
}

void loop() {
    if (ok)
        Serial.println("Hello World");
    else
        Serial.println("Goodbye!");
}