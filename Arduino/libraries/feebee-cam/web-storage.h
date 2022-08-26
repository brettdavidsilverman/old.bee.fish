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

       virtual bool setItem(BeeFishId::Id& id, camera_fb_t* image) {

            _method = "POST";
            _query = "?id=" + id.key();
            

            cout << "Uploading image to " << url() << endl;

            if (!openConnection())
                return false;

            // make a HTTP request:
            // send HTTP header
            BeeFishBString::BStream stream = _connection->getStream();

            if (!sendDefaultHeaders(stream))
                return false;

            stream << "Content-Type: image/jpeg" << "\r\n";
            stream << "Content-Length: " << image->len << "\r\n";
            stream << "\r\n"; // End Headers

            // Write the image page size at a time
            size_t bufferSize = getPageSize();

            for ( size_t written = 0;
                  written < image->len;
                   ) 
            {
                if (written + bufferSize > image->len)
                    bufferSize = image->len - written;

                stream.write((const char*)(image->buf + written), bufferSize);

                written += bufferSize;
                
                cerr << ((float)written / (float)image->len) * 100.0 << endl;
            }

            stream.flush();

            cout << "Uploading image to " << url() << endl;

            return readResponse();
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