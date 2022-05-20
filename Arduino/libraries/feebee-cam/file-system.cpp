#include "FS.h"
#include "SPIFFS.h"
#include <map>
#include <WiFi.h>
#include "web-request.h"
#include "esp-memory.h"
#include "file-system.h"

#define TEMP_FILE_NAME "/temp.txt"

namespace FeebeeCam {

    bool downloadWhenReady = false;
    BeeFishMisc::optional<bool> versionOutOfDate(BeeFishBScript::ObjectPointer& manifest);

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

        Serial.println("Checking if we should download files");

        BeeFishBScript::ObjectPointer manifest;

        BeeFishMisc::optional<bool> result = versionOutOfDate(manifest);

        if (result == false)
            return true;
        else if (result == BeeFishMisc::nullopt)
            return false;

        cout << (*manifest) << endl;

        Serial.println("Downloading beehive files");

        bool success = true;

        for (auto pair : (*manifest)) {
            const BString& key = pair.first;
            const BString& value = pair.second;

            if (key != "version")
                success &= downloadFile(key, value, false, true);

        }

        if (success)
            success &= downloadFile("/beehive/manifest.json", "/manifest.json", false, true);

        if (success)
            Serial.println("Successfully downloaded files");
        else
            Serial.println("Errors downloading all files");

        return success;
        
    }

    bool downloadFile(const BString& source, const BString& destination, bool print, bool ignoreErrors) {

        bool downloaded = false;

        if (SPIFFS.exists("/tmp"))
            SPIFFS.remove("/tmp");

        File file = SPIFFS.open("/tmp", FILE_WRITE);

        FeebeeCam::BeeFishWebRequest request(source);

        size_t size = 0;

        request.setOnData(
            [&file, &size, &print] (const BeeFishBString::Data& data) {
                if (print)
                    Serial.write(data.data(), data.size());
                size += file.write(data.data(), data.size());
                file.flush();
            }
        );

        // Send the request, trigering file write
        downloaded = request.send();
        
        request.flush();

        file.flush();

        // Check the size (error with SPIFFS)
        if (file.size() != size) {
            if (!ignoreErrors)
                downloaded = false;
            cout << "Expected " << size << " got " << file.size() << endl;
        }

        file.close();

        if (downloaded) {
            // Move file from temp to proper file path
            if (SPIFFS.exists(destination.c_str()))
                SPIFFS.remove(destination.c_str());
            SPIFFS.rename("/tmp", destination.c_str());
        }
        else {
            Serial.println("Error downloading file");
        }

        return downloaded;

    }

    BeeFishMisc::optional<bool> versionOutOfDate(BeeFishBScript::ObjectPointer& manifest) {

        bool different = false;

        File file = SPIFFS.open("/manifest.json", FILE_READ);

        if (!file)
            different = true;

        Serial.println("Getting beehive version from " HOST);



        BeeFishJSON::Object json;
        BeeFishBScript::BScriptParser parser(json);

        FeebeeCam::BeeFishWebRequest webRequest("/beehive/manifest.json");

        webRequest.setOnData(
            [&file, &different, &parser] (const BeeFishBString::Data& data) {

                if (parser.result() == BeeFishMisc::nullopt) {
                    parser.read(data);
                }

                if (file) {

                    Byte fileBuffer[data.size()];
                    size_t read = file.read(fileBuffer, data.size());

                    if (read != data.size())
                        different = true;
                    else {
                        if (memcmp(data.data(), fileBuffer, data.size()) != 0)
                            different = true;
                    }

                }

            }
        );

        

        if (!webRequest.send()) {
            if (file)
                file.close();
            Serial.print("Invalid response ");
            Serial.println(webRequest.statusCode());
            return BeeFishMisc::nullopt;
        }

        if (file)
            file.close();

        if (parser.result() == true)
            manifest = parser.value();
        else {
            cerr << "Invalid manifest file" << endl;
            return BeeFishMisc::nullopt;
        }

        return different;

    }


}