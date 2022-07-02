#include "serial.h"
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
            if (FeebeeCam::downloadRequiredFiles())
                cout << "Successfully downloaded files" << endl;
            else
                cout << "Error downloading files" << endl;
        }
        else if (line == "save") {
            setup.save();
        }
        else if (line == "settings") {
            cout << setup.settings() << endl;
        }
        else if (line.startsWith("file")) {
            BString file = line.substr(line.find(' ') + 1);
            if (downloadFile(file, "/tmp.txt", true))
                cout << "File downloaded as /tmp.txt" << endl;
        }
        else if (line == "restart") {
            ESP.restart();
        }
        else if (line.startsWith("ssid")) {
            BString ssid = line.substr(line.find(' ') + 1);
            setup._ssid = ssid;
            if (setup.save()) {
                cout << "SSID changed to " + setup._ssid << endl;
                ESP.restart();
            }
            else
                cout << "Error changing SSID" << endl;
        }
        else if (line.startsWith("password")) {
            BString password = line.substr(line.find(' ') + 1);
            setup._password = password;
            if (setup.save()) {
                cout << "Password changed" << endl;
            }
            else
                cout << "Error changing password" << endl;
        }
        else if (line == "weather") {
            const BeeFishBScript::Object& object = weather.getWeather();
            cout << object << endl;
        }
        else if (line == "logon") {
            if (BeeFishWebRequest::logon(setup._secretHash))
                Serial.println("Logged on");
            else
                Serial.println("Error logging on");
        }
        else if (line == "upload") {
            if (FeebeeCam::uploadWeatherReport())
                Serial.println("Weather report uploaded");
            else
                Serial.println("Error uploading weather report");
        }
        else if (line == "help") {
            cout 
                << "download" << endl 
                << "upload" << endl
                << "save" << endl
                << "settings" << endl
                << "file [filaneme]" << endl
                << "restart " << endl
                << "ssid [ssid]" << endl
                << "password [password]" << endl
                << "weather" << endl
                << "logon" << endl
                << "help" << endl
                << endl;
        }

        return true;

    }


    
}
