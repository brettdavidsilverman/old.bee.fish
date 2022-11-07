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
    const int SENSOR_ID = 0x60;

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

            cout << "Initializing weather sensor" << endl;

            _initialized = false;

            if (_bme)
                delete _bme;

            _bme = new Adafruit_BME280();

            initializeMainBoardTwoWire();

            if (!_bme->begin(_deviceAddress, mainBoardTwoWire)) {
                cout << "Error beginning bme sensor" << endl;
                return false;
            }

            if (_bme->sensorID() != SENSOR_ID) {
                cout << "Invalid sensor id" << endl;
                return false;
            }

            _initialized = true;

            return true;
        }

        void sleep() {

            cerr << "Putting sensor to sleep" << endl;

            if (!_initialized)
                initialize();

            if (_initialized) {

                mainBoardTwoWire->beginTransmission(_deviceAddress);
                mainBoardTwoWire->write((uint8_t)0xF4);
                mainBoardTwoWire->write((uint8_t)0b00000000);
                mainBoardTwoWire->endTransmission();
            }

        }


        float temperature() {
            float temp = _bme->readTemperature();
            if (!isnormal(temp) || (temp >= 100.0)) {
                initialize();
                temp = _bme->readTemperature();
            }
            if (temp >= 100.0)
                return 1.0 / 0.0; // NaN
            return temp;
        }

        float pressure() {
            float pressure = _bme->readPressure();
            if (!isnormal(pressure) || pressure <= 0) {
                initialize();
                pressure = _bme->readPressure();
            }
            if (pressure <= 0)
                return 1 / 0.0; // nan;
            return pressure;
        }

        float humidity() {
            float humidity = _bme->readHumidity();
            if (!isnormal(humidity) || (humidity >= 100.0)) {
                initialize();
                humidity = _bme->readHumidity();
            }
            if (humidity >= 100.0)
                return 1 / 0.0; // nan;
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
                        {"value", this->temperature()},
                        {"unit", "°C"},
                        {"precision", 2}
                    };

                reading["Humidity"] = 
                    BeeFishBScript::Object {
                        {"value", this->humidity()},
                        {"unit", "%"},
                        {"precision", 1}
                    };

                reading["Pressure"] =
                    BeeFishBScript::Object {
                        {"value", this->pressure() / 100.0F},
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
