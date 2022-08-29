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
    const IPAddress softAPIP(10, 10, 1, 1);
    const IPAddress gateway(255, 255, 255, 0);
    DNSServer* dnsServer = nullptr;

    bool initializeDNSServer(IPAddress ipAddress) {
        return false;
        Serial.println("Starting DNS Server");

        if (dnsServer) {
            dnsServer->stop();
            delete dnsServer;
        }

        dnsServer = new DNSServer();

        dnsServer->setErrorReplyCode(DNSReplyCode::NoError);

        bool started = false;

        started = dnsServer->start(53, LOCAL_DNS_HOST_NAME, ipAddress);

        if (started) {
            std::cerr << "DNS Server Started" << std::endl;
            return true;
        }
        else {
            std::cerr << "DNS Server error" << std::endl;
            return false;
        }

    }    

    void accessPointConnected(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("Access point IP Address: ");
        IPAddress ipAddress = WiFi.softAPIP();
        Serial.println(ipAddress);
        FeebeeCam::connectedToAccessPoint = true;

        initializeDNSServer(WiFi.softAPIP());

        std::cerr << "Setup FeebeeCam on http://" << LOCAL_DNS_HOST_NAME << "/setup" << std::endl;
            
        //FeebeeCam::commands.push(FeebeeCam::INITIALIZE_WEBSERVER);
    }

    void accessPointDisconnected(arduino_event_id_t event, arduino_event_info_t info) 
    {
        FeebeeCam::connectedToAccessPoint = (WiFi.softAPgetStationNum() > 0);
        std::cerr << "Lost Access Point Connection" << std::endl;

        if (!FeebeeCam::connectedToAccessPoint) {
            std::cerr << "Last Access point connection lost" << std::endl;
        }
    }

    void stationConnected(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("Internet IP Address: ");
        Serial.println(WiFi.localIP());

        initializeDNSServer(WiFi.localIP());
        
        //BeeFishWebRequest::logoff();

        FeebeeCam::connectedToInternet = true;

        FeebeeCam::commands.push(FeebeeCam::INTERNET);
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

    bool connectToLocalSSID() {
        std::cout   << "Using default setup to connect to wifi with ssid " 
                    << DEFAULT_SSID
                    << std::endl;

        WiFi.begin(DEFAULT_SSID, DEFAULT_PASSWORD);

        while (!WiFi.isConnected()) {
            Serial.print(".");
            delay(500);
        }

        return WiFi.isConnected();

    }

    bool setupFeebeeCam() {
        
        std::cerr << "Setting up FeebeeCam" << std::endl;

        if (FeebeeCam::_setup->_beehiveVersion.length() == 0) {

            if (!connectToLocalSSID())
                return false;

            std::cerr << "Ok" << std::endl;

            if (!FeebeeCam::downloadFiles(false, false)) {
                return false;
            }

            ESP.restart();

            return false;

        }

        std::cerr   << "Running Website with version " 
                    << FeebeeCam::_setup->_beehiveVersion << std::endl;

        return true;
    }

    bool connectToUserSSID() {

        //WiFi.disconnect(false, true);


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

        return true;
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

        bool success = true;

        WiFi.mode(WIFI_AP_STA);
        WiFi.softAPConfig(softAPIP, softAPIP, gateway);
        WiFi.softAP(ACCESS_POINT_SSID, DEFAULT_PASSWORD);

        if (FeebeeCam::_setup->_isSetup) {
            success &= connectToUserSSID();
        }
        else
            success &= setupFeebeeCam();



        return success;
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
