/* Non-Volatile Storage (NVS) Read and Write a Value - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <algorithm>

#define ESP32

#define HTTPD_STACK_SIZE 32768

#ifdef ESP32
extern "C" {
#include <esp_spiram.h>
#include <esp_himem.h>
}
#endif

#define DEBUG

#include "../bee-fish/bee-fish.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_http_server.h"

using namespace std;
using namespace BeeFishHive;

/* Encryption modes */
enum wl_enc_type
{ /* Values map to 802.11 encryption suites... */
  ENC_TYPE_WEP = 5,
  ENC_TYPE_TKIP = 2,
  ENC_TYPE_CCMP = 4,
  /* ... except these two, 7 and 8 are reserved in 802.11-2007 */
  ENC_TYPE_NONE = 7,
  ENC_TYPE_AUTO = 8
};

bool initializeWebServer();

bool Setup::isSetup() {
    return false;
}

Setup::Setup() {
    initialize();
}

void Setup::initialize()
{

  
    if (!psramInit()) {
        printf("psramInit failed\n");
        while (1)
            ;
    }

    printf("PSRAM size: %u\n", ESP.getPsramSize());
    printf("PSRAM free: %u\n", ESP.getFreePsram());

    if (!BeeFishTest::test()) {
        printf("BeeFishTest failed");
        while (1)
            ;
    }

    printf("OK\n");

    while (1)
        ;

  //Serial.println("Starting WiFi");
  //WiFi.softAPConfig(localIP, gateway, subnet);
  //WiFi.softAP("FeebeeCam", "feebeegeeb3");
/*
  if (!initializeWebServer())
  {
    while (1)
      ;
  }

  return;

  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  // Open
  printf("\n");
  printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  }
  else
  {
    printf("Done\n");

    // Read
    printf("Reading restart counter from NVS ... ");
    int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
    switch (err)
    {
    case ESP_OK:
      printf("Done\n");
      printf("Restart counter = %i\n", restart_counter);
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      printf("The value is not initialized yet!\n");
      break;
    default:
      printf("Error (%s) reading!\n", esp_err_to_name(err));
    }

    // Write
    printf("Updating restart counter in NVS ... ");
    restart_counter++;
    err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    printf("Committing updates in NVS ... ");
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    // Close
    nvs_close(my_handle);
  }
  */


}

void loop()
{
  /*
  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
  delay(5000);
*/
}

void listNetworks()
{
  // scan for nearby networks:
  printf("** Scan Networks **\n");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    printf("Couldn't get a wifi connection\n");
    while (true)
      ;
  }

  // print the list of networks seen:
  printf("number of available networks: %u", numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++)
  {
    printf("%i", thisNet);
    printf("\t");
    printf(WiFi.SSID(thisNet).c_str());
    printf("\tSignal: ");
    printf("%i", WiFi.RSSI(thisNet));
    printf(" dBm");
    printf("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
    printf("\n");
  }
}

void printEncryptionType(int thisType)
{
  // read the encryption type and print out the name:
  switch (thisType)
  {
  case ENC_TYPE_WEP:
    printf("WEP");
    break;
  case ENC_TYPE_TKIP:
    printf("WPA");
    break;
  case ENC_TYPE_CCMP:
    printf("WPA2");
    break;
  case ENC_TYPE_NONE:
    printf("None");
    break;
  case ENC_TYPE_AUTO:
    printf("Auto");
    break;
  }
}

String content =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "   <meta charset=\"utf-8\"/>\n"
    "   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=0\"/>\n"
    "   <script src=\"https://bee.fish/head.js\"></script>\n"
    "   <link rel=\"stylesheet\" type=\"text/css\" href=\"https://bee.fish/style.css\" />\n"
    "</head>\n"
    "<body>\n"
    "   <h1>Bee Hive Setup</h1>\n"
    "   <form onsubmit=\"return false;\">\n"
    "     WiFi Name<br/>\n"
    "     <input type=\"text\" value=\"WiFi Name\" id=\"wifi\"></input><br/>\n"
    "     Password<br/>\n"
    "     <input type=\"password\" value=\"password\" id=\"password\"></input><br/>\n"
    "     <button onclick=\"submitForm();\">Ok</button>\n"
    "   </form>\n"
    "   <script>\n"
    "function submitForm(event) {\n"
    "   var wifi = document.getElementById('wifi').value;\n"
    "   var password = document.getElementById('password').value;\n"
    "   var object = {\n"
    "      wifi,\n"
    "      password\n"
    "   }\n"
    "   var submission = JSON.stringify(object);\n"
    "   var params = {}\n"
    "   params.method = \"POST\";\n"
    "   params.body = submission;\n"
    "   var data =\n"
    "      fetch(document.location, params)\n"
    "      .then(response => response.json())\n"
    "      .then(json => alert(JSON.stringify(json)))\n"
    "      .catch(error => alert(error));\n"
    "}\n"
    "   </script>\n"
    "   </body>\n"
    "</html>";

esp_err_t get_index(httpd_req_t *req)
{

  printf("Http request get on /\n");

  esp_err_t res;

  res = httpd_resp_send(req, content.c_str(), content.length());

  //Serial.println(content);

  return ESP_OK;
}

esp_err_t post_index(httpd_req_t *req)
{

  cout << "HTTP post on /" << endl;
  cout << "Stack Size: " << CONFIG_FREERTOS_IDLE_TASK_STACKSIZE << endl;

  char post[256];

  // Truncate if content length larger than the buffer
  size_t receiveSize = std::min(req->content_len, sizeof(post));
  
  int ret = httpd_req_recv(req, post, receiveSize);
  std::string json(post, receiveSize);

  if (ret <= 0)
     return ESP_FAIL;

  cout << "Received " << receiveSize << " bytes" << endl;

  cout << "Setting up object" << endl;

  BeeFishJSON::_Object object;
  object._capture = true;

  cout << "Setting up parser" << endl;

  BeeFishParser::Parser parser(object);
  BeeFishMisc::optional<bool> result;

  cout << "Parsing body..." << endl;

  try {
    cout << "Body: " << json << endl;
    result = parser.read(json);
    cout << "Result: " << result << endl;
  }
  catch (std::exception& ex) {
    Serial.println("Exception: ");
    Serial.print(ex.what());
    return ESP_FAIL;
  }
  catch (...) {
    Serial.println("Unkown Exception");
    return ESP_FAIL;
  }

  
  BeeFishBString::BString wifi;
  BeeFishBString::BString password;

  std::stringstream response;

  if (result == true) {

    Serial.println("Parsed object Ok");

    if (object.contains("wifi"))
      wifi = object["wifi"]->value();

    if (object.contains("password"))
      password = object["password"]->value();

    if (wifi.length() > 0 && password.length() > 0) {
       response << "{\"wifi\": \"";
       wifi.writeEscaped(response);
       response << "\", \"password\": \"";
       password.writeEscaped(response);
       response << "\"}";
    }

  }
  else {
    Serial.println("Invalid post request");
    return ESP_FAIL;
  }

  Serial.println("Request:");
  Serial.println(json.c_str());
  Serial.println("Response:");
  Serial.println(response.str().c_str());

  esp_err_t res;

  res = httpd_resp_set_status(req, "200");

  if (res == ESP_OK)
    res = httpd_resp_set_hdr(req, "Content-Type", "application/json");

  if (res == ESP_OK)
  {
    res = httpd_resp_send(req, response.str().c_str(), response.str().length());
  }

  return res;
}

httpd_uri_t get_index_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_index,
    .user_ctx = NULL};

httpd_uri_t post_index_uri = {
    .uri = "/",
    .method = HTTP_POST,
    .handler = post_index,
    .user_ctx = NULL};

bool initializeWebServer()
{

  Serial.println("Starting web server...");

  WiFi.softAPConfig(localIP, gateway, subnet);
  WiFi.softAP("FeebeeCam", "feebeegeeb3");

  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  config.stack_size = HTTPD_STACK_SIZE;

  httpd_handle_t server = NULL;

  esp_err_t res = httpd_start(&server, &config);

  if (res == ESP_OK)
  {
    res = httpd_register_uri_handler(server, &get_index_uri);
  }

  if (res == ESP_OK)
  {
    res = httpd_register_uri_handler(server, &post_index_uri);
  }

  if (res != ESP_OK)
  {
    printf("Error: %s", esp_err_to_name(res));
    return false;
  }

  printf("Starting server on port: ");

  printf("http://%s:%d/\n", WiFi.softAPIP().toString().c_str(), config.server_port);

  return true;
}
