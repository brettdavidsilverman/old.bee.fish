#include <feebee-cam.h>

BeeFishWebServer::WebServer* webServer80;
BeeFishWebServer::WebServer* webServer81;

void webServerTask(void*);

bool handleRoot(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);
bool handleCamera(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);
bool uploadWeatherReport();

void setup() {

    FeebeeCam::initializeSerial();
    FeebeeCam::initializeMemory();
    FeebeeCam::initializeLight();
    
    FeebeeCam::light->turnOn();

    FeebeeCam::initializeBattery();
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeCamera();
    FeebeeCam::initializeWiFi();

    initializeWebServer();
}


bool initializeWebServer() {

    webServer80 = new BeeFishWebServer::WebServer(80);
    webServer81 = new BeeFishWebServer::WebServer(81);

    webServer80->paths()["/"] = handleRoot;
    webServer81->paths()["/"] = handleCamera;


    TaskHandle_t xHandle = NULL;

    xTaskCreatePinnedToCore(
            webServerTask,        // Task function. 
            "webServerTask80",      // String with name of task. 
            20000,                // Stack size in bytes. 
            webServer80,          // Parameter passed as input of the task 
            1,                    // Priority of the task. 
            &xHandle,             // Task handle
            1                     // Pinned to core 
    );       

    xTaskCreatePinnedToCore(
            webServerTask,        // Task function. 
            "webServerTask81",      // String with name of task. 
            20000,                // Stack size in bytes. 
            webServer81,          // Parameter passed as input of the task 
            1,                    // Priority of the task. 
            &xHandle,             // Task handle
            0                     // Pinned to core 
    );       

    return true;

}

void webServerTask(void* server) {

    BeeFishWebServer::WebServer* webServer = 
        (BeeFishWebServer::WebServer*)server;


    for (;;) {
        Serial.println("Starting web server");
        webServer->start();
        Serial.println("Server quit unexpectedly");
        delay(5000);
    }

    

}

void loop() {

    if (FeebeeCam::downloadWhenReady) {
        
        FeebeeCam::downloadWhenReady = false;

        Serial.println("Connected to WiFi");

        FeebeeCam::downloadRequiredFiles();
        FeebeeCam::initializeSettings();
        FeebeeCam::settings.applyToCamera();
        FeebeeCam::light->turnOff();
    }

    static unsigned long uploadWeatherReportTime = 0;
    const unsigned long weatherReportInterval = 20000;

    if (FeebeeCam::connectedToInternet) {
        if (uploadWeatherReportTime < millis()) {
            uploadWeatherReport();
            uploadWeatherReportTime = millis() + weatherReportInterval;
        }
    }

    FeebeeCam::handleCommandLine();

}

#define PART_BOUNDARY "123456789000000000000987654321"
#define _STREAM_CONTENT_TYPE "multipart/x-mixed-replace;boundary=" PART_BOUNDARY
#define _STREAM_BOUNDARY "\r\n--" PART_BOUNDARY "\r\n"
#define _STREAM_PART  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"

bool handleRoot(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client) {

    std::cerr << "::handleRoot" << std::endl;

    client->_contentType = "text/html";

    std::cerr << "::handleRoot::sendHeaders" << std::endl;

    if (!client->sendHeaders())
        return false;

    std::cerr << "::handleRoot::settings.initialize" << std::endl;
    FeebeeCam::settings.initialize();

    BeeFishBString::BStream output = client->getChunkedOutputStream();

    std::cerr << "::handleRoot::output <<" << std::endl;
    output << "<html>" << endl
           << "   <body>" << endl
           << "      <h1>Settings</h1>" << endl
           << "      <pre>" << endl
           << FeebeeCam::settings << endl
           << "      </pre>" << endl
           << "      <h1>Weather</h1>" << endl
           << "      <pre>" << endl
           << FeebeeCam::weather.getWeather() << endl
           << "      </pre>" << endl
           << "   </body>" << endl
           << "</html>";

    output.flush();

    if(!client->sendChunk())
        return false;

    std::cerr << "::~handleRoot" << std::endl;

    return true;
}

bool handleCamera(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client) {
    
    Serial.println("Camera");
    
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

    client->_contentType = _STREAM_CONTENT_TYPE;

    if (!client->sendHeaders())
        return false;
    
    FeebeeCam::stop = false;
    FeebeeCam::isRunning = true;

    Serial.println("Starting camera loop");

    //settings.save();

    // Turn on RED
    FeebeeCam::light->turnOn();

    bool error = false;

    while(!error && !FeebeeCam::stop) {

        frameBuffer = esp_camera_fb_get();
        if (!frameBuffer) {
            cerr << "Camera capture failed" << endl;
            continue;
        } 

        const Data capturedFrame(frameBuffer->buf, frameBuffer->len);

        size_t headerLength = snprintf((char *)part_buf, 64, _STREAM_PART, capturedFrame.size());

        if (!client->sendChunk(Data((byte*)part_buf, headerLength))) {
            error = true;
            break;
        }

        if (!client->sendChunk(capturedFrame)) {
            error = true;
            break;
        }

        if (!client->sendChunk(streamBoundary)) {
            error = true;
            break;
        }

        esp_camera_fb_return(frameBuffer);

        ++FeebeeCam::frameCount;

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

    if (frameBuffer)
        esp_camera_fb_return(frameBuffer);

    if (!error) {
        if (!client->sendChunk())
            error = true;
    }
    
    
    Serial.println("Camera loop ended");

    FeebeeCam::light->turnOff();

    FeebeeCam::stop = false;
    FeebeeCam::isRunning = false;
    FeebeeCam::isPaused = false;
    FeebeeCam::pause = false;

    return !error;

}

bool uploadWeatherReport() {

    if (!FeebeeCam::BeeFishWebRequest::logon(FeebeeCam::_setup._secretHash)) {
        return false;
    }

    BeeFishId::Id id;

    FeebeeCam::BeeFishStorage storage;

    bool uploaded = storage.setItem("/beehive/weather/", id, FeebeeCam::weather.getWeather());

    if (uploaded)
        cout << "Weather report uploaded with id " << id << endl;
    else
        Serial.println("Error uploading weather report");

    return uploaded;

}