#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <iostream>

char label[256] = "Goodbye World";

//WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wifiManager;
WiFiManagerParameter custom_parameter("label", "Label for esp", label, 256);


void saveConfigCallback () {
  std::cerr << "Save Config call back" << std::endl;
  std::cerr << "Label: " << custom_parameter.getValue() << std::endl;
}

void setup() {
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(1500000);
    
    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    wifiManager.resetSettings();


      //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    //add all your parameters here
    wifiManager.addParameter(&custom_parameter);

//    RequestHandler handler = 
//    wifiManager.server->addHandler(RequestHandler *handler)
    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wifiManager.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wifiManager.autoConnect(); // auto generated AP name from chipid
    // res = wifiManager.autoConnect("AutoConnectAP"); // anonymous ap
    res = wifiManager.autoConnect("FeebeeCam"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }

}

void loop() {
    // put your main code here, to run repeatedly:   
}
