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


    const int WEATHER_1 = 0x76;
    const int WEATHER_2 = 0x77;

    class Weather {
    private:
        Adafruit_BME280* _bme = nullptr; // I2C
        int _port;
        bool _initialized = false;
        const int _deviceAddress;

    public:

        Weather(int deviceAddress) : _deviceAddress(deviceAddress)
        {
        }

        bool initialize() {
            
            _initialized = false;

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

            if (!_initialized)
                initialize();

            if (_initialized) {

                multiplexerTwoWire->beginTransmission(_deviceAddress);
                multiplexerTwoWire->write((uint8_t)0xF4);
                multiplexerTwoWire->write((uint8_t)0b00000000);
                multiplexerTwoWire->endTransmission();
            }

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

        virtual BeeFishBScript::Object getWeather() {

            if (!_initialized)
                initialize();

            BeeFishBScript::Object reading;

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

            return reading;

        }

        static BeeFishBScript::Object getWeather(bool extended);

    };

    bool onWeather(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool uploadWeatherReport();
    
    extern Weather weather1;
    extern Weather weather2;
}
