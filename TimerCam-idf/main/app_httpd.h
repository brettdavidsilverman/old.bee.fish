#ifndef FEEBEECAM_APP_HTTPD
#define FEEBEECAM_APP_HTTPD


#include "esp_https_server.h"

#define softAPSSID "BeeHive"
#define softAPPassword "feebeegeeb3"

#define SSID "Android"
#define PASSWORD "feebeegeeb3"

void initializeWiFi();
void initializeCamera();
esp_err_t start_webservers(void);
void stop_webservers();

extern httpd_handle_t server;
extern httpd_handle_t cameraServer;

#endif
