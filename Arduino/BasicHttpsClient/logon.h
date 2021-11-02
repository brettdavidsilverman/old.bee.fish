#ifndef FEEBEECAM_LOGON
#define FEEBEECAM_LOGON

#include "config.h"
#include "r3.pem.h"

#include <HTTPClient.h>

String getSessionIdFromCookie(String cookie);

HTTPClient https;

class NullStream : public Stream {
public:
    
    virtual size_t write(uint8_t){
        return 1;
    }

    virtual int available() {
        return 0;
    };

    virtual int read() {
        return -1;
    };

    virtual int peek()  {
        return -1;
    };

    virtual void flush() {

    }
};

String logon(String host, String secret) {

    const char * headerKeys[] = {"set-cookie"} ;
    const size_t numberOfHeaders = 1;

    if (https.begin(host, 443, "/", rootCACertificate)) {

        https.collectHeaders(headerKeys, numberOfHeaders);
        // start connection and send HTTP header
        String payLoad = "{\"method\":\"logon\", \"secret\":\"" + secret + "\"}";
        
        int httpCode = https.POST(payLoad);

        // httpCode will be negative on error
        if (httpCode > 0) {

            if (!https.hasHeader("set-cookie")) {
                return "";
            }

            // Check response code
            if (httpCode == HTTP_CODE_OK) {
                https.writeToStream(&Serial);
                String cookie = https.header("set-cookie");
                String sessionId = getSessionIdFromCookie(cookie);
                return sessionId;
            }

        } else {
            Serial.printf("[HTTPS] POST logon... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
    }

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