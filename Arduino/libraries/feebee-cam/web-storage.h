#pragma once
#include <bee-fish.h>
#include "bee-fish-web-request.h"

namespace FeebeeCam {

    class BeeFishStorage {
    protected:
        BeeFishWebRequest* _request = nullptr;
        BeeFishBScript::BScriptParser* _parser = nullptr;
    public:

        BeeFishStorage() {

        }
        
        virtual ~BeeFishStorage() {
            if (_request)
                delete _request;
            _request = nullptr;
            if (_parser)
                delete _parser;
            _parser = nullptr;
        }

        virtual bool setItem(const BString& path, const BString& key, const BeeFishBScript::Variable& value) {
            
            if (_request)
                delete _request;

            _request = new BeeFishWebRequest(path, "", true);
            BeeFishBScript::Object& body = _request->body();

            body.clear();

            body["method"] = "setItem";
            body["key"] = key;
            body["value"] = value.bstr();

            bool result =  _request->send();
            
            return result;

        }

        virtual bool setItem(const BString& path, BeeFishId::Id& id, const BeeFishBScript::Variable& value) {
            
            if (_request)
                delete _request;

            _request = new BeeFishWebRequest(path, "", true);

            BeeFishBScript::Object& body = _request->body();

            body.clear();

            body["method"] = "setItem";
            body["id"] = id.key();
            body["value"] = value.bstr();

            bool result =  _request->send();
            
            return result;
        }

        virtual BeeFishBScript::Variable& getItem(const BString& path, const BString& key) {

            if (_request)
                delete _request;

            _request = new BeeFishWebRequest(path, "", true);

            BeeFishBScript::Object& body = _request->body();

            body.clear();

            body["method"] = "getItem";
            body["key"] = key;

            bool result = _request->send();

            if (result) {

                BeeFishBScript::ObjectPointer objectPointer = _request->responseBody();
                                
                return parseValue(objectPointer);

            }

            return undefined;
            
        }

        virtual BeeFishBScript::Variable& getItem(const BString& path, BeeFishId::Id& id) {

            if (_request)
                delete _request;

            _request = new BeeFishWebRequest(path, "", true);

            BeeFishBScript::Object& body = _request->body();

            body.clear();

            body["method"] = "getItem";
            body["id"] = id.key();

            bool result = _request->send();
            
            if (result) {
                return parseValue(_request->responseBody());
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