#include "FS.h"
#include "SPIFFS.h"
#include <map>
#include <WiFi.h>
#include <web-request.h>

namespace FeebeeCam {

    bool downloadWhenReady = false;

    void initializeFileSystem() {
        Serial.println("Initializing file system...");

        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS Mount Failed, formatted instead.");
        }
    }
 
    void downloadRequiredFiles() {

        bool requireDownload = false;

        File file = SPIFFS.open("/version.txt", FILE_READ);
        if (!file)
            requireDownload = true;
        else {
            Serial.println("Getting beehive version from web");

            FeebeeCam::BeeFishWebRequest request("/beehive/version.txt");
            request.setOnData(
                [&file, &requireDownload] (const BeeFishBString::Data& data) {

                    const Byte* buffer = data.data();
                    size_t i;
                    for (i = 0; i < data.size() && file.available(); ++i) {
                        uint8_t character = file.read();
                        cout << (char)character;
                        if (character != buffer[i])
                            requireDownload = true;
                    }

                    if (!file.available() && i < data.size())
                        requireDownload = true;
                }
            );

            request.send();

            if (request.statusCode() != 200)
                requireDownload = true;

            file.close();
        }

        if (!requireDownload) {
            Serial.println("No file download required");
            return;
        }

        Serial.println("Downloading beehive files");
        
        std::map<BeeFishBString::BString, BeeFishBString::BString> files {
            {"/beehive/beehive.html", "/index.html"},
            {"/beehive/error.js", "/error.js"},
            {"/beehive/fetch.js", "/fetch.js"},
            {"/beehive/full-screen.js", "/full-screen.js"},
            {"/beehive/green-small.jpg", "/green-small.jpg"},
            {"/beehive/loading-brown.gif", "/loading-brown.gif"},
            {"/beehive/logon.html", "/logon.html"},
            {"/beehive/red-small.jpg", "/red-small.jpg"},
            {"/beehive/restart.js", "/restart.js"},
            {"/beehive/setup.html", "/setup.html"},
            {"/beehive/sha256.js", "/sha256.js"},
            {"/beehive/sha512.js", "/sha512.js"},
            {"/beehive/style.css", "/style.css"},
            {"/beehive/winnie-black.jpg", "/winnie-black.jpg"},
            {"/beehive/winnie.jpg", "/winnie.jpg"},
            {"/beehive/version.txt", "/version.txt"}
        };

        for (auto pair : files) {
            
            const BeeFishBString::BString& fileName = pair.first;
            const BeeFishBString::BString& destination = pair.second;

            Serial.println(fileName);

            FeebeeCam::BeeFishWebRequest request(fileName);
            File file = SPIFFS.open(destination, FILE_WRITE);
            
            request.setOnData(
                [&file] (const BeeFishBString::Data& data) {
                    file.write(data.data(), data.size());
                }
            );

            request.send();

            file.close();

        }

        Serial.println("Success");

    }


}