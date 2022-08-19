#include <feebee-cam.h>

void setup() {
  // put your setup code here, to run once:
//  FeebeeCam::initializeSerial();

  FeebeeCam::initializeLight();

  FeebeeCam::light->turnOn();
  FeebeeCam::light->flashOn();
}

unsigned long lightDelay = 50;

void loop() {
  /*
  // put your main code here, to run repeatedly:
  if (FeebeeCam::light->status()) {
    FeebeeCam::light->turnOff();
    FeebeeCam::light->flashOn();
  }
  else {
    FeebeeCam::light->flashOff();
    FeebeeCam::light->turnOn();

  }
  */

  //delay(lightDelay);
}
