#ifndef FEEBEECAM_APP_HTTPD
#define FEEBEECAM_APP_HTTPD


#include "esp_https_server.h"

#define softAPSSID "BeeHive"
#define softAPPassword "bds4Bondi"

//#define ssid "Telstra044F87"
//#define password "ugbs3e85p5"

void initializeWiFi();
esp_err_t start_webservers(void);
void stop_webservers();

extern httpd_handle_t server;
extern httpd_handle_t cameraServer;

#endif
