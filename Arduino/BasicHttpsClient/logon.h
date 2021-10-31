#ifndef FEEBEECAM_LOGON
#define FEEBEECAM_LOGON

#include "config.h"
#include "r3.pem.h"

#include <HTTPClient.h>

String getSessionIdFromCookie(String cookie);

String logon(String secret) {

    const char * headerKeys[] = {"set-cookie"} ;
    const size_t numberOfHeaders = 1;

    HTTPClient https;
  
    if (https.begin(HOST, 443, "/", rootCACertificate)) {

        https.collectHeaders(headerKeys, numberOfHeaders);
        // start connection and send HTTP header
        String payLoad = "{\"method\":\"logon\", \"secret\":\"" + secret + "\"}";
        
        int httpCode = https.POST(payLoad);

        // httpCode will be negative on error
        if (httpCode > 0) {

            if (!https.hasHeader("set-cookie")) {
                https.end();
                return "";
            }

            // Check response code
            if (httpCode == HTTP_CODE_OK) {
                String cookie = https.header("set-cookie");
                String sessionId = getSessionIdFromCookie(cookie);
                https.end();
                return sessionId;
            }

        } else {
            Serial.printf("[HTTPS] POST logon... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
    }

    https.end();

    return "";

}

String getSessionIdFromCookie(String cookie) {
    char parts[cookie.length() + 1];
    strcpy(parts, cookie.c_str());
    char* part = strtok(parts, ";");
    while (part) {
        String byte = part;
        byte.trim();
        if (byte.startsWith("sessionId=")) {
            unsigned int start = strlen("sessionId=");
            unsigned int end = byte.length();
            String sessionId = byte.substring(
                start,
                end
            );
            return sessionId;
        }
        part = strtok(NULL, ";");
    }
    return "";
}
#endif