 /*!
  * file scanI2C.ino
  * 
  * Connect I2C devices to I2CMultiplexer, and connect I2CMultiplexer to Arduino, then download this example
  * @n open serial monitor to check the I2C addr.
  *
  * Copyright   [DFRobot](http://www.dfrobot.com), 2016
  * Copyright   GNU Lesser General Public License
  *
  * version  V0.1
  * date  2018-5-10
  */
#include <DFRobot_I2CMultiplexer.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1011.0)

TwoWire wire(0);
DFRobot_I2CMultiplexer multiplexer(0x70, &wire);

class Weather {
private:
    TwoWire* _groveWire;
    Adafruit_BME280 _bme; // I2C

public:

    Weather(TwoWire* wire) : _groveWire(wire) {
        multiplexer.selectPort(0);
        if (!_bme.begin(0x76, _groveWire))
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

#define PIXEL_PIN    SDA  // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 16  // Number of NeoPixels

// Declare our NeoPixel strip object:

/*Create an I2CMultiplexer object, the address of I2CMultiplexer is 0x70*/


void setColor(uint8_t red = 0x00, uint8_t green = 0x00, uint8_t blue = 0x00) {
  
  multiplexer.selectPort(7);
  
  Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

  for(int i = 0; i < strip.numPixels(); ++i) {
    strip.setPixelColor(i, red, green, blue);
  }
  
  strip.show();

}

void setup(){
  //while(!Serial);
  Serial.begin(1500000);
  delay(1000);
  Serial.println("Scan ready!");
  

  wire.begin(SDA, SCL);

  /*Print I2C device of each port*/
  for (uint8_t port=0; port<8; port++) {
    Serial.print("Port:"); Serial.print(port);
    uint8_t* dev = multiplexer.scan(port);
    while(*dev){
      Serial.print("  i2c addr ");Serial.print(*dev, HEX);
      dev++;
    }
    Serial.println();
  }

  multiplexer.selectPort(7);

  setColor(0xFF, 0x00, 0x00);

}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  
  multiplexer.selectPort(7);

  Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void loop(){
  
  rainbow(10);

  Weather weather(&wire);
  weather.print(Serial);

  //delay(2000);
 
}
