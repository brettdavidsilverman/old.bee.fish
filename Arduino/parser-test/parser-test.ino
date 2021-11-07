#include <Arduino.h>
#include <parser.h>

using namespace BeeFishParser;
bool ok;

void setup() {
    Serial.begin(115200);

    while (!Serial)
        ;

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