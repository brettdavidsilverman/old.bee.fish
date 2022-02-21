#pragma once

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DFRobot_I2CMultiplexer.h>
#include <battery.h>
#include <bee-fish.h>
#include <battery.h>

namespace FeebeeCam {

    void initializeWeather() {
        Serial.println("Initializing weather");
        bat_init();
    }

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
            begin();
        }

        float temperature() {
            begin();
            return _bme.readTemperature();
        }

        float pressure() {
            begin();
            return _bme.readPressure() / 100.0F;
        }

        float humidity() {
            begin();
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

            if (!begin()) {
                BeeFishJSONOutput::Object error {
                    {"error", "Error starting weather sensor"}
                };
                return error;
            }

            BString url = "Not Connected";

            if (WiFi.isConnected()) {
                url = BString("http://") + WiFi.localIP().toString().c_str() + "/";
            }
            else {
                url = "Not Connected";
            }
                
            static BeeFishJSONOutput::Object reading;
            reading = {
                {"temperature", 
                    BeeFishJSONOutput::Object {
                        {"value", _bme.readTemperature()},
                        {"unit", "°C"},
                        {"precision", 2}
                    }
                },
                {"humidity", 
                    BeeFishJSONOutput::Object {
                        {"value", _bme.readHumidity()},
                        {"unit", "%"},
                        {"precision", 2}
                    }
                },
                {"pressure", 
                    BeeFishJSONOutput::Object {
                        {"value", _bme.readPressure() / 100.0F},
                        {"unit", "hPa"},
                        {"precision", 2}
                    }
                },
                {"memory", 
                    BeeFishJSONOutput::Object {
                        {"value", ((float)ESP.getHeapSize() - (float)ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0},
                        {"unit", "% used"},
                        {"precision", 0}
                    }
                },
                {"ps ram", 
                    BeeFishJSONOutput::Object {
                        {"value", ((float)ESP.getPsramSize() - (float)ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0},
                        {"unit", "% used"},
                        {"precision", 0}
                    }
                },
                {"battery", 
                    BeeFishJSONOutput::Object {
                        {"value", bat_get_voltage()},
                        {"unit", "mV"},
                        {"precision", 0}
                    }
                },
                {"url", 
                    BeeFishJSONOutput::Object {
                        {"value", url}
                    }
                }
        //         {"time", getTime()}
            };

            return reading;
        }

        virtual bool begin() {

            _multiplexer.selectPort(_port);

            if (!_bme.begin(0x76, _wire)) {
                Serial.println("Error setting up weather bme280 sensor");
                return false;
            }

            return true;

        }

    };

}