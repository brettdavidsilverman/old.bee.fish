#include "FS.h"
#include "SPIFFS.h"
#include <map>
#include <WiFi.h>
#include "web-request.h"
#include "esp-memory.h"
#include "file-system.h"

#define TEMP_FILE_NAME "/temp.txt"

#define DOWNLOAD(from, to) downloadFile(from, to)
 //if (!downloadFile(from, to)) return false

namespace FeebeeCam {

    bool downloadWhenReady = false;
    bool versionOutOfDate();

    bool initializeFileSystem() {
        Serial.println("Initializing file system...");

        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS begin failed");
            return false;
        }

        Serial.println("File system initialized");

        return true;
    }

    bool downloadRequiredFiles() {

        if (versionOutOfDate() == false) {
            Serial.println("No file download required");
            return true;
        }

        DOWNLOAD("/beehive/setup/index.html",  "/setup/index.html");
        DOWNLOAD("/beehive/beehive.html",      "/index.html");
        DOWNLOAD("/beehive/error.js",          "/error.js");
        DOWNLOAD("/client/fetch.js",           "/fetch.js");
        DOWNLOAD("/beehive/full-screen.js",    "/full-screen.js");
        DOWNLOAD("/beehive/green-small.jpg",   "/green-small.jpg");
        DOWNLOAD("/beehive/loading-brown.gif", "/loading-brown.gif");
        DOWNLOAD("/beehive/logon.html",        "/logon.html");
        DOWNLOAD("/beehive/red-small.jpg",     "/red-small.jpg");
        DOWNLOAD("/beehive/restart.html",      "/restart.html");
        DOWNLOAD("/beehive/setup/index.html",  "/index.html");
        DOWNLOAD("/client/logon/sha256.js",    "/sha256.js");
        DOWNLOAD("/client/logon/sha512.js",    "/sha512.js");
        DOWNLOAD("/beehive/style.css",         "/style.css");
        DOWNLOAD("/beehive/winnie-black.jpg",  "/winnie-black.jpg");
        DOWNLOAD("/beehive/winnie.jpg",        "/winnie.jpg");
        DOWNLOAD("/beehive/version.json",        "/version.json");

        return true;


        Serial.println("Downloading beehive files");
        
        std::map<BeeFishBString::BString, BeeFishBString::BString> files {
            {"/beehive/setup/index.html",  "/setup/index.html"},
            {"/beehive/beehive.html",      "/index.html"},
            {"/beehive/error.js",          "/error.js"},
            {"/client/fetch.js",           "/fetch.js"},
            {"/beehive/full-screen.js",    "/full-screen.js"},
            {"/beehive/green-small.jpg",   "/green-small.jpg"},
            {"/beehive/loading-brown.gif", "/loading-brown.gif"},
            {"/beehive/logon.html",        "/logon.html"},
            {"/beehive/red-small.jpg",     "/red-small.jpg"},
            {"/beehive/restart.html",      "/restart.html"},
            {"/beehive/setup/index.html",  "/index.html"},
            {"/client/logon/sha256.js",    "/sha256.js"},
            {"/client/logon/sha512.js",    "/sha512.js"},
            {"/beehive/style.css",         "/style.css"},
            {"/beehive/winnie-black.jpg",  "/winnie-black.jpg"},
            {"/beehive/winnie.jpg",        "/winnie.jpg"}
        };

        for (auto pair : files) {
            
            const BeeFishBString::BString& source = pair.first;
            const BeeFishBString::BString& destination = pair.second;

            if (!downloadFile(source,  destination))
                return false;
        }

        if (!downloadFile("/beehive/version.json", "/version.json")) {
            return false;
        }
        
        Serial.println("Success");

        return true;

    }

    bool downloadFile(const BString& source, const BString& destination, bool print) {

        bool downloaded = false;

        if (SPIFFS.exists(destination.c_str()))
            SPIFFS.remove(destination.c_str());

        File file = SPIFFS.open(destination.c_str(), FILE_WRITE);

        FeebeeCam::BeeFishWebRequest request(source);

        size_t size = 0;

        request.setOnData(
            [&file, &size, &print] (const BeeFishBString::Data& data) {
                if (print)
                    Serial.write(data.data(), data.size());
                size += data.size();
                file.write(data.data(), data.size());
                file.flush();
            }
        );

        // Send the request, trigering file write
        downloaded = request.send();

        file.flush();

        // Check the size (error with SPIFFS)
        if (file.size() != size) {
            downloaded = false;
            cout << "Expected " << size << " got " << file.size() << endl;
        }

        if (!downloaded) {
            Serial.println("Error downloading file");
        }

        file.close();

        return downloaded;

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

        

        if (!request.send()) {
            file.close();
            Serial.print("Invalid response ");
            Serial.println(request.statusCode());
            return false;
        }

        file.close();

        return outOfDate;

    }


}