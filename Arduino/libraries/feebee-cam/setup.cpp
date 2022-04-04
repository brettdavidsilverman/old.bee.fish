#include <secure-web-server.h>
#include <file-server.h>
#include "setup.h"

namespace FeebeeCam {

    using namespace BeeFishBString;

    SecureWebServer* secureWebServer;
    
    void serveFile(const BString& filename, HTTPResponse * res);

    bool parseRequest(BeeFishJSON::JSONParser& parser, HTTPRequest* request);

    bool initializeSetupWebServer() {

        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        secureWebServer = new SecureWebServer(1);

        ResourceNode* nodeGetSettings = 
            new ResourceNode("/setup/settings", "GET", [](HTTPRequest * req, HTTPResponse * res)
            {

                Setup setup;
                BeeFishJSONOutput::Object output;

                output["label"] = setup._label;
                output["ssid"] = setup._ssid;

                res->setStatusCode(200);
                res->setStatusText("Ok");
                res->setHeader("Content-Type", "text/javascript");
                res->println(output.str().c_str());
            }
        );
 
        secureWebServer->registerNode(nodeGetSettings);

        ResourceNode* nodePostSettings = 
            new ResourceNode("/setup/settings", "POST", [](HTTPRequest * req, HTTPResponse * res)
            {

                Setup setup;
                BeeFishJSONOutput::Object output;

                using namespace BeeFishBString;
                using namespace BeeFishJSON;
                using namespace BeeFishParser;

                BeeFishJSON::Object::OnKeyValue onsetting =
                    
                    [&setup](const BString& key, JSON& json) {

                        const BString& value = json.value();

                        if (key == "label")
                            setup._label = value;
                        else if (key == "ssid")
                            setup._ssid = value;
                        else if (key == "password")
                            setup._password = value;
                        else if (key == "secretHash")
                            setup._secretHash = value;
                    };

                BeeFishJSON::JSON json;

                BeeFishJSON::JSONParser parser(json);
                json.setup(&parser);
                json._object->setOnKeyValue(onsetting);

                

                if (parseRequest(parser, req)) {
                    setup.save();
                    output["status"] = true;
                    output["message"] = "Setup complete";
                    output["redirectURL"] = HOST "/beehive";
                } else {
                    output["status"] = false;
                    output["message"] = "Setup failed";
                }

                output["label"] = setup._label;
                output["ssid"] = setup._ssid;

                res->setStatusCode(200);
                res->setStatusText("Ok");
                res->setHeader("Content-Type", "text/javascript");
                res->println(output.str().c_str());

                Serial.println("Ok");
            
            }
        );
 
        secureWebServer->registerNode(nodePostSettings);

        ResourceNode* defaultNode  = 
            new ResourceNode("", "GET", [](HTTPRequest * req, HTTPResponse * res)
            {
                std::string filename = req->getRequestString();
                serveFile(filename.c_str(), res);
            }
        );
    
        secureWebServer->setDefaultNode(defaultNode);

        secureWebServer->start();
    
        if (secureWebServer->isRunning()) {
            Serial.println("Setup Server running");
            return true;
        }

        return false;

    }

    void serveFile(const BString& filename, HTTPResponse * res) {

        BString _filename = filename;

        if ((_filename.find('.') == std::string::npos)  && !_filename.endsWith("/"))
            _filename += "/";

        if (_filename.endsWith("/"))
            _filename += "index.html";
        
        Serial.print("Getting ");
        Serial.print(_filename.c_str());
        Serial.print(" ");

        if (SPIFFS.exists(_filename.c_str())) {

            File file = SPIFFS.open(_filename.c_str(), "r");
            res->setStatusCode(200);
            res->setStatusText("Ok");

            vector<BString> parts = _filename.split('.');
            const BString& extension = parts[parts.size() - 1];
            const BString& contentType = FeebeeCam::CONTENT_TYPES[extension];

            res->setHeader("Connection", "keep-alive");
            res->setHeader("Content-Type", contentType.c_str());

            bool cacheRule = FeebeeCam::CACHE_RULES[extension];
            
            if (cacheRule)
                res->setHeader("Cache-Control", "public, max-age=31536000, immutable");
            else
                res->setHeader("Cache-Control", "no-store, max-age=0");

            std::stringstream stream;
            stream << file.size();
            res->setHeader("Content-Length", stream.str().c_str());

            // Finished headers
            size_t size = file.size();
            size_t chunkSize = getpagesize();
            size_t written = 0;
            uint8_t* buffer = (uint8_t*)malloc(chunkSize);
            while (written < size) {
                if (written + chunkSize > size)
                    chunkSize = size - written;
                written += file.read(buffer, chunkSize);
                res->write(buffer, chunkSize);
                //Serial.write(buffer, chunkSize);
            }
            file.close();
            free(buffer);
            Serial.println("Ok");
        }
        else {
            res->setStatusCode(400);
            res->setStatusText("Not Found");
            res->setHeader("Connection", "keep-alive");
            res->setHeader("Content-Type", "text/javascript");
            std::string response = "{\"status\": \"Not found\"}";
            res->write((Byte*)response.c_str(), response.size());
            Serial.println("File Not Found");
        }

    };

    bool parseRequest(BeeFishJSON::JSONParser& parser, HTTPRequest* request) {

        size_t pageSize = getpagesize();
        size_t contentLength = request->getContentLength();
        Byte* buffer = (Byte*)malloc(pageSize);
        size_t bytesRead = 0;

        while (bytesRead < contentLength) {
            
            if (bytesRead + pageSize > contentLength)
                pageSize = contentLength - bytesRead;

            bytesRead += request->readBytes(buffer, pageSize);

            BeeFishBString::Data data(buffer, pageSize);

            parser.read(data);

            if (parser.result() != BeeFishMisc::nullopt)
               break;
        }

        return (parser.result() == true);

    }


}