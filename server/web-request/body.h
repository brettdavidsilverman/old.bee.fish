#ifndef BEE_FISH_WEB__BODY_H
#define BEE_FISH_WEB__BODY_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "content-length.h"
#include "headers.h"

using namespace BeeFishParser;
      
namespace BeeFishWeb {

    class Body :
        public Or
    {
    public:
        BeeFishWeb::ContentLength* _contentLength = nullptr;
        BeeFishJSON::Object* _json = nullptr;

    public:
        Body() : Or() {

        }

        virtual void setup(Parser* parser, Headers*  headers) {
            BString contentType;
            
            if ( headers->contains("content-type")) {
                contentType = (*headers)["content-type"];
            }

            if (contentType.startsWith("application/json") ) {
                _json = new BeeFishJSON::Object();
                _json->setup(parser);
                _inputs.push_back(_json);
            }
            else if (headers->contains("content-length") ) {
                _contentLength = new ContentLength();
                _contentLength->setup(parser, headers);
                _inputs.push_back(_contentLength);
            }

            Or::setup(parser);

        }

        virtual ~Body() {
        }
    
        void setOnData(BStream::OnBuffer ondata) {
            if (_contentLength)
                _contentLength->_onbuffer = ondata;
        }

        virtual void flush() {
            if (_contentLength) {
                _contentLength->flush();
            }
        }

        virtual bool hasJSON() const {
            return _json && _json->matched();            
        }

    };
}

#endif