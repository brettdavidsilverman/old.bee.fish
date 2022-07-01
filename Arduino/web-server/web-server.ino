#include <feebee-cam.h>
#include <queue>
#include <esp_task_wdt.h>

BeeFishWebServer::WebServer* webServer;

void webServerTask(void*);

bool handleRoot(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);
bool handleCamera(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);
bool uploadWeatherReport();
void commandLoop(void*);
bool initializeCommandLoop();

enum command_t {
    DO_NOTHING,
    INITIALIZE,
    SAVE_SETTINGS,
    UPLOAD_WEATHER
};

class Commands : public std::queue<command_t> {
protected:
    std::map<command_t, bool> _commands;

public:
    Commands() {

    }

    virtual void push(command_t command) {
        if (_commands[command] == false) {
            std::queue<command_t>::push(command);
            _commands[command] = true;
        }
    }

    virtual command_t pop() {
        command_t command = front();
        std::queue<command_t>::pop();
        _commands[command] = false;
        return command;
    }
};

Commands commands;

void setup() {

    FeebeeCam::initializeMemory();

    FeebeeCam::initializeSerial();

    FeebeeCam::initializeLight();
    
    FeebeeCam::light->turnOn();

    FeebeeCam::initializeBattery();
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeCamera();
    FeebeeCam::initializeWiFi();

    initializeWebServer();
    initializeCommandLoop();

}


bool initializeCommandLoop() {
    TaskHandle_t xHandle = NULL;
            
    xTaskCreatePinnedToCore(
        commandLoop,      // Task function. 
        "commandLoop",      // String with name of task. 
        20000,                // Stack size in bytes. 
        NULL,                 // Parameter passed as input of the task 
        1,     // Priority of the task. 
        &xHandle,             // Task handle
        1                  // Pinned to core 
    );

    if (xHandle == NULL) {
        cerr << "Error starting command loop task" << endl;
        return false;
    }

    return (xHandle != NULL);
}

bool initializeWebServer() {

    webServer = new BeeFishWebServer::WebServer(80);

    webServer->paths()["/"] = handleRoot;
    webServer->paths()["/camera"] = handleCamera;

    webServer->start();

    return true;

}


void loop() {

    if (FeebeeCam::downloadWhenReady) {
        
        FeebeeCam::downloadWhenReady = false;

        Serial.println("Connected to WiFi");

        commands.push(INITIALIZE);

        FeebeeCam::light->turnOff();
    }

    static unsigned long uploadWeatherReportTime = 0;
    const unsigned long weatherReportInterval = 20000;

    if (FeebeeCam::connectedToInternet) {
        if (uploadWeatherReportTime < millis()) {
            commands.push(UPLOAD_WEATHER);
            uploadWeatherReportTime = millis() + weatherReportInterval;
        }
    }

    FeebeeCam::handleCommandLine();

    delay(10);
}

#define PART_BOUNDARY "123456789000000000000987654321"
#define _STREAM_CONTENT_TYPE "multipart/x-mixed-replace;boundary=" PART_BOUNDARY
#define _STREAM_BOUNDARY "\r\n--" PART_BOUNDARY "\r\n"
#define _STREAM_PART  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"

bool handleRoot(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client) {

    std::cerr << "::handleRoot" << std::endl;

    std::cerr << "::handleRoot::settings.initialize" << std::endl;

    client->_contentType = "text/html";

    std::cerr << "::handleRoot::sendHeaders" << std::endl;

    if (!client->sendHeaders())
        return false;

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

    commands.push(SAVE_SETTINGS);

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

        delay(10);

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

void commandLoop(void *) {
    for (;;) {
        esp_task_wdt_reset();
        while (!commands.empty()) {
            command_t command = commands.pop();
            switch (command) {
            case INITIALIZE:
                //FeebeeCam::downloadRequiredFiles();
                //FeebeeCam::initializeSettings();
                //FeebeeCam::settings.applyToCamera();
                break;
            case SAVE_SETTINGS:
                //FeebeeCam::settings.save();
                break;
            case UPLOAD_WEATHER:
                uploadWeatherReport();
                break;
            default:
                ;
            }
        }
        delay(190);
    }
}

bool uploadWeatherReport() {

    if (FeebeeCam::_setup._secretHash.length() == 0) {
        cerr << "Missing setup secret hash " << endl;
        return false;
    }
/*
    if (!FeebeeCam::BeeFishWebRequest::logon(FeebeeCam::_setup._secretHash)) {
        cerr << "Couldnt upload weather report... Couldn't log on" << endl;
        return false;
    }
*/
    static FeebeeCam::BeeFishStorage* storage = nullptr;
    
    if (!storage)
        storage = new FeebeeCam::BeeFishStorage("/beehive/weather/");

    BeeFishId::Id id;

    bool uploaded = storage->setItem(id, FeebeeCam::weather.getWeather());

    if (uploaded)
        cout << "Weather report uploaded with id " << id << endl;
    else {
        cerr << "Error uploading weather report" << endl;
        delete storage;
        storage = nullptr;
    }

    return uploaded;

}

