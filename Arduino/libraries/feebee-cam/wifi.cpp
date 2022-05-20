#include "wifi.h"
#include "web-request.h"
#include "setup.h"
#include "file-system.h"

#define LAPTOP_SSID "Laptop"         // your network SSID (name)
#define PASSWORD "feebeegeeb3"    // your network password
#define ACCESS_POINT_SSID "FeebeeCam"


namespace FeebeeCam {

    void clientConnected(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("Access point IP Address: ");
        IPAddress ipAddress = WiFi.softAPIP();
        Serial.println(ipAddress);

    }

    void lostConnection(arduino_event_id_t event, arduino_event_info_t info) 
    {
        Serial.println("Reconnecting");
        WiFi.reconnect();
    }

    void gotInternet(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("Internet IP Address: ");
        Serial.println(WiFi.localIP());
        
        BeeFishWebRequest::logoff();

        FeebeeCam::downloadWhenReady = true;

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
        WiFi.softAP(ACCESS_POINT_SSID, PASSWORD);
        
        // attempt to connect to Wifi network:
        std::string ssid;
        std::string password;

        if (_setup._ssid.length()) {
            std::cout << "Using user setup" << std::endl;
            ssid = _setup._ssid.c_str();
            password = _setup._password.c_str();
        }
        else {
            std::cout << "Using default setup" << std::endl;
            ssid = LAPTOP_SSID;
            password = PASSWORD;
        }

        std::cout << "Connecting to ssid " 
                  << ssid
                  << std::endl;

        if (password.length() == 0)
            WiFi.begin(ssid.c_str());
        else
            WiFi.begin(ssid.c_str(), password.c_str());

    }

}
