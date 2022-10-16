#include <iostream>
#include "FS.h"
#include <map>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "web-request.h"
#include "esp-memory.h"
#include "commands.h"
#include "file-system.h"
#include "setup.h"

#define TEMP_FILE_NAME "/temp.txt"
#define MAX_RETRIES 5

namespace FeebeeCam {

    BeeFishBScript::ObjectPointer getManifest();
    bool versionOutOfDate(BeeFishBScript::ObjectPointer& manifest);
    bool installBinaryProgram();

    bool initializeFileSystem() {

        Serial.println("Initializing file system...");

        if (!SPIFFS.begin(true, "/spiffs", 10, "spiffs")) {
            cerr << "SPIFFS begin failed, formatting" << endl;
            if (SPIFFS.format()) {
                std::cerr << "SPIFFS formatted" << std::endl;
                SPIFFS.begin(false);
                FeebeeCam::initializeSetup();
            }
            else {
                std::cerr << "Errro with formatting SPIFFS" << std::endl;
            }
        }

        Serial.println("File system initialized");

        return true;
    }

    bool downloadFiles(bool force, bool downloadBinary) {

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

        int count = 0;
        int max = manifest->size();

        status.clear();
        status["text"] = "Downloading files";
        status["percent"] = 0.0f;
        status["completed"] = false;

        for (auto it = manifest->cbegin(); it != manifest->cend(); ++it) {
            
            const BString& key = *it;
            const BString& value = (*manifest)[key];

            if (key != "version") {

                status["text"] = value;
                status["percent"] = (float)++count / (float)max * 100.00;

                bool downloaded = false;

                for (int i = 0; i < MAX_RETRIES && !downloaded; ++i) {
                    downloaded = downloadFile(key, value, false);
                }

                success &= downloaded;

            }

        }

        if (downloadBinary)
            success &= installBinaryProgram();

        if (success) {
            _setup->_beehiveVersion = (*manifest)["version"];
            success &= _setup->save();
            if (success) {
                std::cerr   << "Beehive Version upgraded to " 
                            << FeebeeCam::_setup->_beehiveVersion 
                            << std::endl;
                status["completed"] = true;
                status["text"] = 
                    "Beehive version upgraded to " + 
                    FeebeeCam::_setup->_beehiveVersion +
                    " Restart your device to complete upgrade";
            }
            else {
                status["text"] = "Error saving new beehive version";
                cerr << status["text"];
            }
        }
        else
            status["text"] = "Error downloading files";

        return success;
        
    }

    bool  downloadFile(BString source, BString destination, bool print) {

        bool downloaded = false;

        if (SPIFFS.exists("/tmp.txt"))
            SPIFFS.remove("/tmp.txt");

        File file = SPIFFS.open("/tmp.txt", FILE_WRITE);

        FeebeeCam::BeeFishWebRequest request(source);

        size_t size = 0;

        if (source.endsWith("time-zones.json")) {
            cerr << "SET PRINT TO TRUE FOR time-zones.json" << endl;
            print = true;
        }

        request.setOnData(
            [&file, &size, &print] (const BeeFishBString::Data& data) {

                if (print)
                    Serial.write(data._data, data.size());

                size += file.write(data._data, data.size());

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
            RESTART_AFTER_ERROR();
            return nullptr;
        }

        return webRequest.responseBody();

    }

    bool versionOutOfDate(BeeFishBScript::ObjectPointer& manifest) {

        const BString& webVersion = (*manifest)["version"];
        const BString& localVersion = _setup->_beehiveVersion;

        return webVersion != localVersion;

    }

    bool installBinaryProgram() {
        
        status["text"] = "Installing binary program";
        status["percent"] = 0.0f;

        size_t size = 0;

        bool success = true;

        Update.onProgress(
            [](size_t a, size_t b) {
                status["percent"] = (float)a / float(b) * 100.0;
                std::cerr << "{" << (float)a  / (float)b  * 100.0 << "}" << std::endl;
            }
        );

        // Send the request, trigering file write
        success &= Update.begin(UPDATE_SIZE_UNKNOWN);

        FeebeeCam::BeeFishWebRequest request("/beehive/feebee-cam.ino.bin");

        request.setOnData(
            [&size] (const BeeFishBString::Data& data) {

                size += Update.write((uint8_t*)data._data, data.size());

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

        BeeFishBScript::Object output =
            {
                {"status", true}
            };

        bool initiateDownload = false;

        if (client->_webRequest.method() == "POST") {
            
            initiateDownload = true;
        }
        else  {
            output["message"] = "0%";
        }

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "application/json";
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        stream << output.str();

        client->sendFinalChunk();

        if (initiateDownload)
            FeebeeCam::commands.push(FeebeeCam::DOWNLOAD_FILES);

        return true;

    }

}