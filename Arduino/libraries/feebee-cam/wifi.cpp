#include <esp_wifi.h>
#include "wifi.h"
#include "web-request.h"
#include "setup.h"
#include "file-system.h"
#include "web-server.h"
#include "commands.h"
#include "config.h"
#include "light.h"
#include "status.h"
#include "local-time.h"

namespace FeebeeCam {

    bool isConnectedToInternet = false;
    bool isConnectedToESPAccessPoint = false;
    const IPAddress softAPIP(10, 10, 1, 1);
    const IPAddress gateway(255, 255, 255, 0);
    DNSServer* dnsServer = nullptr;
    bool waitForConnection();

    bool initializeDNSServer(IPAddress ipAddress) {

        Serial.println("Starting DNS Server");

        FeebeeCam::deinitializeDNSServer();

        dnsServer = new DNSServer();

        //dnsServer->setErrorReplyCode(DNSReplyCode::NoError);

        bool started = false;

        started = dnsServer->start(53, "*", ipAddress);
        //started = dnsServer->start(53, LOCAL_DNS_HOST_NAME, ipAddress);

        if (started) {
            std::cerr << "DNS Server Started" << std::endl;
            return true;
        }
        else {
            std::cerr << "DNS Server error" << std::endl;
            return false;
        }

    }    

    bool deinitializeDNSServer() {

        if (FeebeeCam::dnsServer) {
            DNSServer* _dnsServer = FeebeeCam::dnsServer;
            FeebeeCam::dnsServer = nullptr;
            _dnsServer->stop();
            delete _dnsServer;
        }

        return true;
    }


    void accessPointConnected(arduino_event_id_t event, arduino_event_info_t info) 
    {

        Serial.print("Access point IP Address: ");
        IPAddress ipAddress = WiFi.softAPIP();
        FeebeeCam::isConnectedToESPAccessPoint = true;

        initializeDNSServer(ipAddress);

        FeebeeCam::status._wakeupNextTime = true;

        std::cerr 
            << "Setup FeebeeCam on http://"
            << ipAddress.toString().c_str() 
            << "/setup" 
            << std::endl;

        //FeebeeCam::initializeWebServers();
    
        FeebeeCam::commands.push(FeebeeCam::INITIALIZE_WEBSERVER);
    }

    void accessPointDisconnected(arduino_event_id_t event, arduino_event_info_t info) 
    {
        FeebeeCam::isConnectedToESPAccessPoint = (WiFi.softAPgetStationNum() > 0);
        std::cerr << "Lost Access Point Connection" << std::endl;

        if (!FeebeeCam::isConnectedToESPAccessPoint) {
            std::cerr << "Last Access point connection lost" << std::endl;
            FeebeeCam::deinitializeDNSServer();
        }
    }

    void stationConnected(arduino_event_id_t event, arduino_event_info_t info) 
    {

        FeebeeCam::isConnectedToInternet = true;

        FeebeeCam::deinitializeDNSServer();

        BeeFishWebRequest::logoff();

        FeebeeCam::commands.push(FeebeeCam::INTERNET);

    }

    void stationDisconnected(arduino_event_id_t event, arduino_event_info_t info) 
    {
        if (FeebeeCam::isConnectedToInternet) {

            FeebeeCam::isConnectedToInternet = false;

            Serial.println("Lost internet wifi");

            if (!FeebeeCam::isConnectedToESPAccessPoint) {
                Serial.println("Reconnecting wifi");
                WiFi.reconnect();
                waitForConnection();
            }
        }
    }

    bool waitForConnection() {

        unsigned long timeout = millis() + WAIT_FOR_WIFI_CONNECT;

        while ( !WiFi.isConnected() && 
                !FeebeeCam::isConnectedToESPAccessPoint && 
                !Serial.available() &&
                timeout > millis()
                )
        {
            Serial.print(".");
            delay(500);
        }

        if (WiFi.isConnected() || FeebeeCam::isConnectedToESPAccessPoint) {
            return true;
        };


        RESTART_AFTER_ERROR();

        return false;


    }
    
    bool connectToDefaultSSID() {
        std::cout   << "Using default setup to connect to wifi with ssid " 
                    << DEFAULT_SSID
                    << std::endl;

        WiFi.begin(DEFAULT_SSID, DEFAULT_PASSWORD);

        return waitForConnection();

    }

    bool setupFeebeeCam() {
        
        std::cerr << "Setting up FeebeeCam" << std::endl;
        
        if (!connectToDefaultSSID())
            return false;

        return true;
/*
        std::cerr << "Ok" << std::endl;

        if (!FeebeeCam::downloadFiles(false, false)) {
            return false;
        }

        std::cerr   << "Running Website with version " 
                    << FeebeeCam::_setup->_version << std::endl;
*/
        return true;
    }

    bool connectToUserSSID() {

        // attempt to connect to Wifi network:
        std::string hostSSID;
        std::string password;

        WiFi.setAutoReconnect(true);

        std::cout << "Using user setup" << std::endl;
        hostSSID = _setup->_hostSSID.str();
        password = _setup->_hostPassword.str();
        std::cout << "Connecting to host ssid " 
                  << hostSSID
                  << std::endl;

/*
#warning "remove this show password output"
        std::cout << "Host password: " 
                  << password
                  << std::endl;
*/

        if (password.length() == 0)
            WiFi.begin(hostSSID.c_str());
        else
            WiFi.begin(hostSSID.c_str(), password.c_str());

        bool success = waitForConnection();
        return success;
    }

    bool initializeWiFi() {

        using namespace std;

        std::cerr << "Initializing WiFi" << std::endl;

        WiFi.disconnect(false, true);

        WiFi.onEvent(stationConnected,          ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(stationDisconnected,       ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        WiFi.onEvent(accessPointConnected,      ARDUINO_EVENT_WIFI_AP_STACONNECTED);
        WiFi.onEvent(accessPointDisconnected,   ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);

        if (FeebeeCam::_setup->_isSetup)
            WiFi.hostname(FeebeeCam::_setup->_label.str().c_str());

        WiFi.mode(WIFI_AP_STA);
//        WiFi.mode(WIFI_STA);

        bool success = true;
        WiFi.softAPConfig(softAPIP, softAPIP, gateway);

        std::string ssid;
        std::string password;

        ssid = FeebeeCam::_setup->_feebeeCamSSID.str();
        password = FeebeeCam::_setup->_feebeeCamPassword.str();

        WiFi.softAP(
            ssid.c_str(),
            password.c_str()
        );

        std::cerr
            << "Access point SSID: "
            << "\"" << ssid << "\""
            << endl
            << "Access point password: "
            << "\"" << password << "\""
            << endl;


        if (FeebeeCam::_setup->_isSetup)
            success &= connectToUserSSID();
        else
            success &= connectToDefaultSSID();

        return success;
    }

    BString getURL(int port) {

        std::stringstream url;
        BString ipAddress;
        
        if (FeebeeCam::isConnectedToESPAccessPoint) 
            ipAddress = WiFi.softAPIP().toString().c_str();
        else if (FeebeeCam::isConnectedToInternet)
            ipAddress = WiFi.localIP().toString().c_str();
        
        if (ipAddress.length()) {
            url << "http://" << ipAddress;
            if (port > 0 && port != 80)
                url << ":" << port;
            url << "/";
        }
        else
            url << "disconnected";

        return url.str();
    }

}
