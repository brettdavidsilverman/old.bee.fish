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
        {"html", true},
        {"txt", false},
        {"js", true},
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
            output << "HTTP/1.1 200 OK\r\n";

            vector<BString> parts = filename.split('.');
            const BString& extension = parts[parts.size() - 1];
            const BString& contentType = CONTENT_TYPES[extension];
            output << "Connection: keep-alive\r\n";
            BString contentTypeHeader = 
                BString("Content-Type") + ": " + contentType;
            output << contentTypeHeader << "\r\n";

            bool cacheRule = CACHE_RULES[extension];

            cacheRule = false;

            if (cacheRule)
                output << "Cache-Control: public, max-age=31536000, immutable\r\n";
            else
                output << "Cache-Control: no-store, max-age=0\r\n";

            output << "Content-Length: " << file.size() << "\r\n";

            // Finished headers
            output << "\r\n";
            size_t size = file.size();
            size_t chunkSize = getPageSize();
            size_t written = 0;
            uint8_t* buffer = (uint8_t*)malloc(chunkSize);
            while (written < size) {
                if (written + chunkSize > size)
                    chunkSize = size - written;
                file.read(buffer, chunkSize);
                output.write((const char*)buffer, chunkSize);
                //std::cerr.write((const char*)buffer, chunkSize);
                written += chunkSize;
            }
            file.close();
            free(buffer);
            Serial.println("Ok");
        }
        else {
            output << 
                "HTTP/1.1 404 Not Found\r\n" <<
                "Connection: keep-alive\r\n" <<
                "Content-Type: application/json\r\n" <<
                "\r\n" << 
                "{\"status\": \"Not found\"}\r\n";
            Serial.println("File Not Found");
        }

        output.flush();

        return true;

    };

    bool onFileServer(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

        BString redirect = "http://10.10.1.1/setup";

        if (false && !FeebeeCam::_setup->_isSetup && !path.startsWith("http://10.10.1.1")) {

            std::cerr << "Redireccting all trafic to " << redirect << std::endl;
            BeeFishBString::BStream& stream = client->getOutputStream();
            stream << "HTTP/1.1 " << 302 << " " << "Found" << "\r\n"
                    "server: FeebeeCam server" <<  "\r\n" <<
                    "location: " << redirect << "\r\n" <<
                    "\r\n";
            stream.flush();
            return true;
        }
        else {
            return serveFile(path, client);
        }

    }
}
