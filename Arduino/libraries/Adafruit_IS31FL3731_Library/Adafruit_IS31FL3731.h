#ifndef _ADAFRUIT_IS31FL3731_H_
#define _ADAFRUIT_IS31FL3731_H_

#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
//I2C address
#define ISSI_ADDR_DEFAULT 0x74

#define ISSI_REG_CONFIG  0x00
	#define ISSI_REG_CONFIG_PICTUREMODE 0x00
	#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
	#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18

#define ISSI_CONF_PICTUREMODE 0x00
#define ISSI_CONF_AUTOFRAMEMODE 0x04
#define ISSI_CONF_AUDIOMODE 0x08

#define ISSI_REG_PICTUREFRAME  0x01
#define ISSI_REG_AUTOPLAYTYPE  0x02
#define ISSI_REG_AUTOOPLAYDELAY  0x03


#define ISSI_REG_DISPLAYOPTION 0x05
#define ISSI_REG_AUDIOSYNC 0x06
#define ISSI_REG_FRAMESTATE 0x07
#define ISSI_REG_BREATHCONTROL1 0x08
#define ISSI_REG_BREATHCONTROL2 0x09
#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AGCCONTROL 0x0B
#define ISSI_REG_ADCRATE 0x0C

#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B    // helpfully called 'page nine'


/**************************************************************************/
/*!
    @brief Constructor for generic IS31FL3731 breakout version
*/
/**************************************************************************/
class Adafruit_IS31FL3731 : public Adafruit_GFX {
 public:
  Adafruit_IS31FL3731(uint8_t x=16, uint8_t y=9); 
  bool begin(uint8_t addr = ISSI_ADDR_DEFAULT);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void clear(void);
  void setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank = 0);
  void audioSync(bool sync);
  void setFrame(uint8_t b);
  void displayFrame(uint8_t frame);
  void setBlink(bool blinkon);
  void setBlinkTime(float secs);

 protected:
  void selectBank(uint8_t bank);
  void writeRegister8(uint8_t bank, uint8_t reg, uint8_t data);
  uint8_t readRegister8(uint8_t bank, uint8_t reg);
  uint8_t _i2caddr,  ///< The I2C address we expect to find the chip
    _frame; ///< The frame (of 8) we are currently addressing
};

/**************************************************************************/
/*!
    @brief Constructor for FeatherWing IS31FL3731 version
*/
/**************************************************************************/
class Adafruit_IS31FL3731_Wing : public Adafruit_IS31FL3731 {
 public:
  Adafruit_IS31FL3731_Wing(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
};


/**************************************************************************/
/*!
	@brief Constructor for Pimeroni RGB 5x5 IS31FL3731 version
*/
/**************************************************************************/
class Pimoroni_IS31FL3731_5x5RGB : public Adafruit_IS31FL3731 {
public:
	Pimoroni_IS31FL3731_5x5RGB(void);
	 bool begin(uint8_t addr = ISSI_ADDR_DEFAULT);
	static constexpr std::array<int, 25> redlookup{126,15,8,9,10,125,124,123,122,121,131,130,129,128,127,132,133,134,112,113,118,117,116,115,114};
static constexpr std::array<int, 25> greenlookup{ 29,95,89,90,91,28,27,26,25,41,18,17,33,32,47,19,20,21,80,81,69,68,84,83,82};
static constexpr std::array<int, 25> bluelookup{45,111,105,106,107,44,43,42,58,57,34,50,49,48,63,35,36,37,96,97,85,101,100,99,98};
	void drawPixel(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue);

};


#endif
