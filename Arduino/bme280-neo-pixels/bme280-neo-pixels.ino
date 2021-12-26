/*
Connecting the BME280 Sensor:
Sensor              ->  Board
-----------------------------
Vin (Voltage In)    ->  3.3V
Gnd (Ground)        ->  Gnd
SDA (Serial Data)   ->  A4 on Uno/Pro-Mini, 20 on Mega2560/Due, 2 Leonardo/Pro-Micro
SCK (Serial Clock)  ->  A5 on Uno/Pro-Mini, 21 on Mega2560/Due, 3 Leonardo/Pro-Micro
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "bme280i2c.h"

#define PIN A5
//#define PIN A4

#define SERIAL_BAUD 115200

BME280I2C* bme;    // Default : forced mode, standby time = 1000 ms
                  // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
Adafruit_NeoPixel* strip = nullptr;
 
//bool displayNeoPixels = false;

void displayWeather();
void displayNeoPixels();
void colorfull();

long timer = millis();

void loop()
{
    displayNeoPixels();
//  colorfull();
    if ((millis() - timer ) > 10000) {
      timer = millis();
      displayWeather();
    }
//  displayNeoPixels();
}

//////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(SERIAL_BAUD);

  while(!Serial) {} // Wait

  Serial.println("Starting...");

  Serial.println("Starting wire...");
  
  //displayWeather();
    
  Serial.println("Setting up NeoPixel...");
  strip = new Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);
  strip->begin();

  Serial.println("Setting pixels color to red...");
  displayNeoPixels();
    
  Serial.println("Ok");
}


void displayNeoPixels() {
  
//  if (strip)
//    delete strip;
    
  strip->clear();   
  strip->setBrightness(255);

  uint32_t color = strip->Color(255, 0, 0); // Red
  
  for(uint16_t i=0; i<strip->numPixels(); i++) {
    strip->setPixelColor(i, color);
  }
  
  strip->show(); // Initialize all pixels to 'off'

}
void displayWeather() {
  
  strip->setBrightness(0);

  Wire.begin();

  bme = new BME280I2C();  

  while (!bme->begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(500);
  }
/*
  switch(bme->chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor! No Humidity available.");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }
  */
   float temp(NAN), hum(NAN), pres(NAN);
  
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);
  
   bme->read(pres, temp, hum, tempUnit, presUnit);
   delete bme;
   Wire.end();
  
   Serial.print("Temp: ");
   Serial.print(temp);
   Serial.print("°"+ String(tempUnit == BME280::TempUnit_Celsius ? 'C' :'F'));
   Serial.print("\t\tHumidity: ");
   Serial.print(hum);
   Serial.print("% RH");
   Serial.print("\t\tPressure: ");
   Serial.print((float)pres/100.0);
   Serial.println("Pa");

  

}
//////////////////////////////////////////////////////////////////
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip->numPixels(); i++) {
    strip->setPixelColor(i, c);
    strip->show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip->numPixels(); i++) {
      strip->setPixelColor(i, Wheel((i+j) & 255));
    }
    strip->show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip->numPixels(); i++) {
      strip->setPixelColor(i, Wheel(((i * 256 / strip->numPixels()) + j) & 255));
    }
    strip->show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip->numPixels(); i=i+3) {
        strip->setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip->show();

      delay(wait);

      for (uint16_t i=0; i < strip->numPixels(); i=i+3) {
        strip->setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip->numPixels(); i=i+3) {
        strip->setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip->show();

      delay(wait);

      for (uint16_t i=0; i < strip->numPixels(); i=i+3) {
        strip->setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void colorfull()
{

  // Some example procedures showing how to display to the pixels:
  colorWipe(strip->Color(255, 0, 0), 50); // Red
  colorWipe(strip->Color(0, 255, 0), 50); // Green
  colorWipe(strip->Color(0, 0, 255), 50); // Blue
  //colorWipe(strip->Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip->Color(127, 127, 127), 50); // White
  theaterChase(strip->Color(127, 0, 0), 50); // Red
  theaterChase(strip->Color(0, 0, 127), 50); // Blue
  
  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);

}
