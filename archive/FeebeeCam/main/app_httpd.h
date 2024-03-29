#ifndef FEEBEECAM_APP_HTTPD
#define FEEBEECAM_APP_HTTPD

#define SECURE_SOCKETS


#ifdef SECURE_SOCKETS

#include "esp_https_server.h"

#include "ssl-cert.h"

extern httpsserver::SSLCert * cert;

#define PROTOCOL "https"
#define CREATE_HTTPD_CONFIG createHTTPDSSLConfig
#define HTTPD_START httpd_ssl_start
#define HTTPD_CONFIG httpd_ssl_config_t

#else

#include "esp_http_server.h"

#define PROTOCOL "http"
#define CREATE_HTTPD_CONFIG createHTTPDConfig
#define HTTPD_START httpd_start
#define HTTPD_CONFIG httpd_config_t

#endif

#include <bee-fish.h>

#define softAPSSID "FeebeeCam"
#define softAPPassword "feebeegeeb3"

#define SSID "Android"
#define PASSWORD "feebeegeeb3"

namespace FeebeeCam {

    void initializeWiFi();
    void initializeCamera();
    esp_err_t startWebServers(void);
    void stopWebServers();
    void printWebServers();
    void printStatus();
    BeeFishJSONOutput::Object getWeather();

    extern IPAddress localIPAddress; // 10.10.1.1;
    extern IPAddress subnetIP;       // 10.10.1.1
    extern IPAddress gatewayIP;      // 255.255.255.0
    
    extern volatile bool onGotIPAddressFlag;
    extern volatile bool isRunning;
    extern volatile bool stop;

    extern int64_t lastTimeFramesCounted;
    extern unsigned long frameCount;
    extern float framesPerSecond;

}

#endif
