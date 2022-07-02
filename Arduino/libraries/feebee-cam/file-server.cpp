#include "file-server.h"

namespace FeebeeCam {

    std::map<BeeFishBString::BString, BeeFishBString::BString> CONTENT_TYPES = {
        {"html", "text/html; charset=utf-8"},
        {"txt", "text/plain]; charset=utf-8"},
        {"js", "text/javascript; charset=utf-8"},
        {"json", "text/javascript; charset=utf-8"},
        {"jpg", "image/jpeg"},
        {"gif", "image/gif"}
    };

    std::map<BeeFishBString::BString, bool> CACHE_RULES = {
        {"html", true},
        {"txt", false},
        {"js", true},
        {"jpg", true},
        {"json", false},
        {"gif", true}
    };

    bool serveFile(const BString& path,  BeeFishWebServer::WebClient* client) {

        BString filename = path;

        if ((filename.find('.') == std::string::npos)  && !filename.endsWith("/"))
            filename += "/";

        if (filename.endsWith("/"))
            filename += "index.html";
        
        Serial.print("Getting ");
        Serial.print(filename.c_str());
        Serial.println();

        BeeFishBString::BStream output = client->getOutputStream();

        if (SPIFFS.exists(filename.c_str())) {

            File file = SPIFFS.open(filename.c_str(), "r");
            output << "HTTP/1.1 200 OK\r\n";

            vector<BString> parts = filename.split('.');
            const BString& extension = parts[parts.size() - 1];
            const BString& contentType = CONTENT_TYPES[extension];
            output << "Connection: close\r\n";
            const BString contentTypeHeader = 
                BString("Content-Type") + ": " + contentType;
            output << contentTypeHeader.c_str() << "\r\n";

            //bool cacheRule = CACHE_RULES[extension];
            
            //if (cacheRule)
            //    output << "Cache-Control: public, max-age=31536000, immutable\r\n";
            //else
                output << "Cache-Control: no-store, max-age=0\r\n";

            output << "Content-Length: " << file.size() << "\r\n";

            // Finished headers
            output << "\r\n";
            size_t size = file.size();
            size_t chunkSize = getpagesize();
            size_t written = 0;
            uint8_t* buffer = (uint8_t*)malloc(chunkSize);
            while (written < size) {
                if (written + chunkSize > size)
                    chunkSize = size - written;
                file.read(buffer, chunkSize);
                output.write((const char*)buffer, chunkSize);
            }
            file.close();
            free(buffer);
        }
        else {
            output << 
                "HTTP/1.1 404 Not Found\r\n" <<
                "Connection: keep-alive\r\n" <<
                "Content-Type: text/javascript\r\n" <<
                "\r\n" << 
                "{\"status\": \"Not found\"}\r\n";
            Serial.println("File Not Found");
        }

        output.flush();

        return true;

    };

    bool onFileServer(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client) {
        return serveFile(path, client);
    }
}
