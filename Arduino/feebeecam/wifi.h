#include <WiFi.h>

#define SSID        "ESP32"
#define PASSWORD    "feebeegeeb3"

#define IP_ADDRESS IPAddress(10, 10, 1, 1)
#define GATEWAY    IPAddress(10, 10, 1, 1)
#define SUBNET     IPAddress(255, 255, 255, 0)

namespace FeebeeCam {

    void clientConnected(WiFiEvent_t event, WiFiEventInfo_t info) 
    {

        Serial.print("IP Address: ");
        IPAddress ipAddress = WiFi.softAPIP();
        Serial.println(ipAddress);
    
    }

    void initializeWiFi() {

        WiFi.onEvent(clientConnected, SYSTEM_EVENT_AP_STACONNECTED);

        WiFi.mode(WIFI_AP);

        WiFi.softAPConfig(IP_ADDRESS, GATEWAY, SUBNET);

        WiFi.softAP(SSID, PASSWORD);
        //WiFi.begin(ssid, password);
    /*
        while (!WiFi.isConnected()) {
            Serial.print(".");
            delay(500);
        }
        

        Serial.println(WiFi.localIP());
    */

    }
}