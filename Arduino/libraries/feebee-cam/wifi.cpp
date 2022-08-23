#include <esp_wifi.h>
#include "wifi.h"
#include "web-request.h"
#include "setup.h"
#include "file-system.h"
#include "web-server.h"
#include "commands.h"
#include "config.h"


namespace FeebeeCam {

    bool connectedToInternet = false;
    bool connectedToAccessPoint = false;

    void accessPointConnected(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("Access point IP Address: ");
        IPAddress ipAddress = WiFi.softAPIP();
        Serial.println(ipAddress);
        FeebeeCam::connectedToAccessPoint = true;
        //FeebeeCam::commands.push(FeebeeCam::INITIALIZE_WEBSERVER);
    }

    void stationDisconnected(arduino_event_id_t event, arduino_event_info_t info) 
    {
        if (FeebeeCam::connectedToInternet) {

            FeebeeCam::connectedToInternet = false;

            Serial.println("Lost internet wifi");

            if (!FeebeeCam::connectedToAccessPoint) {
                Serial.println("Reconnecting wifi");
                WiFi.reconnect();
            }
        }
    }

    void accessPointDisconnected(arduino_event_id_t event, arduino_event_info_t info) 
    {
        FeebeeCam::connectedToAccessPoint = (WiFi.softAPgetStationNum() > 0);
        std::cerr << "Lost Access Point Connection" << std::endl;

        if (!FeebeeCam::connectedToAccessPoint)
            std::cerr << "Last Access point connection lost" << std::endl;
    }

    void stationConnected(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("Internet IP Address: ");
        Serial.println(WiFi.localIP());
        
        //BeeFishWebRequest::logoff();

        FeebeeCam::connectedToInternet = true;

        FeebeeCam::commands.push(FeebeeCam::INTERNET);
    }

    bool setupFeebeeCam() {
        
        std::cerr << "Setting up FeebeeCam" << std::endl;

        if (FeebeeCam::_setup->_beehiveVersion.length() == 0) {

            std::cout   << "Using default setup to connect to wifi with ssid " 
                        << DEFAULT_SSID
                        << std::endl;

            WiFi.mode(WIFI_STA);
            WiFi.begin(DEFAULT_SSID, DEFAULT_PASSWORD);

            while (!WiFi.isConnected()) {
                std::cerr << "." << std::flush;
                delay(500);
            }

            std::cerr << "Ok" << std::endl;

            if (!FeebeeCam::downloadFiles(false, false)) {
                return false;
            }

            ESP.restart();

            return false;

        }

        WiFi.mode(WIFI_AP);

        WiFi.softAPConfig(IPAddress(10, 10, 1, 1), IPAddress(10, 10, 1, 1), IPAddress(255, 255, 255, 0));
        WiFi.softAP(ACCESS_POINT_SSID, DEFAULT_PASSWORD);

        std::cerr << "Waiting for user to connect to access point" << std::endl;
        while (WiFi.softAPgetStationNum() == 0) {
            std::cerr << "." << std::flush;
            delay(500);
        }
        std::cerr << "Ok" << std::endl;

        std::cerr   << "Running Website with version " 
                    << FeebeeCam::_setup->_beehiveVersion << std::endl;

        if (!FeebeeCam::initializeWebServer()) {
            std::cerr << "Error starting web server" << std::endl;
            return false;
        }

        delay(500);

        std::cerr << "Setup FeebeeCam on http://10.10.1.1/setup" << std::endl;

        while (!FeebeeCam::_setup->_isSetup) {
            delay(1000);
        }

        return true;
    }

    bool connectToUserSSID() {

        //WiFi.disconnect(false, true);

        WiFi.mode(WIFI_AP_STA);

        // attempt to connect to Wifi network:
        std::string ssid;
        std::string password;

        std::cout << "Using user setup" << std::endl;
        ssid = _setup->_ssid.c_str();
        password = _setup->_password.c_str();

        std::cout << "Connecting to ssid " 
                  << ssid
                  << std::endl;

        if (password.length() == 0)
            WiFi.begin(ssid.c_str());
        else
            WiFi.begin(ssid.c_str(), password.c_str());

        unsigned long timeout = millis() + WAIT_FOR_STA_CONNECT_TIME_OUT;

        while (!WiFi.isConnected() && timeout > millis()) {
            std::cerr << "." << std::flush;
            delay(500);
        }

        if (millis() >= timeout && !WiFi.isConnected()) {
            std::cerr << "Timed out trying to connect to wifi" << std::endl;    
        }

        return WiFi.isConnected();
    }

    bool initializeWiFi() {

        using namespace std;

        Serial.println("Initializing WiFi");

        WiFi.disconnect(false, true);

        WiFi.onEvent(stationConnected,          ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(stationDisconnected,       ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        WiFi.onEvent(accessPointConnected,      ARDUINO_EVENT_WIFI_AP_STACONNECTED);
        WiFi.onEvent(accessPointDisconnected,   ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);

        WiFi.hostname(ACCESS_POINT_SSID);

        if (!FeebeeCam::_setup->_isSetup) {
            if (!setupFeebeeCam())
                return false;
        }

        if (!connectToUserSSID()) {
            return false;
        }

        if (!FeebeeCam::initializeWebServer()) {
            std::cerr << "Error starting web server" << std::endl;
            return false;
        }

        return true;
    }

    BString getURL() {

        BString url;
        BString ipAddress;
        
        //if (FeebeeCam::connectedToAccessPoint)
        //    ipAddress = WiFi.softAPIP().toString().c_str();
        
        if (FeebeeCam::connectedToInternet)
            ipAddress = WiFi.localIP().toString().c_str();

        if (ipAddress.length())
            url = BString("http://") + ipAddress + "/";
        else
            url = "disconnected";

        return url;
    }

}
