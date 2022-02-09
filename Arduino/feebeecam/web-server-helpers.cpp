#include "web-server-helpers.h"

namespace FeebeeCam {
    
    bool parseRequest(BeeFishParser::Parser& parser, httpsserver::HTTPRequest* req) {

        size_t read = 0;
        size_t size = req->getContentLength();
        size_t chunkSize = getpagesize();
        byte*  buffer = (byte*)malloc(chunkSize);

        while (read < size) {
            
            if (read + chunkSize > size)
                chunkSize = size - read;
            
            read += req->readBytes(buffer, chunkSize);
            
            const BeeFishBString::Data data(buffer, chunkSize);

            if (!parser.read(data)) {
                return false;
            }
        }

        free(buffer);

        return (parser.result() == true);

    }

    void setDefaultHeaders(httpsserver::HTTPResponse* res) {
        res->setHeader("Connection", "keep-alive");
        res->setHeader("Content-Type", "text/javascript; charset=utf-8");
    }

    void sendChunk(httpsserver::HTTPResponse* response, const byte* chunk, size_t length) {
        
        response->println(length, HEX);
        
        if (chunk)
            response->write(chunk, length);
        
        response->println();

    }

    void sendResponse(httpsserver::HTTPResponse* response, const BeeFishJSONOutput::Object& output) {

        setDefaultHeaders(response);

        BeeFishBString::DataStream stream;

        stream.setOnBuffer(
            [&response](const Data& buffer) {
                sendChunk(response, buffer.data(), buffer.size());
            }
        );

        stream << output;

        stream.flush();

        sendChunk(response, nullptr, 0);

    }
}
