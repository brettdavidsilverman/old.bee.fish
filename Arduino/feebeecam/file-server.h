#include <wifi-web-server.h>

namespace FeebeeCam {

    std::map<BeeFishBString::BString, BeeFishBString::BString> CONTENT_TYPES = {
        {"html", "text/html; charset=utf-8"},
        {"txt", "text/plain]; charset=utf-8"},
        {"js", "text/javascript; charset=utf-8"},
        {"jpg", "image/jpeg"},
        {"gif", "image/gif"}
    };

    bool onFileServer(BeeFishWeb::WebRequest& request, WiFiClient& client) {
        using namespace BeeFishBString;

        BString filename = request.path();

        if (filename == "/")
            filename = "/index.html";
            
        Serial.print("Getting ");
        Serial.print(filename.c_str());
        File file = SPIFFS.open(filename.c_str(), "r");
        if (file) {

            client.println("HTTP/1.1 200 OK");

            vector<BString> parts = filename.split('.');
            const BString& extension = parts[parts.size() - 1];
            const BString& contentType = CONTENT_TYPES[extension];
            client.println("Connection: keep-alive");
            const BString contentTypeHeader = 
                BString("Content-Type") + ": " + contentType;
            client.println(contentTypeHeader.c_str());
            client.println();
            size_t size = file.size();
            size_t chunkSize = getpagesize();
            size_t read = 0;
            uint8_t * nbuf = (uint8_t*)malloc(chunkSize);
            while (read < size) {
                if (read + chunkSize > size)
                    chunkSize = size - read;
                read += file.read(nbuf, chunkSize);
                client.write(nbuf, chunkSize);
            }
            file.close();
            free(nbuf);
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