#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <bee-fish.h>

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

}
