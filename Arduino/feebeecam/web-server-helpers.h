#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <bee-fish.h>

namespace FeebeeCam {
    
    bool parseRequest(BeeFishParser::Parser& parser, httpsserver::HTTPRequest* req);

    void setDefaultHeaders(httpsserver::HTTPResponse* res);

    void sendChunk(httpsserver::HTTPResponse* response, const byte* chunk, size_t length);

    void sendResponse(httpsserver::HTTPResponse* response, const BeeFishJSONOutput::Object& output);

}
