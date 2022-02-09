#include "weather.h"

namespace FeebeeCam {

    Weather::Weather(int port) :
            _wire(&wire),
            _port(port)
    {
        multiplexer.selectPort(_port);
        if (!_bme.begin(0x76, _wire))
            Serial.println("Error setting up weather bme280 sensor");
    }

    float Weather::temperature() {
        return _bme.readTemperature();
    }

    float Weather::pressure() {
        return _bme.readPressure() / 100.0F;
    }

    float Weather::humidity() {
        return _bme.readHumidity();
    }

    void Weather::print(Stream& output) {
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

        Weather weather;
        float fps = framesPerSecond;    
        lastTimeFramesCounted = esp_timer_get_time();
        frameCount = 0;

        BeeFishJSONOutput::Object reading {
            {"temp", weather.temperature()},
            {"humidity", weather.humidity()},
            {"pressure", weather.pressure()},
            {"memory", ((float)ESP.getHeapSize() - (float)ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0},
            {"psram", ((float)ESP.getPsramSize() - (float)ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0},
            {"battery", bat_get_voltage()},
            {"framesPerSecond", fps}
    //         {"time", getTime()}
        };

        if (WiFi.isConnected()) {
            reading["url"] = BString("http://") + WiFi.localIP().toString().c_str() + "/";
        }

        //cout << reading << endl;

        return reading;
    }

    void onWeatherGet(httpsserver::HTTPRequest * request, httpsserver::HTTPResponse * response) {
        BeeFishJSONOutput::Object reading = getWeather();
        sendResponse(response, reading);
    }

}
