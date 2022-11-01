#pragma once
#include <bee-fish.h>
#include "bee-fish-web-request.h"

namespace FeebeeCam {

    class BeeFishStorage : public BeeFishWebRequest {
    protected:
        BeeFishBScript::BScriptParser* _parser = nullptr;
    public:

        BeeFishStorage(const BString& path) :
            BeeFishWebRequest(path, "", true) {

        }
        
        virtual ~BeeFishStorage() {
            if (_parser)
                delete _parser;
            _parser = nullptr;
        }

        virtual BeeFishBScript::Variable getItem(const BString& key) {

            _query = "?key=" + key.encodeURI();
            
            return getItem();
        }

        virtual BeeFishBScript::Variable getItem(BeeFishId::Id& id) {

            _query = "?id=" + id.key().encodeURI();

            return getItem();
            
        }

    protected:
        virtual BeeFishBScript::Variable getItem() {

            _hasBody = false;
            _method = "GET";

            bool result = send();

            if (statusCode() == 404)
                return nullptr;
                
            if (result)
                return responseBody();

            return undefined;
        }


    public:
        virtual bool setItem(const BString& key, const BeeFishBScript::Object& value) {
            BeeFishBScript::Variable variable = value;
            return setItem(key, variable);
        }

        virtual bool setItem(BeeFishId::Id& id, const BeeFishBScript::Object& value) {
            BeeFishBScript::Variable variable = value;
            return setItem(id, variable);
        }

        virtual bool setItem(const BString& key, const BeeFishBScript::Variable& value) {
            
            _query = "?key=" + key.encodeURI();
            return setItem(value);
           
        }

        virtual bool setItem(BeeFishId::Id& id, const BeeFishBScript::Variable& value) {
            _query = "?id=" + id.key().encodeURI();            
            return setItem(value);
        }

        virtual bool setItem(BeeFishId::Id& id, const BString& contentType, const Data& data) {
            _query = "?id=" + id.key().encodeURI();
            return setItem(contentType, data);
        }

        virtual bool setItem(const BString& key, const BString& contentType, const Data& data) {
            _query = "?key=" + key.encodeURI();
            return setItem(contentType, data);
        }

    protected:
        virtual bool setItem(const BeeFishBScript::Variable& value) {
            _method = "POST";

            std::cerr << "Posting json to " << path() << std::endl;

            if (!authenticate()) {
                std::cerr << "Unauthenticated" << std::endl;
                return false;
            }

            // make a HTTP request:
            // send HTTP header
            BeeFishBString::BStream stream = _connection->getStream();

            if (!sendDefaultHeaders(stream))
                return false;

            size_t contentLength = value.contentLength();
            
            stream << "content-length: " << contentLength << "\r\n";
            stream << "content-type: application/json; charset=utf-8" << "\r\n";

            stream << "\r\n"; // End Headers

            stream << value;

            stream.flush();

            return readResponse();

        }

        virtual bool setItem(const BString& contentType, const Data& data) {

            _method = "POST";
            std::cerr << "Posting data to " << path() << std::endl;

            if (!authenticate())
                return false;

            // make a HTTP request:
            // send HTTP header
            BeeFishBString::BStream stream = _connection->getStream();

            if (!sendDefaultHeaders(stream))
                return false;

            size_t dataSize = data.size();

            stream << "content-type: " << contentType << "\r\n";
            stream << "content-length: " << dataSize << "\r\n";

            stream << "\r\n"; // End Headers

            // Write the image page size at a time
            size_t bufferSize = getPageSize();

            for ( size_t written = 0;
                  written < dataSize;
                ) 
            {
                if (written + bufferSize > dataSize)
                    bufferSize = dataSize - written;

                
                stream.write((const char*)(data._data + written), bufferSize);

                written += bufferSize;

            }

            stream.flush();

            return readResponse();
        }



    };

}