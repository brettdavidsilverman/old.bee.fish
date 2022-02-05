/*
 * This is an example from https://github.com/me-no-dev/ESPAsyncWebServer
 * It demonstrates how to use the new ESPAsyncServer for firmware-update
 * 
 * This is a basic sketch to prove that it works. On success, I will enhance it
 * with the following features:
 * - SSL
 * - Wifi Manager (including a Config Portal with config-saving in EEPROM or Filesystem). This can be achieved by using
 *   the ESPAsync_WiFiManager library (https://github.com/khoih-prog/ESPAsync_WiFiManager)
 * - Blynk support (probably I'll use a completely new library for that (https://github.com/khoih-prog/Blynk_Async_WM)
 */
#include <ESP.h>
#define ASYNC_TCP_SSL_ENABLED 1
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>

/*
 * configuration options
 */

// choose your file system: LittleFS is newer, SPIFFS may be more stable
//#define USE_LITTLEFS
#define USE_SPIFFS

// the hostname is set in credentials.h
#define PORT 443

/* end of configuration options
 * don't forget to also add a "credentials.h" file with the following definitions:
 *   - SSID : the SSID of your Wireless network
 *   - WIFI_PASSWORD : password for your Wireless network
 *   - HTTP_USERNAME : username for the updater authentication
 *   - HTTP_PASSWORD : password for the updater authentication
 *   - KEYFILE : ssl server key file
 *   - CERTFILE : ssl server certificate file
 */

// you shouldn't need to alter anything below this line
#include "credentials.h"

#ifndef KEYFILE
  #error "you must #define a KEYFILE"
#endif
#ifndef CERTFILE
  #error "you must #define a CERTFILE"
#endif

#ifndef HOSTNAME
  #error "you must #define a HOSTNAME"
#endif
#ifndef PORT
  #error "you must #define a PORT"
#endif

#ifndef SSID
  #error "you must #define an SSID"
#endif
#ifndef WIFI_PASSWORD
  #error "you must #define a WIFI_PASSWORD"
#endif
#ifndef HTTP_USERNAME
  #error "you must #define an HTTP_USERNAME"
#endif
#ifndef HTTP_PASSWORD
  #error "you must #define an HTTP_PASSWORD"
#endif

#if defined USE_LITTLEFS && defined USE_SPIFFS
  #error "you can only specify one file system. #define USE_LITTLEFS _or_ USE_SPIFFS"
#elif !defined USE_LITTLEFS && !defined USE_SPIFFS
  #error "you must specify a file system. #define USE_LITTLEFS or USE_SPIFFS""
#endif

#ifdef USE_LITTLEFS
#define FILESYSTEM "LittleFS"
#else
#define FILESYSTEM "SPIFFS"
#endif

#define BUILD __FILE__ " | " SSID " | " HOSTNAME " | " FILESYSTEM " | " KEYFILE " | " CERTFILE " | " __DATE__ " | " __TIME__

//#pragma message "BUILD ID: " STR(BUILD)
#pragma message "BUILD ID: " BUILD

#ifdef USE_LITTLEFS
#include <LittleFS.h>
#endif
#ifdef USE_SPIFFS
#include <SPIFFS.h>
#include <FS.h>
#endif

AsyncWebServer server(PORT);

const char* ssid = SSID;
const char* password = WIFI_PASSWORD;
const char* http_username = HTTP_USERNAME;
const char* http_password = HTTP_PASSWORD;

//flag to use from web update to reboot the ESP
bool shouldReboot = false;

void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

void setup(){
#ifdef USE_LITTLEFS
  LittleFS.begin();
#endif
#ifdef USE_SPIFFS
  SPIFFS.begin();
#endif

  Serial.begin(1500000);
  Serial.printf("Build ID: %s\n", BUILD);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WIFI...");
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.println("connected");
  Serial.printf("Build ID: %s\n", BUILD);

  // print the feature index
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<p>SSL-Updater using ESPAsyncWebServer</p><ul><li><a href='/reboot'>Reboot the ESP8266</a></li><li><a href='/build'>Print the build ID</a></li><li><a href='/update'>Firmware Update</a></li></ul>");
  });

  // print out the Build ID
  server.on("/build", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", BUILD);
    Serial.printf("Build ID: %s\n", BUILD);
  });

  // request reboot
  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
        return request->requestAuthentication();
    request->send(200, "text/html", "<form method='POST' action='/reboot' enctype='multipart/form-data'><input type='submit' value='Reboot'></form>");
  });

  server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
        return request->requestAuthentication();
    shouldReboot = true;
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Rebooting...");
    response->addHeader("Connection", "close");
    request->send(response);
  });

  
  // Simple Firmware Update Form
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
        return request->requestAuthentication();
    request->send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
        return request->requestAuthentication();
    shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot?"Update Successful":"Update Failed");
    response->addHeader("Connection", "close");
    request->send(response);
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index){
      Serial.printf("Update Start: %s\n", filename.c_str());
      //Update.runAsync(true);
      if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
        Update.printError(Serial);
      }
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
        Serial.printf("Update failed:\n");
        Update.printError(Serial);
      }
    }
    if(final){
      if(Update.end(true)){
        Serial.printf("Update Success: %uB\n", index+len);
      } else {
        Serial.printf("Update failed:\n");
        Update.printError(Serial);
      }
    }
  });

  // Catch-All Handlers: Any request that can not find a Handler that canHandle it ends in the callbacks below.
  server.onNotFound(onRequest);

// this is called whenever https is used to load the certificate and the key file
  server.onSslFileRequest([](void * arg, const char *filename, uint8_t **buf) -> int {
    Serial.printf("SSL File: %s\n", filename);
#ifdef USE_LITTLEFS
    File file = LittleFS.open(filename, "r");
#endif
#ifdef USE_SPIFFS
    File file = SPIFFS.open(filename, "r");
#endif
    int loaded = 0;
    *buf = 0;
    if(file) {
      size_t size = file.size();
      uint8_t * nbuf = (uint8_t*) calloc(size+1, sizeof(uint8_t));
      if(nbuf){
        loaded = file.read(nbuf, size);
        if (loaded == size) {
          *buf = nbuf;
          Serial.printf("successfully loaded file\n");
        } else {
          Serial.printf("ERROR: loading file failed\n");
          free(nbuf);
          loaded = 0;
        }
      } else {
        Serial.printf("ERROR: out of memory\n");
      }
      file.close();
    }
    else {
      Serial.printf("ERROR: Could not open file\n");
    }
    return loaded;
  }, NULL);

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  delay(1000);
  server.beginSecure(CERTFILE, KEYFILE, NULL);
/* There are restrictions in what kinds of certificates can be used. I don't know what they are, but here's a configuration that works for me:
 *   openssl genrsa -out Key.pem 1024
 *   openssl req -x509 -out Cert.pem -key Key.pem -new -sha256 -subj /CN=<your domain>/O=<your OE>/C=<your country-code>/emailAddress=<your email address> -addext "keyUsage=digitalSignature,keyEncipherment" -addext extendedKeyUsage=serverAuth
 */
}

void loop(){
  if(shouldReboot){
    Serial.println("Rebooting...");
    delay(1000);
    ESP.restart();
  }
}
