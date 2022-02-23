#include <WiFi.h>
#include "file-system.h"

#define SSID            "ESP32"
#define PASSWORD        "feebeegeeb3"
#define SSID_INTERNET   "Android"

#define IP_ADDRESS IPAddress(10, 10, 1, 1)
#define GATEWAY    IPAddress(10, 10, 1, 1)
#define SUBNET     IPAddress(255, 255, 255, 0)

namespace FeebeeCam {

    void clientConnected(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("IP Address: ");
        IPAddress ipAddress = WiFi.softAPIP();
        Serial.println(ipAddress);
    
    }

    void gotIPAddress(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        FeebeeCam::downloadWhenReady = true;

    }

    void initializeWiFi() {

        WiFi.onEvent(clientConnected, ARDUINO_EVENT_WIFI_AP_STACONNECTED);

        WiFi.onEvent(gotIPAddress, ARDUINO_EVENT_WIFI_STA_GOT_IP);

        WiFi.mode(WIFI_AP_STA);

        WiFi.softAPConfig(IP_ADDRESS, GATEWAY, SUBNET);

        WiFi.softAP(SSID, PASSWORD);
        
        WiFi.begin(SSID_INTERNET, PASSWORD);

/*    
        while (!WiFi.isConnected()) {
            Serial.print(".");
            delay(500);
        }
        
*/
    

    }
}
