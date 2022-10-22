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
        public Match,
        public BeeFishBString::BStream
    {
    public:
        BeeFishWeb::ContentLength* _contentLength = nullptr;
        BeeFishJSON::Object* _json = nullptr;
        std::vector<Byte> _bytes;

    public:
        Body() : Match() {

        }

        virtual void setup(Parser* parser, Headers*  headers) {
            BString contentType;
            
            if ( headers->contains("content-type")) {
                contentType = (*headers)["content-type"];
            }

            if (contentType.startsWith("application/json") ) {
                _json = new BeeFishJSON::Object();
                _match = _json;
            }
            else if (headers->contains("content-length") ) {
                std::string contentLengthString = (*headers)["content-length"].str();
                size_t contentLength = atoi(contentLengthString.c_str());
                if (contentLength > 0) {
                    _contentLength = new ContentLength(contentLength);
                    parser->setDataBytes(contentLength);
                    _match = _contentLength;
                    _bytes.reserve(contentLength);
                }
            }

            Match::setup(parser);

        }

        virtual ~Body() {
        }
    
        const std::vector<Byte>& body() const {
            return _bytes;
        }

        virtual bool hasJSON() const {
            return _json && _json->matched();            
        }

        virtual bool matchCharacter(const Char& character) {

            if (Match::matchCharacter(character)) {
                _bytes.push_back((Byte)character);
                BeeFishBString::BStream::push_back(character);
                return true;
            }

            return false;
        }
    };
}

#endif