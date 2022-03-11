#include "FS.h"
#include "SPIFFS.h"
#include <map>
#include <WiFi.h>
#include "web-request.h"
#include "esp-memory.h"

#define TEMP_FILE_NAME "/temp"

namespace FeebeeCam {

    bool downloadWhenReady = false;

    void initializeFileSystem() {
        Serial.println("Initializing file system...");

        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS Mount Failed, formatted instead.");
        }
    }

    bool versionOutOfDate() {

        bool outOfDate = false;

        File file = SPIFFS.open("/version.txt", FILE_READ);

        if (!file)
            return true;

        Serial.println("Getting beehive version from " HOST);

        FeebeeCam::BeeFishWebRequest request("/beehive/version.txt");
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
            return false;
        }

        file.close();

        return outOfDate;

    }

    bool downloadFile(const BString& source, const BString& destination) {

        bool downloaded = false;

        for (int tryDownload = 0; !downloaded && tryDownload < 3; ++tryDownload) {

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

            bool success = false;

            if (request.send()) {
                tempFile.close();
                bool correctStatusCode = (request.statusCode() == 200);
                if (correctStatusCode) {

                    if (SPIFFS.exists(destination.c_str()))
                        SPIFFS.remove(destination.c_str());

                    SPIFFS.rename(TEMP_FILE_NAME, destination.c_str());

                    success = true;

                }
            }
            else {
                tempFile.close();
            }

            if (!success)
                Serial.println("Error downloading file");

            downloaded = success;
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
            {"/beehive/fetch.js", "/fetch.js"},
            {"/beehive/full-screen.js", "/full-screen.js"},
            {"/beehive/green-small.jpg", "/green-small.jpg"},
            {"/beehive/loading-brown.gif", "/loading-brown.gif"},
            {"/beehive/logon.html", "/logon.html"},
            {"/beehive/red-small.jpg", "/red-small.jpg"},
            {"/beehive/restart.html", "/restart.html"},
            {"/beehive/setup.html", "/setup.html"},
            {"/beehive/sha256.js", "/sha256.js"},
            {"/beehive/sha512.js", "/sha512.js"},
            {"/beehive/style.css", "/style.css"},
            {"/beehive/winnie-black.jpg", "/winnie-black.jpg"},
            {"/beehive/winnie.jpg", "/winnie.jpg"},
        };

        for (auto pair : files) {
            
            const BeeFishBString::BString& source = pair.first;
            const BeeFishBString::BString& destination = pair.second;

            if (!downloadFile(source,destination))
                return false;

            printMemory();

        }

        if (!downloadFile("/beehive/version.txt", "/version.txt")) {
            return false;
        }
        
        Serial.println("Success");

        return true;

    }


}