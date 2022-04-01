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
        {"html", false},
        {"txt", false},
        {"js", true},
        {"jpg", true},
        {"json", false},
        {"gif", true}
    };

   bool onFileServer(BeeFishWeb::WebRequest& request, WiFiClient& client) {
        using namespace BeeFishBString;

        BString filename = request.path();

        if (filename.endsWith("/"))
            filename += "index.html";
        
        Serial.print("Getting ");
        Serial.print(filename.c_str());
        Serial.print(" ");
        
        if (SPIFFS.exists(filename.c_str())) {

            File file = SPIFFS.open(filename.c_str(), "r");
            client.println("HTTP/1.1 200 OK");

            vector<BString> parts = filename.split('.');
            const BString& extension = parts[parts.size() - 1];
            const BString& contentType = CONTENT_TYPES[extension];
            client.println("Connection: keep-alive");
            const BString contentTypeHeader = 
                BString("Content-Type") + ": " + contentType;
            client.println(contentTypeHeader.c_str());

            bool cacheRule = CACHE_RULES[extension];
            
            if (cacheRule)
                client.println("Cache-Control: public, max-age=31536000, immutable");
            else
                client.println("Cache-Control: no-store, max-age=0");

            client.print("Content-Length: ");
            client.println(file.size());

            // Finished headers
            client.println();
            size_t size = file.size();
            size_t chunkSize = getpagesize();
            size_t written = 0;
            uint8_t* buffer = (uint8_t*)malloc(chunkSize);
            while (written < size) {
                if (written + chunkSize > size)
                    chunkSize = size - written;
                written += file.read(buffer, chunkSize);
                client.write(buffer, chunkSize);
                Serial.write(buffer, chunkSize);
            }
            cout << endl << " wrote " << written << " from " << file.size() << endl;
            file.close();
            free(buffer);
            Serial.println("Ok");
        }
        else {
            client.println("HTTP/1.1 404 Not Found");
            client.println("Connection: keep-alive");
            client.println("Content-Type: text/javascript");
            client.println();
            client.println("{\"status\": \"Not found\"}");
            Serial.println("File Not Found");
        }

        return true;
    }
}
