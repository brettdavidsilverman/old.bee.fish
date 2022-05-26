/**
 * Example for the ESP32 HTTP(S) Webserver
 *
 * IMPORTANT NOTE:
 * To run this script, your need to
 *  1) Enter your WiFi SSID and PSK below this comment
 *  2) Make sure to have certificate data available. You will find a
 *     shell script and instructions to do so in the library folder
 *     under extras/
 *
 * This script will install an HTTPS Server on your ESP32 with the following
 * functionalities:
 *  - Show simple page on web server root
 *  - 404 for everything else
 */

// TODO: Configure your WiFi here
#define WIFI_SSID "Laptop"
#define WIFI_PSK  "feebeegeeb3"

// Include certificate data (see note above)
#include "cert.h"
#include "private_key.h"

// Binary data for the favicon
#include "favicon.h"

// We will use wifi
#include <WiFi.h>

// Includes for the server
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

#include <feebee-cam.h>

// The HTTPS Server comes in a separate namespace. For easier use, include it here.
using namespace httpsserver;
using namespace FeebeeCam;
/*
// Create an SSL certificate object from the files included above
SSLCert cert = SSLCert(
  example_crt_DER, example_crt_DER_len,
  example_key_DER, example_key_DER_len
);
*/

// Create an SSL-enabled server that uses the certificate
// The contstructor takes some more parameters, but we go for default values here.
//HTTPSServer secureServer = HTTPSServer(&cert);
HTTPServer server0 = HTTPServer(80);
HTTPServer server1 = HTTPServer(81);

// Declare some handler functions for the various URLs on the server
// The signature is always the same for those functions. They get two parameters,
// which are pointers to the request data (read request body, headers, ...) and
// to the response data (write response, set status code, ...)
void handleRoot(HTTPRequest * req, HTTPResponse * res);
void handleFavicon(HTTPRequest * req, HTTPResponse * res);
void handle404(HTTPRequest * req, HTTPResponse * res);
void handleCamera(HTTPRequest * request, HTTPResponse * response);
bool sendChunk(HTTPResponse* response, const Data& data);
bool sendChunk(HTTPResponse* response);
void cameraServerTask( void * input );
bool uploadWeatherReport2();

void setup() {
  // For logging
  Serial.begin(1500000);

  // Connect to WiFi
  Serial.println("Starting Static-Page.ino...");

  psramInit();
  
  initializeBattery();

  initializeLight();
  initializeWiFi();
  initializeCamera();
  
  // For every resource available on the server, we need to create a ResourceNode
  // The ResourceNode links URL and HTTP method to a handler function
  ResourceNode * nodeRoot    = new ResourceNode("/", "GET", &handleRoot);
  ResourceNode * nodeFavicon = new ResourceNode("/favicon.ico", "GET", &handleFavicon);
  ResourceNode * node404     = new ResourceNode("", "GET", &handle404);

  // Add the root node to the server
  server0.registerNode(nodeRoot);

  // Add the favicon
  server0.registerNode(nodeFavicon);

  // Add the 404 not found node to the server.
  // The path is ignored for the default node.
  server0.setDefaultNode(node404);

  Serial.println("Starting server 0...");
  server0.start();
  if (server0.isRunning()) {
    Serial.println("Server 0 ready.");
  }

  BaseType_t xReturned;
  TaskHandle_t xHandle = NULL;

  xReturned = xTaskCreatePinnedToCore(
        cameraServerTask,         /* Task function. */
        "cameraServerTask",      /* String with name of task. */
        20000,               /* Stack size in bytes. */
        NULL,                /* Parameter passed as input of the task */
        1,                     /* Priority of the task. */
        &xHandle            /* Task handle. */,
        0                      /* Pinned to core */
  );         

}

void cameraServerTask( void * input ) {

  ResourceNode * nodeCamera  = new ResourceNode("/camera", "GET", &handleCamera);

  server1.registerNode(nodeCamera);

  Serial.println("Starting server 1...");
  server1.start();
  if (server1.isRunning()) {
    Serial.println("Server 1 ready.");
  }

  for (;;) {

    server1.loop();

    delay(1);

  }


}

void loop() {
  // This call will let the server do its work
  server0.loop();

  static unsigned long weatherReportTime = 0;

  if (FeebeeCam::downloadWhenReady) {
    FeebeeCam::downloadWhenReady = false;
    initializeSettings();
    settings.applyToCamera();
  }

  if (  millis() > weatherReportTime  &&
        _setup._secretHash.length() &&
        FeebeeCam::connectedToInternet ) {
          
    //uploadWeatherReport2();

    weatherReportTime = millis() + 20000;

  }

  // Other code would go here...
  delay(1);
}

void handleRoot(HTTPRequest * req, HTTPResponse * res) {
  // Status code is 200 OK by default.
  // We want to deliver a simple HTML page, so we send a corresponding content type:
  res->setHeader("Content-Type", "text/html");
  res->setHeader("Connection", "close");

  // The response implements the Print interface, so you can use it just like
  // you would write to Serial etc.
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Hello World!</title></head>");
  res->println("<body>");
  res->println("<h1>Hello World!</h1>");
  res->print("<p>Your server is running at ");
  // A bit of dynamic data: Show the uptime
  res->print(FeebeeCam::getFramerate());
  res->println(" frames per second.</p>");
  res->println("</body>");
  res->println("</html>");
}

void handleFavicon(HTTPRequest * req, HTTPResponse * res) {
  // Set Content-Type
  res->setHeader("Content-Type", "image/vnd.microsoft.icon");
  // Write data from header file
  res->write(FAVICON_DATA, FAVICON_LENGTH);
}

void handle404(HTTPRequest * req, HTTPResponse * res) {
  // Discard request body, if we received any
  // We do this, as this is the default node and may also server POST/PUT requests
  req->discardRequestBody();

  // Set the response status
  res->setStatusCode(404);
  res->setStatusText("Not Found");

  // Set content type of the response
  res->setHeader("Content-Type", "text/html");

  // Write a tiny HTTP page
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Not Found</title></head>");
  res->println("<body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body>");
  res->println("</html>");
}

#define PART_BOUNDARY "123456789000000000000987654321"
#define _STREAM_CONTENT_TYPE "multipart/x-mixed-replace;boundary=" PART_BOUNDARY
#define _STREAM_BOUNDARY "\r\n--" PART_BOUNDARY "\r\n"
#define _STREAM_PART  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"

void handleCamera(HTTPRequest * request, HTTPResponse * response) {
    
    Serial.println("Camera get");

    FeebeeCam::stop = false;
    if (FeebeeCam::isRunning) {
      FeebeeCam::stop = true;
      while  (FeebeeCam::isRunning)
        delay(1);
    }
    FeebeeCam::stop = false;
    
    camera_fb_t * frameBuffer = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len;
    uint8_t * _jpg_buf;
    char * part_buf[64];
    
    Data streamBoundary((byte*)_STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));

    response->setHeader("Content-Type", _STREAM_CONTENT_TYPE);
    response->setHeader("Transfer-Encoding", "chunked");
    response->setHeader("Connection", "close");
    
    
    FeebeeCam::stop = false;
    FeebeeCam::isRunning = true;

    Serial.println("Starting camera loop");

    //settings.save();

    // Turn on RED
    FeebeeCam::light->turnOn();
    
    HTTPConnection* connection = (HTTPConnection*)(response->_con);

    while(!connection->isClosed() && !connection->isError() && !FeebeeCam::stop) {

//            delay(1);

        //esp_task_wdt_restart();
        //taskYIELD();
        //yield();

        frameBuffer = esp_camera_fb_get();
        if (!frameBuffer) {
            DEBUG_OUT("Camera capture failed");
            break ;
        } 

        const Data capturedFrame(frameBuffer->buf, frameBuffer->len);

        size_t headerLength = snprintf((char *)part_buf, 64, _STREAM_PART, capturedFrame.size());

        if (!sendChunk(response, Data((byte*)part_buf, headerLength)))
          break;

        if (!sendChunk(response, capturedFrame))
          break;

        if (!sendChunk(response, streamBoundary))
          break;

        esp_camera_fb_return(frameBuffer);

        ++frameCount;

        int64_t frameEndTime = esp_timer_get_time();
        int64_t frameTime = frameEndTime - FeebeeCam::lastTimeFramesCounted;
        FeebeeCam::framesPerSecond =
            1000.00 * 1000.00 * (float)FeebeeCam::frameCount / (float)frameTime;

        if (FeebeeCam::pause) {

            Serial.println("Paused");
            FeebeeCam::isPaused = true;

            while (FeebeeCam::pause) {
                delay(10);
            }

        }

        if (FeebeeCam::isPaused) {

            Serial.println("Resuming");
            FeebeeCam::isPaused = false;

            // Turn on RED
            FeebeeCam::light->turnOn();            

        }

    }

    sendChunk(response);
    
    Serial.println("Camera loop ended");

    FeebeeCam::light->turnOff();

    FeebeeCam::stop = false;
    FeebeeCam::isRunning = false;
    FeebeeCam::isPaused = false;
    FeebeeCam::pause = false;

    connection->closeConnection();
}


bool sendChunk(HTTPResponse* response, const Data& data) {
    
    response->println(data.size(), HEX);
    
    if (data.size()) {
      size_t printed = 
        response->write(data.data(), data.size());
      
      if (printed != data.size())
        return false;
    }
    
    return response->println() > 0;

}

bool sendChunk(HTTPResponse* response) {
    Data data;
    return sendChunk(response, data);
}


bool uploadWeatherReport2() {

  if (!BeeFishWebRequest::logon(_setup._secretHash)) {
    return false;
  }

  BeeFishId::Id id;

  BeeFishStorage storage;

  bool uploaded = storage.setItem("/beehive/weather/", id, weather.getWeather());

  if (uploaded)
    cout << "Weather report uploaded with id " << id << endl;
  else
    Serial.println("Error uploading weather report");
  
  return uploaded;

}