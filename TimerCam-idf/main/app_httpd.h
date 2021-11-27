#ifndef FEEBEECAM_APP_HTTPD
#define FEEBEECAM_APP_HTTPD


#include "esp_https_server.h"

void initializeWebServer(const char* ssid, const char* password);
esp_err_t start_webservers(void);
void stop_webservers();

extern httpd_handle_t server;
extern httpd_handle_t cameraServer;

#endif
