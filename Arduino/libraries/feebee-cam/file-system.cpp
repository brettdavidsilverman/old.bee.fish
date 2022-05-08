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
    bool versionOutOfDate(std::shared_ptr<BeeFishBScript::Object>& manifest);

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

        std::shared_ptr<BeeFishBScript::Object> manifest;

        if (versionOutOfDate(manifest) == false) {
            Serial.println("No file download required");
            return true;
        }

        cout << (*manifest) << endl;

        Serial.println("Downloading beehive files");

        bool success = true;

        for (auto pair : (*manifest)) {
            const BString& key = pair.first;
            const BString& value = pair.second;

            if (key != "version")
                success &= downloadFile(key, value);

        }

        if (success)
            success &= downloadFile("/beehive/manifest.json", "/manifest.json");

        if (success)
            Serial.println("Successfully downloaded files");
        else
            Serial.println("Errors downloading all files");

        return success;
        
    }

    bool downloadFile(const BString& source, const BString& destination, bool print) {

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

    bool versionOutOfDate(std::shared_ptr<BeeFishBScript::Object>& manifest) {

        bool different = false;

        File file = SPIFFS.open("/manifest.json", FILE_READ);

        if (!file)
            return true;

        Serial.println("Getting beehive version from " HOST);

        BeeFishJSON::Object json;
        BeeFishBScript::BScriptParser webParser(json);

        FeebeeCam::BeeFishWebRequest request("/beehive/manifest.json");

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

        manifest = webParser.value();

        return different;

    }


}