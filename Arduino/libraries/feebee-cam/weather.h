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
#include "local-time.h"
#include "status.h"
#include "commands.h"

namespace FeebeeCam {


    class Weather {
    private:
        Adafruit_BME280* _bme = nullptr; // I2C
        int _port;
        bool _initialized = false;
        const int _deviceAddress = 0x76;

    public:

        Weather()
        {
        }

        bool initialize() {
            
            if (_bme)
                delete _bme;

            _bme = new Adafruit_BME280();

            initializeMultiplexer();
                
            if (!_bme->begin(_deviceAddress, multiplexerTwoWire)) {
                  return false;
            }

            _initialized = true;

            return true;
        }

        void sleep() {

            multiplexerTwoWire->beginTransmission(_deviceAddress);
            multiplexerTwoWire->write((uint8_t)0xF4);
            multiplexerTwoWire->write((uint8_t)0b00000000);
            multiplexerTwoWire->endTransmission();
        }


        float temperature() {
            float temp = _bme->readTemperature();
            if (isnan(temp))
            {
                initialize();
                temp = _bme->readTemperature();
            }
            return temp;
        }

        float pressure() {
            float pressure = _bme->readPressure();
            if (isnan(pressure)) {
                initialize();
                pressure = _bme->readPressure();
            }
            return pressure / 100.0F;
        }

        float humidity() {
            float humidity = _bme->readHumidity();
            if (isnan(humidity)) {
                initialize();
                humidity = _bme->readHumidity();
            }
            return humidity;
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

        BeeFishBScript::Object getWeather(bool extended = false) {

            if (!_initialized)
                initialize();

            BeeFishBScript::Object reading;

            reading["Label"] =
                BeeFishBScript::Object {
                    {"value", _setup->_label}
                };

            reading["Date time"] =
                BeeFishBScript::Object {
                    {"value", FeebeeCam::getDateTime()}
                };

            if (_initialized) {

                reading["Temperature"] = 
                    BeeFishBScript::Object {
                        {"value", _bme->readTemperature()},
                        {"unit", "°C"},
                        {"precision", 2}
                    };

                reading["Humidity"] = 
                    BeeFishBScript::Object {
                        {"value", _bme->readHumidity()},
                        {"unit", "%"},
                        {"precision", 2}
                    };

                reading["Pressure"] =
                    BeeFishBScript::Object {
                        {"value", _bme->readPressure() / 100.0F},
                        {"unit", "hPa"},
                        {"precision", 2}
                    };
            }
            else {
                reading["Weather"] =
                    BeeFishBScript::Object {
                        {"value", "Error initializing BME280 sensor"}
                    };
            }

            reading["Battery"] = BeeFishBScript::Object {
                {"value", bat_get_voltage()},
                {"unit", "V"},
                {"precision", 2}
            };

            double frameRate = getFrameRate();
            if (frameRate > 0.0) {
                reading["Frame rate"] = BeeFishBScript::Object{
                    {"value", frameRate},
                    {"unit", "frames/sec"},
                    {"precision", 2}
                };
            }

            if (extended) {

                BeeFishBScript::Object extended;

                extended["Memory"] =
                    BeeFishBScript::Object {
                        {"value", (float)ESP.getFreeHeap() / (float)ESP.getHeapSize() * 100.0},
                        {"unit", "% free"},
                        {"precision", 2}
                    };

                if (ESP.getPsramSize() > 0) {

                    extended["External memory"] =
                        BeeFishBScript::Object {
                            {"value", (float)ESP.getFreePsram() / (float)ESP.getPsramSize() * 100.0},
                            {"unit", "% free"},
                            {"precision", 2}
                        };
                }

                extended["Free sketch size"] = BeeFishBScript::Object {
                    {"value", ESP.getFreeSketchSpace()},
                    {"unit", "bytes"},
                    {"precision", 0}
                };


                extended["URL"] =
                    BeeFishBScript::Object {
                        {"value", FeebeeCam::getURL()},
                        {"unit", "url"},
                        {"label", "Beehive local"}
                    };

                extended["Last image URL"] =
                    BeeFishBScript::Object {
                        {"value", FeebeeCam::status._lastImageURL},
                        {"unit", "url"},
                        {"label", "Last Image"}
                    };

                extended["Previous Weather URL"] =
                    BeeFishBScript::Object {
                        {"value", FeebeeCam::status._lastWeatherURL},
                        {"unit", "url"},
                        {"label", "Previous weather URL"}
                    };
                    
                reading["extended"] = extended;
            }

            return reading;
        }

    };

    bool onWeather(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool uploadWeatherReport();
    
    extern Weather weather;
}
