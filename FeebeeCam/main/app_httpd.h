#ifndef FEEBEECAM_APP_HTTPD
#define FEEBEECAM_APP_HTTPD

#define SECURE_SOCKETS


#ifdef SECURE_SOCKETS

#include "esp_http_server.h"

#include "ssl-cert.h"

extern httpsserver::SSLCert * cert;

#define PROTOCOL "https"
#define CREATE_HTTPD_CONFIG createHTTPDSSLConfig
#define HTTPD_START httpd_ssl_start
#define HTTPD_CONFIG httpd_ssl_config_t

#else

#include "esp_https_server.h"

#define PROTOCOL "http"
#define CREATE_HTTPD_CONFIG createHTTPDConfig
#define HTTPD_START httpd_start
#define HTTPD_CONFIG httpd_config_t

#endif

#define softAPSSID "FeebeeCam"
#define softAPPassword "feebeegeeb3"

#define SSID "Android"
#define PASSWORD "feebeegeeb3"

void initializeWiFi();
void initializeCamera();
esp_err_t start_webservers(void);
void stop_webservers();

extern httpd_handle_t server;
extern httpd_handle_t cameraServer;
extern bool stopped;

#endif
