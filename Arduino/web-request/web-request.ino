#include "bee-fish-web-request.h"

void setup() {
    FeebeeCam::BeeFishWebRequest request("https://laptop/beehive/setup.html");
    request.setOnData(
        [] (const char* buffer, size_t length) {
            Serial.println(buffer);
        }
    );
    request.send();
}

void loop() {

}