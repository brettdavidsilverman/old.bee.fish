#pragma once
#include <bee-fish.h>
#include "bee-fish-web-request.h"

namespace FeebeeCam {

    class BeeFishStorage : public BeeFishWebRequest {
    protected:
        BeeFishBScript::BScriptParser* _parser = nullptr;
        BeeFishBString::BString _path;
    public:

        BeeFishStorage(const BString& path) :
            BeeFishWebRequest(path, "", true) {

        }
        
        virtual ~BeeFishStorage() {
            if (_parser)
                delete _parser;
            _parser = nullptr;
        }

        virtual bool setItem(const BString& key, const BeeFishBScript::Variable& value) {
            
            BeeFishBScript::Object& body = BeeFishWebRequest::body();

            body.clear();

            body["method"] = "setItem";
            body["key"] = key;
            body["value"] = value.bstr();

            bool result =  send();
            
            if (!result)
                cerr << "Error saving to web storage with key \"" << key << "\"" << endl;

            return result;

        }

        virtual bool setItem(BeeFishId::Id& id, const BeeFishBScript::Variable& value) {
            
            BeeFishBScript::Object& body = BeeFishWebRequest::body();

            body.clear();

            body["method"] = "setItem";
            body["id"] = id.key();
            body["value"] = value.bstr();

            bool result =  send();
            
            return result;
        }

        virtual bool setItem(BeeFishId::Id& id, const BeeFishBScript::Object& value) {
            BeeFishBScript::Variable variable = value;
            return setItem(id, variable);
        }

        virtual bool setItem(BeeFishId::Id& id, const Data& data) {
            cerr << "SET ITEM FOR IMAGE" << endl;

            _method = "POST";
            _query = "?id=" + id.key();
            
            const BString contentType = "image/jpeg";
            
            std::cerr << "Uploading image to " << url() << std::endl;

            if (!openConnection())
                return false;

            // make a HTTP request:
            // send HTTP header
            BeeFishBString::BStream stream = _connection->getStream();

            if (!sendDefaultHeaders(stream))
                return false;

            stream << "content-type: " << contentType << "\r\n";
            stream << "content-length: " << data.size() << "\r\n";
            stream << "\r\n"; // End Headers

            // Write the image page size at a time
            size_t bufferSize = getPageSize();

            for ( size_t written = 0;
                  written < data.size();
                ) 
            {
                if (written + bufferSize > data.size())
                    bufferSize = data.size() - written;

                stream.write((const char*)(data._data + written), bufferSize);

                written += bufferSize;
                
                cerr << ((float)written / (float)data.size()) * 100.0 << endl;
            }

            stream.flush();

            bool success = readResponse();

            if (success && WebRequest::_parser->_stack.size() > 0) {
                BeeFishBScript::ObjectPointer object = WebRequest::_parser->json();
                if ((*object)["response"] == "ok") {
                    std::cerr << "Image upload success" << std::endl;
                    return true;
                }
            }

            std::cerr << "Error uploading image" << std::endl;

            return false;   
        }


        virtual BeeFishBScript::Variable& getItem(const BString& key) {

            BeeFishBScript::Object& body = BeeFishWebRequest::body();

            body.clear();

            body["method"] = "getItem";
            body["key"] = key;

            bool result = send();

            if (result) {

                BeeFishBScript::ObjectPointer objectPointer = responseBody();
                                
                return parseValue(objectPointer);

            }

            return undefined;
            
        }

        virtual BeeFishBScript::Variable& getItem(BeeFishId::Id& id) {

            BeeFishBScript::Object& body = BeeFishWebRequest::body();

            body.clear();

            body["method"] = "getItem";
            body["id"] = id.key();

            bool result = send();
            
            if (result) {
                return parseValue(responseBody());
            }

            return undefined;
            
        }

    protected:

        BeeFishBScript::Variable& parseValue(BeeFishBScript::ObjectPointer responseBody) {

            BeeFishBScript::Variable& value = (*responseBody)["value"];

            if (value == nullptr)
                return value;

            BString& string = value;

            if (_parser)
                delete _parser;

            BeeFishJSON::JSON json;

            _parser = new BeeFishBScript::BScriptParser(json);

            if (_parser->read(string) == true) {
                return _parser->value();
            }

            return undefined;

        }
    };

}