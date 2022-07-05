#include <feebee-cam.h>
#include <queue>

bool uploadSettings();

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
    FeebeeCam::initializeCommands();

}



void loop() {

    if (FeebeeCam::downloadWhenReady) {
        
        FeebeeCam::downloadWhenReady = false;

        Serial.println("Connected to WiFi");

        //commands.push(INITIALIZE);
        uploadSettings();

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


bool uploadSettings() {

    FeebeeCam::BeeFishStorage storage("/beehive/");
    BeeFishBScript::Variable variable = storage.getItem("settings");
    BeeFishBScript::Object status;

    bool result = true;

    if (variable == nullptr) {
        status["checkEvery"] = 30;
        status["photographMinutes"] = 1;
        status["wakeup"] = false;
    }
    else {
        status = *(BeeFishBScript::ObjectPointer)variable;
        bool wakeUp = false;

        if (status.contains("wakeup"))
            wakeUp = status["wakeup"];
        
        if (!wakeUp) {
            FeebeeCam::putToSleep();
            return true;
        }
    }

    status["label"] = FeebeeCam::setup._label,
    status["url"] = BString("http://") + WiFi.localIP().toString().c_str();
    status["awake"] = true;

    result = storage.setItem("settings", status);
    
    if (result)
        clog << "Uploaded beehive settings" << endl;
    else
        clog << "Error uploading beehive settings" << endl;

    return result;

}