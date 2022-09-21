#include <iostream>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BME280.h>
//#include <multiplexer.h>

Adafruit_BME280 bme1; // I2C
Adafruit_BME280 bme2; // I2C

void setup() {

    using namespace std;

    Serial.begin(115200);

    while (!Serial)
        delay(10);

    cout << "Starting i2c scanner" << endl;

    if (bme1.begin(0x76))
        cout << "Started BME1 0x76" << endl;
    else
        cout << "Error BME1 0x76" << endl;

    if (bme2.begin(0x77))
        cout << "Started BME2 0x77" << endl;
    else
        cout << "Error BME2 0x77" << endl;

}

void scan() {
    Serial.println("Scanning I2C port");
    //Wire.end();
    //Wire.setPins(SDA, SCL);
    //Wire.begin(SDA, SCL);

    for (int i = 0; i < 128; ++i) {
        Adafruit_I2CDevice i2c_dev = Adafruit_I2CDevice(i);
        if (i2c_dev.begin()) {
            Serial.print("Device found on address 0x");
            Serial.println(i2c_dev.address(), HEX);
        }
    }
    Serial.println("I2C scan complete");

}
/*
void scanMultiplexer() {
    Serial.println("Scanning multiplexer");
    Multiplexer multiplexer;
    for (uint8_t port=0; port<7; port++) {
        Serial.print("Port:"); 
        Serial.println(port);
        multiplexer.selectPort(port);
        scan();
    }

}
*/

void loop() {
/*    
    Adafruit_I2CDevice i2c_dev = Adafruit_I2CDevice(0x70);
    
    if (i2c_dev.begin()) {
        Serial.println("Multiplexer foound");
        scanMultiplexer();
    }
    else {
        Serial.println("Multiplexer not found");
    }
*/
    scan();
    delay(5000);
}