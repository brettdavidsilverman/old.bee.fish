#include <Adafruit_I2CDevice.h>
#include <DFRobot_I2CMultiplexer.h>


DFRobot_I2CMultiplexer I2CMultiplexer(0x70);

void setup() {

    Serial.begin(1500000);

    while (!Serial)
        delay(10);

}

void scan() {
    Serial.println("Scanning I2C port");
    TwoWire wire(0);
    wire.begin(SDA, SCL);

    for (int i = 0; i < 128; ++i) {
        Adafruit_I2CDevice i2c_dev = Adafruit_I2CDevice(i, &wire);
        if (i2c_dev.begin()) {
            Serial.print("Device found on address 0x");
            Serial.println(i2c_dev.address(), HEX);
        }
    }
    Serial.println("I2C scan complete");

}

void scanMultiplexer() {
    Serial.println("Scanning multiplexer");
    for (uint8_t port=0; port<8; port++) {
        Serial.print("Port:"); 
        Serial.println(port);
        I2CMultiplexer.selectPort(port);
        scan();
        /*
        uint8_t* dev = I2CMultiplexer.scan(port);
        while(*dev){
            Serial.print("  i2c addr ");
            Serial.print(*dev);
            dev++;
        }
        Serial.println();
        */
    }

}

void loop() {
//    scan();
    scanMultiplexer();
    delay(2000);
}
