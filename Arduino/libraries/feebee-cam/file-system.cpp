#include "FS.h"
#include <map>
#include <WiFi.h>
#include "web-request.h"
#include "esp-memory.h"
#include "file-system.h"

#define TEMP_FILE_NAME "/temp.txt"

namespace FeebeeCam {

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

    bool downloadRequiredFiles(bool force) {

        Serial.println("Checking if we should download files");

        BeeFishBScript::ObjectPointer manifest;

        BeeFishMisc::optional<bool> result = versionOutOfDate(manifest);

        std::cerr << "downloadRequiredFiles::versionOutOfDate::" << result << std::endl;

        if (!force) {
            if (result == false)
                return true;
            else if (result == BeeFishMisc::nullopt)
                return false;
        }

        cout << (*manifest) << endl;

        Serial.println("Downloading beehive files");

        bool success = true;

        for (auto it = manifest->cbegin(); it != manifest->cend(); ++it) {
            
            const BString& key = *it;
            const BString& value = (*manifest)[key];

            if (key != "version")
                success &= downloadFile(key, value, false);

        }

        if (success) {
            setup._beehiveVersion = (*manifest)["version"];
            if (setup.save()) {
                Serial.print("Beehive Version upgraded to ");
                Serial.println(setup._beehiveVersion);
            }
            else {
                Serial.println("Error saving new beehive version");
            }
        }

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

        static FeebeeCam::BeeFishWebRequest* request = nullptr;

        if (!request)
            request= new FeebeeCam::BeeFishWebRequest(source);

        request->setPath(source);

        size_t size = 0;

        request->setOnData(
            [&file, &size, &print] (const BeeFishBString::Data& data) {
                if (print)
                    Serial.write(data.data(), data.size());
                size += file.write(data.data(), data.size());
                file.flush();
            }
        );

        // Send the request, trigering file write
        downloaded = request->send();
        
        request->flush();

        file.flush();

        // Check the size (error with SPIFFS)
/*
        if (file.size() != size) {
            downloaded = false;
            cout << "Expected " << size << " got " << file.size() << endl;
        }
*/
        if (file.size() == 0) {
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

        Serial.println("Getting beehive version from " HOST);

        BeeFishJSON::Object json;
        BeeFishBScript::BScriptParser parser(json);

        FeebeeCam::BeeFishWebRequest webRequest("/beehive/manifest.json");

        webRequest._ondata = [&parser](const BeeFishBString::Data& data) {
            if (parser.result() == BeeFishMisc::nullopt)
                parser.read(data);
        };

        if (!webRequest.send()) {
            Serial.print("Invalid response ");
            Serial.println(webRequest.statusCode());
            return BeeFishMisc::nullopt;
        }

        if (parser.result() == true)
            manifest = parser.value();
        else {
            cerr << "Invalid manifest file" << endl;
            return BeeFishMisc::nullopt;
        }

        
        const BString& webVersion = (*manifest)["version"];
        const BString& localVersion = setup._beehiveVersion;

        return webVersion != localVersion;

    }


}