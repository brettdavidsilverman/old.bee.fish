#include "serial.h"
#include "wifi.h"
#include "file-system.h"
#include "weather.h"
#include "setup.h"
#include "commands.h"
#include "camera.h"

namespace FeebeeCam {

    bool initializeSerial() {
        
        Serial.begin(1500000);

        while (!Serial)
            delay(1);

        std::cout << "*******************************************" << std::endl;
        
        std::cout << "Feebee-Cam Starting on host " HOST << endl;
        std::cout << "Running version " BEEHIVE_VERSION << std::endl;

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
            if (!WiFi.isConnected()) {
                std::cerr 
                    << "Not connected. Connecting to local ssid" 
                    << std::endl;
                FeebeeCam::connectToLocalSSID();
            }
            FeebeeCam::downloadFiles(true);
        }
        else if (line == "save") {
            _setup->save();
        }
        else if (line == "setup") {
            cout << _setup << endl;
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
            cout << FeebeeCam::Weather::getWeather(true) << endl;
        }
        else if (line == "time") {
            cout << FeebeeCam::getDateTime() << endl;
        }
        else if (line == "rtc") {
            FeebeeCam::_setup->_isRTCSetup = false;
            FeebeeCam::initializeTime();
            cout << FeebeeCam::getDateTime() << endl;
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
        else if (line == "upload image") {
            //if (!FeebeeCam::initializeCamera(1))
            //    Serial.println("Error initializing camera");
            if (FeebeeCam::uploadImage())
                Serial.println("Image uploaded");
            else
                Serial.println("Error uploading image");
        }
        else if (line == "upload weather") {
            FeebeeCam::uploadWeatherReport();
            Serial.println("Weather uploaded");
        }
        else if (line == "connect") {
            if (!FeebeeCam::connectToLocalSSID())
                Serial.println("Error connecting to local");
            else
                Serial.println("Connected to local");
        }
        else if (line == "wifi") {
            if (!FeebeeCam::initializeWiFi())
                Serial.println("Error initializing wifi");
            else
                Serial.println("Connected to wifi");
        }
        else if (line == "sleep") {
            FeebeeCam::putToSleep();
        }
        else if (line == "help") {
            cout 
                << "download" << endl 
                << "upload image|weather" << endl
                << "save" << endl
                << "setup" << endl
                << "time" << endl
                << "rtc" << endl
                << "file [filaneme]" << endl
                << "restart " << endl
                << "ssid [ssid]" << endl
                << "password [password]" << endl
                << "secret [secret]" << endl
                << "label [label]" << endl
                << "weather" << endl
                << "logon" << endl
                << "connect" << endl
                << "wifi" << endl
                << "sleep" << endl
                << "help" << endl
                << endl;
        }

        return true;

    }


   bool checkCommandLine() {

        std::cerr << "Enter command line or ignore to continue" << std::endl;

        delay(1000);

        if (Serial.available()) {

            while (1)
                FeebeeCam::handleCommandLine();
        }

        return true;

    }
    
}
