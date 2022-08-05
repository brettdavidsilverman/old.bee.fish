#include "FS.h"
#include <map>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "web-request.h"
#include "esp-memory.h"
#include "file-system.h"

#define TEMP_FILE_NAME "/temp.txt"
#define MAX_RETRIES 5

namespace FeebeeCam {

    BeeFishBScript::ObjectPointer getManifest();
    bool versionOutOfDate(BeeFishBScript::ObjectPointer& manifest);
    bool installBinaryProgram();

    bool initializeFileSystem() {
        Serial.println("Initializing file system...");

        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS begin failed");
            return false;
        }

        Serial.println("File system initialized");

        return true;
    }

    bool downloadFiles(bool force = false) {

        Serial.println("Checking if we should download files");

        BeeFishBScript::ObjectPointer manifest = getManifest();

        if (manifest == nullptr)
            return false;

        bool isVersionOutOfDate = versionOutOfDate(manifest);

        if (isVersionOutOfDate)
            std::cerr << "Download required" << std::endl;
        else {
            std::cerr << "No download required" << std::endl;
            if (!force)
                return true;
        }


        Serial.println("Downloading beehive files");

        bool success = true;

        for (auto it = manifest->cbegin(); it != manifest->cend(); ++it) {
            
            const BString& key = *it;
            const BString& value = (*manifest)[key];

            if (key != "version") {
                
                bool downloaded = false;
                
                for (int i = 0; i < MAX_RETRIES && !downloaded; ++i) {
                    downloaded = downloadFile(key, value, false);
                }

                success &= downloaded;
            }

        }

        success &= installBinaryProgram();

        if (success) {
            setup._beehiveVersion = (*manifest)["version"];
            success &= setup.save();
            if (success) {
                std::clog   << "Beehive Version upgraded to " 
                            << FeebeeCam::setup._beehiveVersion 
                            << std::endl;
            }
            else {
                std::cerr << "Error saving new beehive version" << std::endl;
            }
        }

        return success;
        
    }

    bool downloadFile(BString source, BString destination, bool print = false) {

        bool downloaded = false;

        if (SPIFFS.exists("/tmp.txt"))
            SPIFFS.remove("/tmp.txt");

        File file = SPIFFS.open("/tmp.txt", FILE_WRITE);

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
        
//            file.flush();

        file.close();

        file = SPIFFS.open("/tmp.txt", FILE_READ);

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
            SPIFFS.rename("/tmp.txt", destination.c_str());
            return true;
        }

        return false;

    }

    BeeFishBScript::ObjectPointer getManifest() {
        Serial.println("Getting beehive manifest from " HOST);

        FeebeeCam::BeeFishWebRequest webRequest("/beehive/manifest.json");

        if (!webRequest.send()) {
            Serial.print("Invalid response ");
            Serial.println(webRequest.statusCode());
            FeebeeCam::resetConnection();
            return nullptr;
        }

        return webRequest.responseBody();

    }

    bool versionOutOfDate(BeeFishBScript::ObjectPointer& manifest) {

        const BString& webVersion = (*manifest)["version"];
        const BString& localVersion = setup._beehiveVersion;

        return webVersion != localVersion;

    }

    bool installBinaryProgram() {
        
        size_t size = 0;

        bool success = true;

        Update.onProgress(
            [](size_t a, size_t b) {
                std::cerr << "{" << a << ", " << b << "}" << std::endl;
//                return;
            }
        );

        // Send the request, trigering file write
        success &= Update.begin(UPDATE_SIZE_UNKNOWN);

        FeebeeCam::BeeFishWebRequest request("/beehive/feebee-cam.ino.bin");

        request.setOnData(
            [&size] (const BeeFishBString::Data& data) {

                size += Update.write((uint8_t*)data.data(), data.size());

  //              if ((size % 10240) == 0)
  //                  std::cerr << size / 1000 << "k" << std::endl;

            }
        );

        // Send the request, trigering file write
        success = request.send();
        success &= Update.end(true);

        if (success)
            std::cerr << "Finished Update" << std::endl; 
        else
            std::cerr << "Error updating binary program" << std::endl;

        return success;

    }

    bool onDownloadFiles(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

        if (client->_webRequest.method() == "POST") {
        
            BeeFishBScript::Object output =
                {
                    {"status", true}
                };

            bool success = false;

            if (success = downloadFiles(true)) {
                output["message"] = "Successfully upgraded";
            }
            else
                output["message"] = "Error upgrading";

            client->_statusCode = 200;
            client->_statusText = "OK";
            client->_contentType = "text/javascript";
            
            client->sendHeaders();

            BeeFishBString::BStream& stream = client->getChunkedOutputStream();

            stream << output;

            client->sendFinalChunk();

            if (success) {
                std::cerr << "Restarting" << std::endl;
                ESP.restart();
            }

            return true;

        }

        return false;

    }

}