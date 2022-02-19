#pragma once

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DFRobot_I2CMultiplexer.h>
#include <battery.h>
#include <bee-fish.h>

namespace FeebeeCam {

    class Weather {
    private:
        TwoWire* _wire;
        DFRobot_I2CMultiplexer _multiplexer;
        Adafruit_BME280 _bme; // I2C
        int _port;
    public:

        Weather(TwoWire* wire, int port = 0) :
            _wire(wire),
            _multiplexer(0x70, wire),
            _port(port)
        {
            _multiplexer.selectPort(_port);
            if (!_bme.begin(0x76, _wire))
                Serial.println("Error setting up weather bme280 sensor");
        }

        float temperature() {
            _multiplexer.selectPort(_port);
            return _bme.readTemperature();
        }

        float pressure() {
            _multiplexer.selectPort(_port);
            return _bme.readPressure() / 100.0F;
        }

        float humidity() {
            _multiplexer.selectPort(_port);
            return _bme.readHumidity();
        }

        void print(Stream& output) {
            output.print("Temperature = ");
            output.print(temperature());
            output.println(" °C");

            output.print("Pressure = ");

            output.print(pressure());
            output.println(" hPa");

            output.print("Humidity = ");
            output.print(humidity());
            output.println(" %");

            output.println();

        }

        BeeFishJSONOutput::Object getWeather() {


            BeeFishJSONOutput::Object reading {
                {"temp", temperature()},
                {"humidity", humidity()},
                {"pressure", pressure()},
                {"memory", ((float)ESP.getHeapSize() - (float)ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0},
                {"psram", ((float)ESP.getPsramSize() - (float)ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0}
                //{"battery", bat_get_voltage()}
        //         {"time", getTime()}
            };

            if (WiFi.isConnected()) {
                reading["url"] = BString("http://") + WiFi.localIP().toString().c_str() + "/";
            }

            //cout << reading << endl;

            return reading;
        }

    };

}
