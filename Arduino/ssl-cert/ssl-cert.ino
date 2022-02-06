#include <ssl-cert.h>

void setup() {
    Serial.begin(1500000);
    while (!Serial)
        delay(10);

    Serial.println("Starting");

    initializeSSLCert();

    Serial.println((char*)(sslCert->getCertData()));
}

void loop() {
    
}