#pragma once

#include <WiFi.h>
#include "i2c.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"
#include "web-server-helpers.h"
#include "battery.h"
#include "camera.h"

namespace FeebeeCam {

    class Weather {
    private:
        TwoWire* _wire;
        Adafruit_BME280 _bme; // I2C
        int _port;
    public:

        Weather(int port = 0);

        float temperature();

        float pressure();

        float humidity();

        void print(Stream& output);

    };

    BeeFishJSONOutput::Object getWeather();

    void onWeatherGet(httpsserver::HTTPRequest * request, httpsserver::HTTPResponse * response);
    

}
