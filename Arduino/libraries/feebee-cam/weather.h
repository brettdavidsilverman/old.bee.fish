#pragma once

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <bee-fish.h>
#include "two-wire.h"
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


    using namespace std;
    
    const int WEATHER_1 = 0x76;
    const int WEATHER_2 = 0x77;
    const int SENSOR_ID = 0x60;

    class Weather {
    private:
        Adafruit_BME280* _bme = nullptr; // I2C
        int _port;
        bool _initialized = false;
        const int _weatherNumber;
        int _deviceAddress;

    public:

        Weather(int weatherNumber) : _weatherNumber(weatherNumber)
        {
            if (weatherNumber == 1)
                _deviceAddress = WEATHER_1;
            else if (weatherNumber == 2)
                _deviceAddress = WEATHER_2;
            else {
                throw runtime_error("Invalid weather number");
            }

        }

        bool initialize() {

            _initialized = false;
            
            cerr << "Initializing weather sensor " << _weatherNumber << "..." << flush;

            if (!initializeTwoWire())
                return false;

            if (_bme)
                delete _bme;

            _bme = new Adafruit_BME280();

            if (!_bme->begin(_deviceAddress, twoWire)) {
                cerr << "Error beginning bme sensor " << _weatherNumber << endl;
                return false;
            }

            if (_bme->sensorID() != SENSOR_ID) {
                cerr << "Invalid sensor id for " << _weatherNumber << endl;
                return false;
            }

            _initialized = true;

            cerr << "OK" << endl;

            return true;
        }

        void sleep() {

            cerr << "Putting sensor to sleep " << _weatherNumber << endl;

            twoWire->beginTransmission(_deviceAddress);
            twoWire->write((uint8_t)0xF4);
            twoWire->write((uint8_t)0b00000000);
            twoWire->endTransmission();

        }


        float temperature() {
            float temp = _bme->readTemperature();
            return temp;
        }

        float pressure() {
            float pressure = _bme->readPressure();
            return pressure;
        }

        float humidity() {
            float humidity = _bme->readHumidity();
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

            BeeFishBScript::Object reading;

            if (_initialized) {
                
                reading["Temperature"] =    
                    BeeFishBScript::Object {
                        {"value", this->temperature()},
                        {"unit", "deg C"},
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
    bool uploadWeatherReport(BeeFishId::Id& id);
    void initializeWeather();

    extern FeebeeCam::Weather weather1;
    extern FeebeeCam::Weather weather2;

    
}
