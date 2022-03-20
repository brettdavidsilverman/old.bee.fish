#include "FS.h"
#include "SPIFFS.h"
#include <map>
#include <WiFi.h>
#include "web-request.h"
#include "esp-memory.h"

#define TEMP_FILE_NAME "/temp"

namespace FeebeeCam {

    bool downloadWhenReady = false;

    bool initializeFileSystem() {
        Serial.println("Initializing file system...");

        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS Mount Failed, formatted instead.");
        }

        if (!SPIFFS.exists("/version.json"))
            return false;

        return true;
    }

    bool versionOutOfDate() {

        bool outOfDate = false;

        File file = SPIFFS.open("/version.json", FILE_READ);

        if (!file)
            return true;

        Serial.println("Getting beehive version from " HOST);

        FeebeeCam::BeeFishWebRequest request("/beehive/version.json");

        request.setOnData(
            [&file, &outOfDate] (const BeeFishBString::Data& data) {

                const Byte* downloadBuffer = data.data();
                Byte fileBuffer[data.size()];
                size_t read = file.read(fileBuffer, data.size());
                if (read != data.size())
                    outOfDate = true;
                else {
                    if (memcmp(downloadBuffer, fileBuffer, data.size()) != 0)
                        outOfDate = true;
                }
                
            }
        );

        request.send();

        if (request.statusCode() != 200) {
            file.close();
            Serial.print("Invalid response ");
            Serial.println(request.statusCode());
            return false;
        }

        file.close();

        return outOfDate;

    }

    bool downloadFile(const BString& source, const BString& destination) {

        bool downloaded = false;

        Serial.print(source.c_str());
        Serial.print(" -> ");
        Serial.println(destination.c_str());

        if (SPIFFS.exists(TEMP_FILE_NAME))
            SPIFFS.remove(TEMP_FILE_NAME);

        File tempFile = SPIFFS.open(TEMP_FILE_NAME, FILE_WRITE);

        FeebeeCam::BeeFishWebRequest request(source);

        request.setOnData(
            [&tempFile] (const BeeFishBString::Data& data) {
                tempFile.write(data.data(), data.size());
            }
        );

        if (request.send()) {
            tempFile.close();
            bool correctStatusCode = (request.statusCode() == 200);
            if (correctStatusCode) {

                if (SPIFFS.exists(destination.c_str()))
                    SPIFFS.remove(destination.c_str());

                SPIFFS.rename(TEMP_FILE_NAME, destination.c_str());

                downloaded = true;

            }
        }
        else {
            tempFile.close();
        }

        if (!downloaded) {
            Serial.println("Error downloading file");
        }

        return downloaded;

    }

    bool downloadRequiredFiles() {

        if (versionOutOfDate() == false) {
            Serial.println("No file download required");
            return true;
        }

        Serial.println("Downloading beehive files");
        
        std::map<BeeFishBString::BString, BeeFishBString::BString> files {
            {"/beehive/beehive.html", "/index.html"},
            {"/beehive/error.js", "/error.js"},
            {"/client/fetch.js", "/fetch.js"},
            {"/beehive/full-screen.js", "/full-screen.js"},
            {"/beehive/green-small.jpg", "/green-small.jpg"},
            {"/beehive/loading-brown.gif", "/loading-brown.gif"},
            {"/beehive/logon.html", "/logon.html"},
            {"/beehive/red-small.jpg", "/red-small.jpg"},
            {"/beehive/restart.html", "/restart.html"},
            {"/beehive/setup.html", "/setup.html"},
            {"/client/logon/sha256.js", "/sha256.js"},
            {"/client/logon/sha512.js", "/sha512.js"},
            {"/beehive/style.css", "/style.css"},
            {"/beehive/winnie-black.jpg", "/winnie-black.jpg"},
            {"/beehive/winnie.jpg", "/winnie.jpg"},
        };

        for (auto pair : files) {
            
            const BeeFishBString::BString& source = pair.first;
            const BeeFishBString::BString& destination = pair.second;

            if (!downloadFile(source, destination))
                return false;
        }

        if (!downloadFile("/beehive/version.json", "/version.json")) {
            return false;
        }
        
        Serial.println("Success");

        return true;

    }


}