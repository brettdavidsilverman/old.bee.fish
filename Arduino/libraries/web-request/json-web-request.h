#ifndef FEEBEECAM__JSON_WEB_REQUEST
#define FEEBEECAM__JSON_WEB_REQUEST
#include <bee-fish.h>
#include "web-request-base.h"

#define TAG "JSONWebRequest"

namespace FeebeeCam {

    class JSONWebRequest : public WebRequest {
    protected:
        BeeFishJSON::JSON* _json;
        BeeFishJSON::JSONParser* _parser;
    public:

        JSONWebRequest(
            BString host,
            BString path,
            BString query,
            BeeFishMisc::optional<BString> body = BeeFishMisc::nullopt
        ) :
            WebRequest(host, path, query, body),
            _json(nullptr),
            _parser(nullptr)
        {
        }

        virtual ~JSONWebRequest() {
            if (_json)
                delete _json;
            if (_parser)
                delete _parser;
        }

        bool expectJSON() {
            return hasBody();
        }

        virtual void ondata(const BeeFishBString::Data& data) {
            if (expectJSON() && _parser->result() == BeeFishMisc::nullopt) {
                _parser->read(data);
            }
            WebRequest::ondata(data);
        }

        BeeFishJSON::JSONParser& parser() {
            return *_parser;
        }

        BeeFishJSON::JSON& json() {
            return *_json;
        }

        virtual void initialize() {
            if (_json)
                delete _json;

            if (_parser)
                delete _parser;

            _json = new BeeFishJSON::JSON();
            _parser = new BeeFishJSON::JSONParser(*_json);
        }            
    };

}

#endif
