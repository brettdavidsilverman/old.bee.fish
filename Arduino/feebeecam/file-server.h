#include "web-server-helpers.h"

namespace FeebeeCam {

    std::map<BeeFishBString::BString, BeeFishBString::BString> CONTENT_TYPES = {
        {"html", "text/html; charset=utf-8"},
        {"txt", "text/plain]; charset=utf-8"},
        {"js", "text/javascript; charset=utf-8"},
        {"jpg", "image/jpeg"},
        {"gif", "image/gif"}
    };

    void onFileServer(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res) {
        using namespace BeeFishBString;

        BString filename = req->getRequestString();

        if (filename == "/")
            filename = "/index.html";
            
        Serial.print("Getting ");
        Serial.print(filename.c_str());
        File file = SPIFFS.open(filename.c_str(), "r");
        if (file) {

            res->setStatusCode(200);

            vector<BString> parts = filename.split('.');
            const BString& extension = parts[parts.size() - 1];
            const BString& contentType = CONTENT_TYPES[extension];
            res->setHeader("Connection", "keep-alive");
            res->setHeader("Content-Type", contentType.c_str());

            size_t size = file.size();
            size_t chunkSize = getpagesize();
            size_t read = 0;
            uint8_t * nbuf = (uint8_t*)malloc(chunkSize);
            while (read < size) {
                if (read + chunkSize > size)
                    chunkSize = size - read;
                read += file.read(nbuf, chunkSize);
                res->write(nbuf, chunkSize);
            }
            file.close();
            free(nbuf);
            Serial.println("Ok");
        }
        else {
            res->setStatusCode(404);
            res->println("{\"status\": \"Not found\"}");
            Serial.println("File Not Found");
        }
    }
}