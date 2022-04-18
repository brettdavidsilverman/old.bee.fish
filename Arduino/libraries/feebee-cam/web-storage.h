#pragma once
#include <bee-fish.h>
#include "bee-fish-web-request.h"

namespace FeebeeCam {

    class BeeFishStorage : public BeeFishWebRequest {
    public:
        static bool setItem(BString path, BString key, BeeFishJSONOutput::Object& value) {
            
            BeeFishWebRequest request(path, "", true);
            BeeFishJSONOutput::Object& body = request.body();

            body.clear();

            body["method"] = "setItem";
            body["key"] = key;
            body["value"] = value.bstr();

            return request.send();
        }
        
    };

}