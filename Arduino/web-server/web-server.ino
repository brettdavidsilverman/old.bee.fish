#include <feebee-cam.h>
#include <queue>
#include <esp_task_wdt.h>


void webServerTask(void*);

bool handleRoot(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);
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

    FeebeeCam::initializeWebServer();

    initializeCommandLoop();

}


bool initializeCommandLoop() {
    TaskHandle_t xHandle = NULL;
            
    xTaskCreatePinnedToCore(
        commandLoop,      // Task function. 
        "commandLoop",      // String with name of task. 
        5000,                // Stack size in bytes. 
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


void loop() {

    if (FeebeeCam::downloadWhenReady) {
        
        FeebeeCam::downloadWhenReady = false;

        Serial.println("Connected to WiFi");

        //commands.push(INITIALIZE);

        FeebeeCam::light->turnOff();
    }

    static unsigned long uploadWeatherReportTime = 0;
    const unsigned long weatherReportInterval = 20000;

    if (FeebeeCam::connectedToInternet) {
        if (uploadWeatherReportTime < millis()) {
            //commands.push(UPLOAD_WEATHER);
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


void commandLoop(void *) {
    for (;;) {
        esp_task_wdt_reset();
        while (!commands.empty()) {
            command_t command = commands.pop();

            switch (command) {
            case INITIALIZE:
                FeebeeCam::downloadRequiredFiles();
                FeebeeCam::initializeSettings();
                FeebeeCam::settings.applyToCamera();
                break;
            case SAVE_SETTINGS:
                FeebeeCam::settings.save();
                break;
            case UPLOAD_WEATHER:
                FeebeeCam::uploadWeatherReport();
                break;
            default:
                ;
            }
        }
        delay(190);
    }
}


