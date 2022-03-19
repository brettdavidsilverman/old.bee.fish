#include "bee-fish.h"


void setup() {
  
    Serial.begin(1500000);

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
