#include "feebee-cam.h"

namespace FeebeeCam {


    Status status;

    bool initializeStatus() {
        if (!status.load()) {
            RESTART_AFTER_ERROR();
        }

        return true;
    }


    bool onStatus(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "application/json; charset=utf-8";
        client->_chunkedEncoding = true;
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        status.assign();

        stream << status;

        client->sendFinalChunk();

        FeebeeCam::resetCameraWatchDogTimer();

        return true;
    }

}