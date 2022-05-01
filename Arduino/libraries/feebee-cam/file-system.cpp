#include "FS.h"
#include "SPIFFS.h"
#include <map>
#include <WiFi.h>
#include "web-request.h"
#include "esp-memory.h"
#include "file-system.h"

#define TEMP_FILE_NAME "/temp.txt"

#define DOWNLOAD(from, to) {success &= downloadFile(from, to);}
 //if (!downloadFile(from, to)) return false

namespace FeebeeCam {

    bool downloadWhenReady = false;
    bool versionOutOfDate(BeeFishBScript::Object& manifest);

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

        BeeFishBScript::Object manifest;

        if (versionOutOfDate(manifest) == false) {
            Serial.println("No file download required");
            return true;
        }

        cout << manifest << endl;

        Serial.println("Downloading beehive files");

        bool success = true;

        DOWNLOAD("/beehive/beehive-index.html","/index.html");
        DOWNLOAD("/beehive/error.js",          "/error.js");
        DOWNLOAD("/beehive/full-screen.js",    "/full-screen.js");
        DOWNLOAD("/beehive/green-small.jpg",   "/green-small.jpg");
        DOWNLOAD("/beehive/loading-brown.gif", "/loading-brown.gif");
        DOWNLOAD("/beehive/logon.html",        "/logon.html");
        DOWNLOAD("/beehive/red-small.jpg",     "/red-small.jpg");
        DOWNLOAD("/beehive/setup/index.html",  "/setup/index.html");
        DOWNLOAD("/beehive/style.css",         "/style.css");
        DOWNLOAD("/beehive/winnie-black.jpg",  "/winnie-black.jpg");
        DOWNLOAD("/beehive/winnie.jpg",        "/winnie.jpg");
        DOWNLOAD("/client/fetch.js",           "/fetch.js");
        DOWNLOAD("/client/logon/sha256.js",    "/sha256.js");
        DOWNLOAD("/client/logon/sha512.js",    "/sha512.js");
        DOWNLOAD("/client/id/id.js",           "/id.js");
        DOWNLOAD("/client/storage/storage.js", "/storage.js");

        if (success)
            DOWNLOAD("/beehive/version.json",      "/version.json");

        if (success)
            Serial.println("Successfully downloaded files");
        else
            Serial.println("Errors downloading all files");

        return success;
        
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

    bool versionOutOfDate(BeeFishBScript::Object& manifest) {

        bool different = false;

        File file = SPIFFS.open("/version.json", FILE_READ);

        if (!file)
            return true;

        Serial.println("Getting beehive version from " HOST);

        BeeFishBScript::BScriptParser webParser;

        FeebeeCam::BeeFishWebRequest request("/beehive/version.json");

        request.setOnData(
            [&file, &different, &webParser] (const BeeFishBString::Data& data) {

                const Byte* downloadBuffer = data.data();
                
                if (webParser.result() == BeeFishMisc::nullopt)
                    webParser.read(data);

                Byte fileBuffer[data.size()];
                size_t read = file.read(fileBuffer, data.size());
                if (read != data.size())
                    different = true;
                else {
                    if (memcmp(downloadBuffer, fileBuffer, data.size()) != 0)
                        different = true;
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

        if (different) {
            throw std::logic_error("For Fuck sake shouldnt be here");
//            manifest = webParser.object();
        }

        return different;

    }


}