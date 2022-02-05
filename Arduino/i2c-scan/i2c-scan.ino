#include <Adafruit_I2CDevice.h>



void setup() {

    Serial.begin(1500000);

    while (!Serial)
        delay(10);

}

void scan() {
    Serial.println("I2C address detection test");
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
void loop() {
    scan();
    delay(2000);
}
