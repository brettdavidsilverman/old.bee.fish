#define HTTPS_LOGLEVEL 4
#define HTTPS_KEEPALIVE_CACHESIZE 2048

#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"
#include <secure-web-server.h>
#include <file-server.h>
#include <feebee-cam.h>

#define SSID "laptop"
#define PASSWORD "feebeegeeb3"

using namespace FeebeeCam;
FeebeeCam::SecureWebServer* secureServer1;
FeebeeCam::SecureWebServer* secureServer2;
void serveFile(const BString& filename, HTTPResponse * res);
bool handleCamera(HTTPResponse * res);
bool handleCommand(HTTPRequest* req, HTTPResponse * res);
bool sendChunk(HTTPResponse * res, const BeeFishBString::Data& data);
bool sendChunk(HTTPResponse * res);

void setup() {

    Serial.begin(1500000);

    while (!Serial)
        delay(10);

    Serial.println("SecureWebServer starting");

    Serial.print("Connecting to WiFi");

    WiFi.begin(SSID, PASSWORD);

    while (!WiFi.isConnected())
    {
        Serial.print(".");
        delay(500);
        
    }

    Serial.println("Connected");

    Serial.println(WiFi.localIP());
    
    
    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeLight();
    FeebeeCam::initializeBattery();
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeCamera();
    FeebeeCam::initializeCommands();
    
    Serial.println("Creating secure web server");
    
    secureServer1 = new FeebeeCam::SecureWebServer(1, 443);
    secureServer2 = new FeebeeCam::SecureWebServer(0, 444);

    ResourceNode * nodeRoot = 
        new ResourceNode("/", "GET", [](HTTPRequest * req, HTTPResponse * res)
        {
            res->setHeader("Connection", "keep-alive");
            std::string response = "Secure Hello World Root!!!";
            std::stringstream stream;
            stream << response.size();
            res->setHeader("Content-Length", stream.str().c_str());
            res->write((const uint8_t*)response.data(), response.size());
        }
    );
 
    secureServer1->registerNode(nodeRoot);
 
    ResourceNode * nodeWeather = 
        new ResourceNode("/weather", "GET", [](HTTPRequest * req, HTTPResponse * res)
        {
            res->setHeader("Content-type", "text/javascript; charset=utf-8");
            BeeFishBScript::Object reading = weather.getWeather();
            std::string response = reading.str();
            std::stringstream stream;
            stream << response.size();
            res->setHeader("Content-Length", stream.str().c_str());
            res->write((const uint8_t*)response.data(), response.size());
        }
    );
 
    secureServer1->registerNode(nodeWeather);

    ResourceNode * defaultNode  = 
        new ResourceNode("", "GET", [](HTTPRequest * req, HTTPResponse * res)
        {
            std::string filename = req->getRequestString();
            serveFile(filename.c_str(), res);
        }
    );
 
    secureServer1->setDefaultNode(defaultNode);

    ResourceNode * nodeCamera = 
        new ResourceNode("/camera", "GET", [](HTTPRequest * req, HTTPResponse * res)
        {
            handleCamera(res);
        }
    );
 
    secureServer2->registerNode(nodeCamera);
 
    ResourceNode * nodeCommand = 
        new ResourceNode("/command", "POST", [](HTTPRequest * req, HTTPResponse * res)
        {
            handleCommand(req, res);
        }
    );
 
    secureServer1->registerNode(nodeCommand);

    std::string allowOrigin1 = std::string("https://") + WiFi.localIP().toString().c_str() + ":443";
    std::string allowOrigin2 = std::string("https://") + WiFi.localIP().toString().c_str() + ":444";

    secureServer1->setDefaultHeader("Access-Control-Allow-Origin", allowOrigin1.c_str());
    secureServer2->setDefaultHeader("Access-Control-Allow-Origin", allowOrigin2.c_str());
    secureServer1->setDefaultHeader("Connection", "keep-alive");
    secureServer2->setDefaultHeader("Connection", "keep-alive");
    
    secureServer1->start();
    secureServer2->start();
   
    if (  secureServer1->isRunning() && 
          secureServer2->isRunning()  )
    {
        Serial.println("Server ready.");
    }
}

void serveFile(const BString& filename, HTTPResponse * res) {

    BString _filename = filename;

    if (_filename.endsWith("/"))
        _filename += "index.html";
    
    Serial.print("Getting ");
    Serial.print(_filename.c_str());
    Serial.print(" ");

    if (SPIFFS.exists(_filename.c_str())) {

        File file = SPIFFS.open(_filename.c_str(), "r");
        res->setStatusCode(200);
        res->setStatusText("Ok");

        vector<BString> parts = _filename.split('.');
        BString extension = parts[parts.size() - 1];
        BString contentType = FeebeeCam::CONTENT_TYPES[extension];

        res->setHeader("Connection", "keep-alive");
        res->setHeader("Content-Type", contentType.c_str());

        bool cacheRule = FeebeeCam::CACHE_RULES[extension];
        
        cacheRule = true;

        if (cacheRule)
            res->setHeader("Cache-Control", "public, max-age=31536000, immutable");
        else
            res->setHeader("Cache-Control", "no-store, max-age=0");

        std::stringstream stream;
        stream << file.size();
        res->setHeader("Content-Length", stream.str().c_str());

        // Finished headers
        size_t size = file.size();
        size_t chunkSize = getPageSize();
        size_t written = 0;
        uint8_t* buffer = (uint8_t*)malloc(chunkSize);
        while (written < size) {
            if (written + chunkSize > size)
                chunkSize = size - written;
            written += file.read(buffer, chunkSize);
            res->write(buffer, chunkSize);
            //Serial.write(buffer, chunkSize);
        }
        file.close();
        free(buffer);
        Serial.println("Ok");
    }
    else {
        res->setStatusCode(400);
        res->setStatusText("Not Found");
        res->setHeader("Connection", "keep-alive");
        res->setHeader("Content-Type", "text/javascript");
        std::string response = "{\"status\": \"Not found\"}";
        res->write((Byte*)response.c_str(), response.size());
        Serial.println("File Not Found");
    }

};

void loop() {
    FeebeeCam::handleCommandLine();
}

#define PART_BOUNDARY "123456789000000000000987654321"
#define _STREAM_CONTENT_TYPE "multipart/x-mixed-replace;boundary=" PART_BOUNDARY
#define _STREAM_BOUNDARY "\r\n--" PART_BOUNDARY "\r\n"
#define _STREAM_PART  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"

bool handleCamera(HTTPResponse * res) {
        
    Serial.println("Camera");

    if (FeebeeCam::isCameraRunning) {
        FeebeeCam::stop = true;
        while  (FeebeeCam::isCameraRunning)
            delay(1);
    }
    FeebeeCam::stop = false;
    
    camera_fb_t * frameBuffer = NULL;
    size_t _jpg_buf_len;
    uint8_t * _jpg_buf;
    char * part_buf[64];
    
    Data streamBoundary((byte*)_STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));

    res->setStatusCode(200);
    res->setStatusText("Ok");
    res->setHeader("Content-Type", _STREAM_CONTENT_TYPE);
    res->setHeader("Transfer-Encoding", "chunked");

    FeebeeCam::isCameraRunning = true;

    Serial.println("Starting camera loop");

    // Turn on RED
    FeebeeCam::light->turnOn();

    bool error = false;

    int count = 0;

    while(!error && !FeebeeCam::stop) {

        frameBuffer = esp_camera_fb_get();
        if (!frameBuffer) {
            cerr << "Camera capture failed" << endl;
            continue;
        } 

        const Data capturedFrame(frameBuffer->buf, frameBuffer->len);

        size_t headerLength = snprintf((char *)part_buf, 64, _STREAM_PART, capturedFrame.size());

        if (!sendChunk(res, Data((byte*)part_buf, headerLength))) {
            error = true;
            break;
        }

        if (!sendChunk(res, capturedFrame)) {
            error = true;
            break;
        }

        if (!sendChunk(res, streamBoundary)) {
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

        if ((++count % 60) == 0) {
            std::cerr << "Frames per second: " << FeebeeCam::getFrameRate() << std::endl;
            count = 0;
        }

        delay(1);

    }

    if (frameBuffer)
        esp_camera_fb_return(frameBuffer);

    if (!error) {
        if (!sendChunk(res))
            error = true;
    }
    
    
    Serial.println("Camera loop ended");

    FeebeeCam::light->turnOff();

    FeebeeCam::stop = false;
    FeebeeCam::isCameraRunning = false;
    FeebeeCam::isPaused = false;
    FeebeeCam::pause = false;

    return !error;



}

bool sendChunk(HTTPResponse * res, const BeeFishBString::Data& data) {

    std::stringstream stream;
    stream << std::hex << data.size();
    std::string size = stream.str();

    if (res->write((const uint8_t*)size.c_str(), size.size()) != size.size())
        return false;

    if (res->write((const uint8_t*)"\r\n", 2) != 2)
        return false;

    if (res->write(data._data, data.size()) != data.size())
        return false;

    if (res->write((const uint8_t*)"\r\n", 2) != 2)
        return false;

    return true;
}

bool sendChunk(HTTPResponse * res) {
    BeeFishBString::Data data;
    return sendChunk(res, data);
}


bool handleCommand(HTTPRequest* req, HTTPResponse * res) {
    
    using namespace BeeFishBString;
    using namespace BeeFishJSON;
    using namespace BeeFishParser;

    BeeFishBScript::Object object;
    object["status"] = BeeFishBScript::Null();
    object["message"] = "Invalid command";

    BeeFishJSON::JSON json;
    BeeFishBScript::BScriptParser parser(json);

    BeeFishBString::Data buffer = BeeFishBString::Data::create();

    while (!req->requestComplete()) {
        size_t read = req->readBytes(buffer.data(), buffer.size());
        parser.read(buffer, read);
    }

    BeeFishBScript::ObjectPointer request = 
        (BeeFishBScript::ObjectPointer)(parser.value());

    // Command
    BString command = (*request)["command"];

    bool _putToSleep = false;

    if (command == "stop") {
        FeebeeCam::stop = true;
        object["status"] = true;
        object["message"] = "Camera stopped";
    }
    else if (command == "reset") {
        FeebeeCam::setup.reset();
        FeebeeCam::setup.save();
        FeebeeCam::setup.applyToCamera();
        object["status"] = true;
        object["message"] = "Camera reset";
    }
    else if (command == "sleep") {
        object["status"] = true;
        object["message"] = "Camera put to sleep";
        object["redirectURL"] = HOST "/beehive/";
        //commands.push(PUT_TO_SLEEP);
        _putToSleep = true;
    }
            
    
    Serial.print("Sent Camera command ");
    Serial.println(command.c_str());

    res->setHeader("Content-Type", "text/javascript");

    std::stringstream objectStream;
    objectStream << object;
    std::string objectString = objectStream.str();
    std::stringstream sizeStream;
    sizeStream << objectString.size();
    
    res->setHeader("Content-Length", sizeStream.str());

    res->write((const uint8_t*)objectString.c_str(), objectString.size());

    if (_putToSleep) {
        FeebeeCam::commands.push(FeebeeCam::PUT_TO_SLEEP);
    }

    return true;

}

