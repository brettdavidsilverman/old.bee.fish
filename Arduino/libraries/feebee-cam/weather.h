#pragma once

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <bee-fish.h>
#include "multiplexer.h"
#include "setup.h"
#include "battery.h"
#include "camera.h"

namespace FeebeeCam {

    class Weather {
    private:
        Adafruit_BME280 _bme; // I2C
        int _port;
    public:

        Weather()
        {
        }

        bool initialize() {
            
            if (!_bme.begin(0x76)) {
                Serial.println("Error setting up weather bme280 sensor");
                return false;
            }

            return true;
        }

        float temperature() {
            return _bme.readTemperature();
        }

        float pressure() {
            return _bme.readPressure() / 100.0F;
        }

        float humidity() {
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

        BeeFishJSONOutput::Object& getWeather() {

            Setup setup;

            static BeeFishJSONOutput::Object reading;

            reading.clear();


            reading["temperature"] = 
                BeeFishJSONOutput::Object {
                    {"value", _bme.readTemperature()},
                    {"unit", "°C"},
                    {"precision", 2}
                };

            reading["humidity"] = 
                BeeFishJSONOutput::Object {
                    {"value", _bme.readHumidity()},
                    {"unit", "%"},
                    {"precision", 2}
                };

            reading["pressure"] =
                BeeFishJSONOutput::Object {
                    {"value", _bme.readPressure() / 100.0F},
                    {"unit", "hPa"},
                    {"precision", 2}
                };

            reading["memory"] =
                BeeFishJSONOutput::Object {
                    {"value", ((float)ESP.getHeapSize() - (float)ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0},
                    {"unit", "% used"},
                    {"precision", 0}
                };

            if (ESP.getPsramSize() > 0) {

                reading["external mamory"] =
                    BeeFishJSONOutput::Object {
                        {"value", ((float)ESP.getPsramSize() - (float)ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0},
                        {"unit", "% used"},
                        {"precision", 0}
                    };
            }

            reading["ip address"] =
                BeeFishJSONOutput::Object {
                    {"value", WiFi.localIP().toString().c_str()}
                };

            reading["label"] =
                BeeFishJSONOutput::Object {
                    {"value", setup._label}
                };

            reading["battery"] = BeeFishJSONOutput::Object {
                {"value", bat_get_voltage()},
                {"unit", "mV"},
                {"precision", 0}
            };

            reading["frame rate"] = BeeFishJSONOutput::Object{
                {"value", getFramerate()},
                {"unit", "fps"},
                {"precision", 2}
            };
            
            return reading;
        }

    };

    bool onWeather(BeeFishWeb::WebRequest& request, WiFiClient& client);

    extern Weather weather;
    
}
