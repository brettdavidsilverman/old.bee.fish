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
    FeebeeCam::initializeCommands();

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

    FeebeeCam::BeeFishStorage storage("/beehive/");
    BeeFishBScript::Variable variable = storage.getItem("status");
    BeeFishBScript::Object status;

    bool result = true;

    if (variable == undefined) {
        status["label"] = FeebeeCam::setup._label,
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
            long sleepTime = (double)status["checkEvery"] * 1000L * 1000L;
            FeebeeCam::putToSleep(sleepTime);
            return true;
        }
    }

    status["url"] = BString("http://") + WiFi.localIP().toString().c_str();

    result = storage.setItem("status", status);
    
    if (result)
        clog << "Uploaded beehive status" << endl;
    else
        clog << "Error uploading beehive status" << endl;

    return result;

}