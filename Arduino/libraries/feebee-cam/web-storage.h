#pragma once
#include <bee-fish.h>
#include "bee-fish-web-request.h"

namespace FeebeeCam {

    class BeeFishStorage : public BeeFishWebRequest {
    public:
        static bool setItem(const BString& path, const BString& key, const BeeFishBScript::Object& value) {
            
            BeeFishWebRequest request(path, "", true);
            BeeFishBScript::Object& body = request.body();

            body.clear();

            body["method"] = "setItem";
            body["key"] = key;
            body["value"] = value.bstr();

            return request.send();
        }

        static bool getItem(const BString& path, const BString& key, BeeFishBScript::Object& value) {
            BeeFishWebRequest request(path, "", true);
            BeeFishBScript::Object& body = request.body();

            body.clear();

            body["method"] = "getItem";
            body["key"] = key;

            if (request.send()) {
                value = request.body();
                return true;
            }

            return false;
            
        }
        
    };

}