#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1002.0)

class Weather {
private:
    TwoWire _groveWire;
    Adafruit_BME280 _bme; // I2C

public:

    Weather() : _groveWire(0) {
        _groveWire.begin(SDA, SCL, 10000000);
        if (!_bme.begin(0x76, &_groveWire))
            Serial.println("Error setting up weather bme280 sensor");
    }

    float temperature() {
        return _bme.readTemperature();
    }

    float pressure() {
        return _bme.readPressure() / 100.0F;
    }

    float altitude() {
        return _bme.readAltitude(SEALEVELPRESSURE_HPA);
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

        output.print("Approx. Altitude = ");
        output.print(altitude());
        output.println(" m");

        output.print("Humidity = ");
        output.print(humidity());
        output.println(" %");

        output.println();

    }
};