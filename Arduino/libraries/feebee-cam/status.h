#ifndef FEEBEE_CAM__STATUS
#define FEEBEE_CAM__STATUS

#include <bee-fish.h>
#include "web-storage.h"

namespace FeebeeCam {
    
    class Status : public BeeFishBScript::Object
    {
    public:
        bool             _wakeupNextTime;
        bool             _sleeping;
        BString          _sleepTime;
        BString          _wakeupTime;
        BString          _label;
        BString          _url;
        BString          _lastWeatherURL;
        BString          _lastImageURL;
        BString          _lastImageTime;
    public:
        Status() : BeeFishBScript::Object() {

        }

        bool save() {

            using namespace BeeFishBScript;

            if (!FeebeeCam::isConnectedToInternet)
                return false;

            assign();
            
            cerr << "Saving status" << endl;

            BeeFishStorage storage = BeeFishStorage("/beehive/");

            if (!storage.setItem("status", *this)) {
                std::cerr << "Couldnt save status" << std::endl;
                return false;
            }
            
            return true;
        }

        void assign() {

            using namespace BeeFishBScript;

            (*this)["label"]                = FeebeeCam::_setup->_label;
            (*this)["wakeupNextTime"]       = _wakeupNextTime;
            (*this)["sleeping"]             = _sleeping;
            (*this)["sleepTime"]            = _sleepTime;
            (*this)["wakeupTime"]           = _wakeupTime;
            (*this)["url"]                  = _url;
            (*this)["lastWeatherURL"]       = _lastWeatherURL;
            (*this)["lastImageURL"]         = _lastImageURL;

            if (_lastImageTime.length())
                (*this)["lastImageTime"]    = _lastImageTime;
            else
                (*this)["lastImageTime"]    = undefined;

        }

        bool load() {

            using namespace BeeFishBScript;

            if (!FeebeeCam::isConnectedToInternet)
                return false;

            clear();

            FeebeeCam::BeeFishStorage storage("/beehive/");
            BeeFishBScript::Variable variable = storage.getItem("status");

            bool result = true;

            if (variable != nullptr && variable != undefined) {
                cerr << "Using status from cloud" << endl;
                apply((BeeFishBScript::ObjectPointer)variable);
            }
            else {
                cerr << "Using default status" << endl;
            }

            _label              = FeebeeCam::_setup->_label;

            _wakeupNextTime     = contains("wakeupNextTime") ?
                                    (Boolean)(*this)["wakeupNextTime"] :
                                    true;

            _sleeping           = contains("sleeping") ?
                                    (Boolean)(*this)["sleeping"] :
                                    true;

            _sleepTime          = contains("sleepTime") ?
                                    (*this)["sleepTime"] :
                                    "";

            _wakeupTime         = contains("wakeupTime") ?
                                    (*this)["wakeupTime"] :
                                    "";

            _url                = FeebeeCam::getURL();

            _lastWeatherURL     = contains("lastWeatherURL") ?
                                    (*this)["lastWeatherURL"] :
                                    "";

            _lastImageURL       = contains("lastImageURL") ?
                                    (*this)["lastImageURL"] :
                                    "";

            _lastImageTime      = contains("lastImageTime") ?
                                    (*this)["lastImageTime"] :
                                    "";
            return true;

        }


    };

    extern Status status;

    bool initializeStatus();
    
    bool onStatus(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
}

#endif