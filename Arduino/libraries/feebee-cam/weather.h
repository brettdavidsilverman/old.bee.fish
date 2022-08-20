#pragma once

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <bee-fish.h>
#include "multiplexer.h"
#include "setup.h"
#include "battery.h"
#include "camera.h"
#include "wifi.h"
#include "web-server.h"
#include "commands.h"
#include "rtc-bee.h"

namespace FeebeeCam {

    class Weather {
    private:
        Adafruit_BME280 _bme; // I2C
        int _port;
        bool _initialized = false;
    public:

        Weather()
        {
        }

        bool initialize() {
            
            if (!_bme.begin(0x76)) {
                return false;
            }

            _initialized = true;

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

        BeeFishBScript::Object getWeather() {

            if (!_initialized)
                initialize();

            BeeFishBScript::Object reading;

            reading["label"] =
                BeeFishBScript::Object {
                    {"value", _setup->_label}
                };

            reading["date time"] =
                BeeFishBScript::Object {
                    {"value", FeebeeCam::getDateTime()}
                };

            if (_initialized) {

                reading["temperature"] = 
                    BeeFishBScript::Object {
                        {"value", _bme.readTemperature()},
                        {"unit", "°C"},
                        {"precision", 2}
                    };

                reading["humidity"] = 
                    BeeFishBScript::Object {
                        {"value", _bme.readHumidity()},
                        {"unit", "%"},
                        {"precision", 2}
                    };

                reading["pressure"] =
                    BeeFishBScript::Object {
                        {"value", _bme.readPressure() / 100.0F},
                        {"unit", "hPa"},
                        {"precision", 2}
                    };
            }
            else {
                reading["weather"] =
                    BeeFishBScript::Object {
                        {"value", "Error initializing BME280 sensor"}
                    };
            }

            reading["memory"] =
                BeeFishBScript::Object {
                    {"value", (float)ESP.getFreeHeap() / (float)ESP.getHeapSize() * 100.0},
                    {"unit", "% free"},
                    {"precision", 2}
                };

            if (ESP.getPsramSize() > 0) {

                reading["external memory"] =
                    BeeFishBScript::Object {
                        {"value", (float)ESP.getFreePsram() / (float)ESP.getPsramSize() * 100.0},
                        {"unit", "% free"},
                        {"precision", 2}
                    };
            }


            reading["battery"] = BeeFishBScript::Object {
                {"value", bat_get_voltage()},
                {"unit", "mV"},
                {"precision", 0}
            };

            reading["frame rate"] = BeeFishBScript::Object{
                {"value", getFrameRate()},
                {"unit", "fps"},
                {"precision", 2}
            };
            
            reading["free sketch size"] = BeeFishBScript::Object {
                {"value", ESP.getFreeSketchSpace()},
                {"unit", "bytes"},
                {"precision", 0}
            };

            reading["url"] =
                BeeFishBScript::Object {
                    {"value", FeebeeCam::getURL()}
                };


            return reading;
        }

    };

    bool onWeather(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool uploadWeatherReport();
    extern Weather weather;
}
