#include "file-server.h"

namespace FeebeeCam {

    std::map<BeeFishBString::BString, BeeFishBString::BString> CONTENT_TYPES = {
        {"html", "text/html; charset=utf-8"},
        {"txt", "text/plain]; charset=utf-8"},
        {"js", "text/javascript; charset=utf-8"},
        {"json", "application/json; charset=utf-8"},
        {"jpg", "image/jpeg"},
        {"gif", "image/gif"}
    };

    std::map<BeeFishBString::BString, bool> CACHE_RULES = {
        {"html", false},
        {"txt", false},
        {"js", false},
        {"jpg", true},
        {"json", false},
        {"gif", true}
    };

    bool serveFile(const BString& path,  FeebeeCam::WebClient* client) {

        BString filename = path;


        if ((filename.find('.') == BString::npos)  && !filename.endsWith("/"))
            filename += "/";

        if (filename.endsWith("/"))
            filename += "index.html";
        
        std::string _filename = filename.str();

        Serial.print("Getting ");
        Serial.print(_filename.c_str());
        Serial.print("...");
        
        BeeFishBString::BStream& output = client->getOutputStream();

        if (SPIFFS.exists(_filename.c_str())) {

            File file = SPIFFS.open(_filename.c_str(), "r");
            size_t size = file.size();

            vector<BString> parts = filename.split('.');
            const BString& extension = parts[parts.size() - 1];

            client->_contentType = CONTENT_TYPES[extension];
            client->_contentLength = size;

            bool cacheRule = CACHE_RULES[extension];

            //cacheRule = false;
 
             if (cacheRule)
                client->_cacheControl = "max-age=31536000, immutable";
            else
                client->_cacheControl =  "no-store, max-age=0";


            // Finished headers
            client->sendHeaders();

            size_t chunkSize = getPageSize();
            size_t written = 0;
            Data data = Data::create();

            while (written < size) {
                if (written + chunkSize > size)
                    chunkSize = size - written;
                file.read(data._readWrite, chunkSize);
                output.write((const char*)data._readWrite, chunkSize);
#ifdef DEBUG            

//                std::cerr.write((const char*)data._data, chunkSize);
#endif                
                written += chunkSize;
            }

            file.close();
            Serial.println("Ok");
        }
        else {
            return false;
        }

        client->flush();

        return true;

    };

    bool onFileServer(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

        BString redirect = "http://10.10.1.1/setup";
        BString localIP = client->_client.localIP().toString().c_str();
        BeeFishBString::BStream& stream = client->getOutputStream();

        //if (!FeebeeCam::_setup->_isSetup && !path.startsWith("http://10.10.1.1")) {
        if (!serveFile(path, client)) {
            stream << 
                "HTTP/1.1 404 Not Found\r\n" <<
                "Connection: keep-alive\r\n" <<
                "Content-Type: application/json\r\n" <<
                "\r\n" << 
                "{\"status\": \"Not found\"}\r\n";
            Serial.println("File Not Found");
            return true;
        }

        return true;

    }
}
