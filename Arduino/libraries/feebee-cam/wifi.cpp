#include <esp_wifi.h>
#include "wifi.h"
#include "web-request.h"
#include "setup.h"
#include "file-system.h"
#include "web-server2.h"
#include "commands.h"
#include "config.h"


namespace FeebeeCam {

    volatile bool connectedToInternet = false;
    volatile bool connectedToAccessPoint = false;

    void clientConnected(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("Access point IP Address: ");
        IPAddress ipAddress = WiFi.softAPIP();
        Serial.println(ipAddress);
        FeebeeCam::connectedToAccessPoint = true;
        //FeebeeCam::commands.push(FeebeeCam::INITIALIZE_WEBSERVER);
    }

    void lostConnection(arduino_event_id_t event, arduino_event_info_t info) 
    {
        FeebeeCam::connectedToInternet = false;
        
        //if (!FeebeeCam::connectedToAccessPoint) {
            Serial.println("Reconnecting wifi");
        //    WiFi.reconnect();
            esp_wifi_connect();
        //}
    }

    void gotInternet(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("Internet IP Address: ");
        Serial.println(WiFi.localIP());
        
        //BeeFishWebRequest::logoff();

        FeebeeCam::connectedToInternet = true;

        FeebeeCam::commands.push(FeebeeCam::INTERNET);
    }

    void initializeWiFi() {

        using namespace std;

        Serial.println("Initializing WiFi");

        WiFi.hostname(ACCESS_POINT_SSID);
        
        WiFi.mode(WIFI_AP_STA);
        WiFi.onEvent(clientConnected, ARDUINO_EVENT_WIFI_AP_STACONNECTED);
        WiFi.onEvent(lostConnection, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        WiFi.onEvent(gotInternet, ARDUINO_EVENT_WIFI_STA_GOT_IP);

        WiFi.softAPConfig(IPAddress(10, 10, 1, 1), IPAddress(10, 10, 1, 1), IPAddress(255, 255, 255, 0));
        WiFi.softAP(ACCESS_POINT_SSID, DEFAULT_PASSWORD);
        
        // attempt to connect to Wifi network:
        std::string ssid;
        std::string password;

        if (_setup->_ssid.length()) {
            std::cout << "Using user setup" << std::endl;
            ssid = _setup->_ssid.c_str();
            password = _setup->_password.c_str();
        }
        else {
            std::cout << "Using default setup" << std::endl;
            ssid = DEFAULT_SSID;
            password = DEFAULT_PASSWORD;
        }

        std::cout << "Connecting to ssid " 
                  << ssid
                  << std::endl;

        if (password.length() == 0)
            WiFi.begin(ssid.c_str());
        else
            WiFi.begin(ssid.c_str(), password.c_str());

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
