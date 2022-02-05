#include <DFRobot_I2CMultiplexer.h>


DFRobot_I2CMultiplexer::DFRobot_I2CMultiplexer(uint8_t addr, TwoWire* wire) 
  : _wire(wire)
{
  I2CMultiplexer = addr;
}

uint8_t *DFRobot_I2CMultiplexer::scan(uint8_t port){
  static uint8_t dev[127] = {0};
//  uint8_t dev;
  memset(dev,0,sizeof(dev));
  uint8_t i = 0;
  selectPort(port);

  byte error, address;
  int nDevices;
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    if (address == I2CMultiplexer){ continue;}
    _wire->beginTransmission(address);
    error = _wire->endTransmission();
    if (error == 0){
      dev[i] = address;
      i++;
    }
  }
  return dev;
}

void DFRobot_I2CMultiplexer::selectPort(uint8_t port){
  if (port > 7) return;
  _wire->beginTransmission(I2CMultiplexer);
  _wire->write(1 << port);
  _wire->endTransmission();
}

uint8_t DFRobot_I2CMultiplexer::write(uint8_t port,uint8_t addr, uint8_t reg,uint8_t* buf, uint8_t len){
  selectPort(port);

  _wire->beginTransmission(addr); // transmit to device #8
  _wire->write(reg);              // sends one byte
  uint8_t i = 0;
  for(i = 0; i < len; i++){
    _wire->write(*buf); 
    buf++;
  }
  _wire->endTransmission();    // stop transmitting
}


uint8_t DFRobot_I2CMultiplexer::read(uint8_t port,uint8_t addr,uint8_t reg,uint8_t* data, uint8_t len){
  selectPort(port);
  int i = 0;
  _wire->beginTransmission(addr); //Start transmission to device 
  _wire->write(reg); //Sends register address to read rom
  _wire->endTransmission(); //End transmission
  
  _wire->requestFrom(addr, len);//Send data n-bytes read
   while (_wire->available())   // slave may send less than requested
  {
    data[i++] = _wire->read(); // print the character
  }
//  Serial.println(result[0]);
  return i;
}