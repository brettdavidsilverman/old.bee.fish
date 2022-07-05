#include <feebee-cam.h>
#include <queue>

bool uploadRunningStatus();

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
    //FeebeeCam::initializeCommands();

}



void loop() {

    if (FeebeeCam::downloadWhenReady) {
        
        FeebeeCam::downloadWhenReady = false;

        Serial.println("Connected to WiFi");

        //commands.push(INITIALIZE);
        uploadRunningStatus();

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


bool uploadRunningStatus() {

    BeeFishBScript::Object status  {
        {"label", FeebeeCam::setup._label},
        {"url", BString("http://") + WiFi.localIP().toString().c_str()},
        {"awake", true},
        {"checkEvery", 30},
        {"photographMinutes", 1}
    };

    FeebeeCam::BeeFishStorage storage("/beehive/");
    
    bool result = storage.setItem("status", status);

    if (result)
        clog << "Uploaded beehive status" << endl;
    else
        clog << "Error uploading beehive status" << endl;

    return result;

}