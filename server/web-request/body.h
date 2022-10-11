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
        public Match
    {
    public:
        BeeFishWeb::ContentLength* _contentLength = nullptr;
        BeeFishJSON::JSON* _json = nullptr;

    public:
        Body() : Match() {

        }

        virtual void setup(Parser* parser, Headers*  headers) {

            if ((*headers)["content-type"].startsWith("application/json") ) {
                _json = new BeeFishJSON::JSON();
                _json->setup(parser);
                _match = _json;
            }
            else if (headers->contains("content-length") ) {
                _contentLength = new ContentLength();
                _contentLength->setup(parser, headers);
                _match = _contentLength;
            }

            Match::setup(parser);

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