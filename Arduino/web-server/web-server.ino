#include <feebee-cam.h>
#include <queue>

bool uploadSettings();

void setup() {

    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeLight();
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

    }

    static unsigned long uploadWeatherReportTime = 0;
    const unsigned long weatherReportInterval = 20000;

    if (FeebeeCam::connectedToInternet) {
        if (uploadWeatherReportTime < millis()) {
            FeebeeCam::commands.push(FeebeeCam::UPLOAD_WEATHER);
            uploadWeatherReportTime = millis() + weatherReportInterval;
        }
    }

    FeebeeCam::handleCommandLine();

    delay(10);
}


bool uploadSettings() {

    FeebeeCam::BeeFishStorage storage("/beehive/");
    BeeFishBScript::Variable variable = storage.getItem("settings");

    bool result = true;

    cerr << "Variable: " << variable << endl;

    if (variable == nullptr) {
        cerr << "Creating default settings" << endl;
        FeebeeCam::settings.clear();
        FeebeeCam::settings["checkEvery"] = 30;
        FeebeeCam::settings["photographMinutes"] = 1;
        FeebeeCam::settings["wakeup"] = false;
    }
    else {
        cerr << "Using settings from cloud" << endl;
        FeebeeCam::settings = *(BeeFishBScript::ObjectPointer)variable;
        bool wakeup = false;

        if (FeebeeCam::settings.contains("wakeup"))
            wakeup = FeebeeCam::settings["wakeup"];
        
        if (!wakeup) {
            FeebeeCam::putToSleep();
            return true;
        }
    }

    FeebeeCam::settings["label"] = FeebeeCam::setup._label,
    FeebeeCam::settings["url"] = BString("http://") + WiFi.localIP().toString().c_str();
    FeebeeCam::settings["sleeping"] = false;

    result = storage.setItem("settings", FeebeeCam::settings);
    
    if (result)
        clog << "Uploaded beehive settings" << endl;
    else
        clog << "Error uploading beehive settings" << endl;

    return result;

}