#include "serial.h"
#include "settings.h"
#include "wifi.h"
#include "file-system.h"
#include "weather.h"
#include "setup.h"

namespace FeebeeCam {

    bool initializeSerial() {
        
        Serial.begin(1500000);

        while (!Serial)
            delay(10);

        Serial.println("FeebeeCam starting");

        return true;
    }

    bool handleCommandLine() {

        if (!Serial.available())
            return false;

        String readString = Serial.readString();
        BString line = readString.c_str();

        while (Serial.available())
            Serial.read();

        if (line == "download") {
            FeebeeCam::downloadWhenReady = true;
        }
        else if (line == "save") {
            settings.save();
        }
        else if (line == "settings") {
            cout << settings << endl;
        }
        else if (line.startsWith("file")) {
            BString file = line.substr(line.find(' ') + 1);
            if (downloadFile(file, "/tmp.txt", true))
                cout << "File downloaded as /tmp.txt" << endl;
        }
        else if (line == "weather") {
            const BeeFishBScript::Object& object = weather.getWeather();
            cout << object << endl;
        }
        else if (line == "logon") {
            if (BeeFishWebRequest::logon(_setup._secretHash))
                Serial.println("Logged on");
            else
                Serial.println("Error logging on");
        }

        return true;

    }


    
}
