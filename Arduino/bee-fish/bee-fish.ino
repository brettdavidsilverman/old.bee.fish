#include "bee-fish.h"


void setup() {
  
    Serial.begin(115200);

    while (!Serial)
        ;

    if (BeeFishTest::test())
        Serial.println("BeeFishTest OK");
    else
        Serial.println("BeeFishTest Fail");

    while (1)
        ;
}

void loop() {
    
}
