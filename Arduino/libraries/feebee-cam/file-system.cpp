#include <iostream>
#include "FS.h"
#include <map>
#include <WiFi.h>
#include <ArduinoOTA.h>
extern "C" {
esp_err_t esp_spiffs_format(const char* partition_label);
}
#include "web-request.h"
#include "esp-memory.h"
#include "commands.h"
#include "file-system.h"
#include "setup.h"

#define TEMP_FILE_NAME "/temp.txt"
#define MAX_RETRIES 5

namespace FeebeeCam {

    BeeFishBScript::Object downloadStatus;
    
    BeeFishBScript::ObjectPointer getManifest();
    bool versionOutOfDate(BeeFishBScript::ObjectPointer& manifest);
    bool installBinaryProgram();

    bool initializeFileSystem() {

        Serial.println("Initializing file system...");

        if (!SPIFFS.begin(true)) {
            cerr << "Formatting SPIFFS failed!" << endl;
            return false;
        }

        cerr << "File system initialized" << endl;

        return true;
    }

    bool downloadFiles(bool overrideVersion, bool downloadBinary) {

        Serial.println("Checking if we should download files");

        BeeFishBScript::ObjectPointer manifest = getManifest();

        if (manifest == nullptr)
            return false;

        bool isVersionOutOfDate = versionOutOfDate(manifest);

        if (isVersionOutOfDate)
            std::cerr << "Download required" << std::endl;
        else {
            std::cerr << "No download required" << std::endl;
            if (!overrideVersion)
                return true;
        }


        Serial.println("Downloading beehive files");

        bool success = true;

        int count = 0;
        int max = manifest->size() - 1; // Remove 1 from size because we
                                        // dont use include the version

        downloadStatus.clear();
        downloadStatus["text"] = "Downloading files";
        downloadStatus["percent"] = 0.0f;
        downloadStatus["completed"] = false;

        for (auto it = manifest->cbegin(); it != manifest->cend(); ++it) {
            
            const BString& key = *it;
            const BString& value = (*manifest)[key];

            if (key == "version")
                continue;

            downloadStatus["text"] = value;
            downloadStatus["percent"] = (float)++count / (float)max * 100.00;

            BString source = key;
            BString destination = value;

            bool downloaded = false;
            
            cerr << "Downloading " << source << " " << std::flush;

            for (int i = 0; i < MAX_RETRIES && !downloaded; ++i) {
                
                downloaded = downloadFile(source, destination, false);
                delay(1);

            }

            if (!downloaded)
                break;

        }

        if (downloadBinary)
            success &= installBinaryProgram();

        if (success) {
            _setup->_version = (*manifest)["version"];
            success &= _setup->save();
            if (success) {
                std::cerr   << "Beehive Version upgraded to " 
                            << FeebeeCam::_setup->_version 
                            << std::endl;
                downloadStatus["completed"] = true;
                downloadStatus["text"] = 
                    "Beehive version upgraded to " + 
                    FeebeeCam::_setup->_version +
                    " Restart your device to complete upgrade";
            }
            else {
                downloadStatus["text"] = "Error saving new beehive version";
                cerr << downloadStatus["text"];
            }
        }
        else
            downloadStatus["text"] = "Error downloading files";

        return success;
        
    }

    bool downloadFile(BString source, BString destination, bool print) {

        bool downloaded = false;

        if (SPIFFS.exists("/tmp.txt"))
            SPIFFS.remove("/tmp.txt");

        File file = SPIFFS.open("/tmp.txt", FILE_WRITE);

        FeebeeCam::BeeFishWebRequest request(source);

        size_t size = 0;

        if (source.endsWith("time-zones.json")) {
            
            // Handle time-zones specially because the file is too large
            // to be parsed by an ESP32.
            request._parseJSON = false;

            //cerr << "SET PRINT TO TRUE FOR time-zones.json" << endl;
            //print = true;
        }

        request.setOnData(
            [&file, &size, &print] (const BeeFishBString::Data& data) {

                if (print)
                    cerr.write((const char*)data._data, data.size());

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
            std::string _destination = destination.str();
            // Move file from temp to proper file path
            if (SPIFFS.exists(_destination.c_str()))
                SPIFFS.remove(_destination.c_str());
            SPIFFS.rename("/tmp.txt", _destination.c_str());
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

        //const BString& webVersion = ;
        const BString& webVersion = (*manifest)["version"];
        const BString& localVersion = BEEHIVE_VERSION;;

        cerr << "Local Version: " << localVersion << endl;
        cerr << "Web Version:   " << webVersion << endl;

        return webVersion != localVersion;

    }

    bool installBinaryProgram() {
        
        downloadStatus.clear();
        downloadStatus["text"] = "Installing binary program";
        downloadStatus["percent"] = 0.0f;
        downloadStatus["completed"] = false;

        size_t written = 0;

        bool success = true;
/*
        Update.onProgress(
            [](size_t a, size_t b) {
                downloadStatus["percent"] = (float)a / float(b) * 100.0;
                std::cerr << "{" << (float)a  / (float)b  * 100.0 << "}" << std::endl;
            }
        );
*/
        // Send the request, trigering file write
        success &= Update.begin(UPDATE_SIZE_UNKNOWN);

        FeebeeCam::BeeFishWebRequest request("/beehive/feebee-cam.ino.bin");

        FeebeeCam::downloadStatus["completed"] = false;

        request.setOnData(
            [&written, &request] (const BeeFishBString::Data& data) {
//                std::cerr << "{" << (float)a  / (float)b  * 100.0 << "}" << std::endl;
                size_t size = request.webResponse().contentLength();
                written += Update.write((uint8_t*)data._data, data.size());
                if (written == size) {
                    downloadStatus["percent"] = 100.0;
                    downloadStatus["completed"] = true;
                }
                else
                    downloadStatus["percent"] = (float)written / (float)size * 100.0;
                delay(1);

            }
        );

        // Send the request, trigering file write
        cerr << "Sending request... " << std::flush;
        success = (request.send() == request.webResponse().contentLength());
        
        if (success) {
            cerr << "Ending Update..." << std::flush;
            success = Update.end(true);
        }


        if (success)
            std::cerr << "Finished Update" << std::endl; 
        else
            std::cerr << "Error updating binary program" << std::endl;

        return success;

    }
/*
    bool onDownloadFiles(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

        BeeFishBScript::Object output {
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
        client->_chunkedEncoding = true;
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        stream << output.str();

        client->sendFinalChunk();

        if (initiateDownload)
            FeebeeCam::commands.push(FeebeeCam::DOWNLOAD_FILES);

        return true;

    }
*/
    bool onDownloadStatus(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "application/json; charset=utf-8";
        client->_chunkedEncoding = true;
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        stream << downloadStatus;

        client->sendFinalChunk();

        FeebeeCam::resetCameraWatchDogTimer();

        return true;
    }
}