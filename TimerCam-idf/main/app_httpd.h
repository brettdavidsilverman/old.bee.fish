#ifndef FEEBEECAM_APP_HTTPD
#define FEEBEECAM_APP_HTTPD


#include "esp_https_server.h"

#define ssid "Bee"
#define password "feebeegeeb3"

//#define ssid "Telstra044F87"
//#define password "ugbs3e85p5"

void initializeWebServers();
esp_err_t start_webservers(void);
void stop_webservers();

extern httpd_handle_t server;
extern httpd_handle_t cameraServer;

#endif
