#include "serial.h"
#include "wifi.h"
#include "file-system.h"
#include "weather.h"
#include "setup.h"
#include "commands.h"

namespace FeebeeCam {

    bool initializeSerial() {
        
        Serial.begin(1500000);

        while (!Serial)
            delay(10);

        std::cout << "*******************************************" << std::endl;
        
        std::cout << "Feebee-Cam Starting" << std::endl;

        std::cout << "*******************************************" << std::endl;

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
            FeebeeCam::downloadFiles(true);
        }
        else if (line == "save") {
            _setup->save();
        }
        else if (line == "settings") {
            cout << _setup->settings() << endl;
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
            _setup->_ssid = ssid;
            if (_setup->save()) {
                cout << "SSID changed to " + _setup->_ssid << endl;
                ESP.restart();
            }
            else
                cout << "Error changing SSID" << endl;
        }
        else if (line.startsWith("password")) {
            BString password = line.substr(line.find(' ') + 1);
            _setup->_password = password;
            if (_setup->save()) {
                cout << "Password changed" << endl;
            }
            else
                cout << "Error changing password" << endl;
        }
        else if (line == "weather") {
            cout << FeebeeCam::weather.getWeather() << endl;
        }
        else if (line.startsWith("secret")) {
            BString secret = line.substr(line.find(' ') + 1);
            _setup->_secretHash = secret;
            if (_setup->save()) {
                cout << "Secret hash changed" << endl;
            }
            else
                cout << "Error setting secret hash" << endl;
        }
        else if (line.startsWith("label")) {
            BString label = line.substr(line.find(' ') + 1);
            _setup->_label = label;
            if (_setup->save()) {
                cout << "Label changed" << endl;
            }
            else
                cout << "Error setting label" << endl;
        }
        else if (line == "logon") {
            if (BeeFishWebRequest::logon(_setup->_secretHash))
                Serial.println("Logged on");
            else
                Serial.println("Error logging on");
        }
        else if (line == "upload") {
            if (!FeebeeCam::initializeCamera(1))
                Serial.println("Error initializing camera");
            if (FeebeeCam::uploadWeatherReport())
                Serial.println("Weather report uploaded");
            else
                Serial.println("Error uploading weather report");
        }
        else if (line == "sleep") {
            FeebeeCam::putToSleep();
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
                << "secret [secret]" << endl
                << "label [label]" << endl
                << "weather" << endl
                << "logon" << endl
                << "sleep" << endl
                << "help" << endl
                << endl;
        }

        return true;

    }


    
}
