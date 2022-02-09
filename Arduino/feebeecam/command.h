#include "web-server-helpers.h"
#include "camera-pins.h"
#include "camera.h"
#include "sensor.h"
#include "esp_camera.h"
#include "light.h"
#include <bee-fish.h>

namespace FeebeeCam {

    void onCommandPost(httpsserver::HTTPRequest * request, httpsserver::HTTPResponse * response) {
        
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        BeeFishJSONOutput::Object object;
        object["status"] = BeeFishJSONOutput::Null();
        object["message"] = "Invalid command";
        
        // Command
        BString command;
        JSONParser::OnValue oncommand = 
            [&object, &command](const BString& key, JSON& json) {
                command = json.value();
                if (command == "stop") {
                    FeebeeCam::stop = true;
                    object["status"] = true;
                    object["message"] = "Camera stopped";
                }
                else if (command == "save") {
                    esp_camera_save_to_nvs("user");
                    object["status"] = true;
                    object["message"] = "User settings saved";
                }
                else if (command == "capture") {
                    
                    Light light;

                    // Set stop flag to initiate stop camera stream procecss
                    FeebeeCam::stop = true;

                    // Whilst were waiting, save the camera settings to temporary
                    // so we can recall after changing capture specific settings
                    esp_err_t err = esp_camera_save_to_nvs("system");

                    // Wait for camera process to halt
                    while (FeebeeCam::isRunning) {
                        yield();
                    }

                    // Set capture specific settings...
                    sensor_t *s = esp_camera_sensor_get();

                    // Largest frame size?
                    s->set_framesize(s, FRAMESIZE_XGA); //FRAMESIZE_QXGA

                    // Flush the frame buffer queue
                    for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
                    {
                        camera_fb_t* fb = esp_camera_fb_get();
                        if (fb)
                            esp_camera_fb_return(fb);
                    }
                    
                    // Set lights on
                    light.turnOn();

                    // Take the picture
                    camera_fb_t* fb = esp_camera_fb_get();

                    // Turn light off
                    light.turnOff();

                    if (!fb) {
                        Serial.println("Camera capture failed");
                    } 

                    err = esp_camera_load_from_nvs("system");
                    
                    object["status"] = true;
                    object["message"] = "Took picture";
                    object["type"] = "data:image/jpeg;base64,";
                    object["base64"] = BeeFishBase64::encode(fb->buf, fb->len);

                    if (fb)
                        esp_camera_fb_return(fb);

                }
            };
        
        BeeFishJSON::JSON json;

        BeeFishJSON::JSONParser parser(json);

        parser.invokeValue("command", oncommand);

        parseRequest(parser, request);
        
        sendResponse(response, object);

        Serial.print("Sent Camera command ");
        Serial.println(command.c_str());

    }
}