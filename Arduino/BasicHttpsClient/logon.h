#ifndef FEEBEECAM_LOGON
#define FEEBEECAM_LOGON

#include "config.h"
#include "r3.pem.h"

#include <HTTPClient.h>

String logon(String secret) {

    const char * headerKeys[] = {"set-cookie"} ;
    const size_t numberOfHeaders = 1;

    HTTPClient https;
  
    if (https.begin(HOST, 443, "/", rootCACertificate)) {
        https.collectHeaders(headerKeys, numberOfHeaders);

        Serial.print("[HTTPS] POST logon...\n");
        // start connection and send HTTP header
        String payLoad = "{\"method\":\"logon\", \"secret\":\"" + secret + "\"}";
        Serial.println(payLoad);
        
        int httpCode = https.POST(payLoad);

        // httpCode will be negative on error
        if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] POST {logon}... code: %d\n", httpCode);

            if (!https.hasHeader("set-cookie")) {
                https.end();
                return "";
            }

            // Check response code
            if (httpCode == HTTP_CODE_OK) {
                String cookies = https.header("set-cookie");
                Serial.printf("Cookie: {%s}\n", cookies.c_str());
                https.end();
                return cookies;
            }

        } else {
            Serial.printf("[HTTPS] POST logon... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
    }

    https.end();

    return "";

}

#endif