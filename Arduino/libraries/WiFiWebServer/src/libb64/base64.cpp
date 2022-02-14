/****************************************************************************************************************************
  base64.cpp - cpp source to a base64 encoding algorithm implementation

  For any WiFi shields, such as WiFiNINA W101, W102, W13x, or custom, such as ESP8266/ESP32-AT, Ethernet, etc

  WiFiWebServer is a library for the ESP32-based WiFi shields to run WebServer
  Forked and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Forked and modified from Arduino WiFiNINA library https://www.arduino.cc/en/Reference/WiFiNINA
  Built by Khoi Hoang https://github.com/khoih-prog/WiFiWebServer
  Licensed under MIT license

  Original author:
  @file       Esp8266WebServer.h
  @author     Ivan Grokhotkov

  Version: 1.5.4

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      12/02/2020 Initial coding for SAMD21, Nano 33 IoT, etc running WiFiNINA
  1.0.1   K Hoang      28/03/2020 Change to use new WiFiNINA_Generic library to support many more boards running WiFiNINA
  1.0.2   K Hoang      28/03/2020 Add support to SAMD51 and SAM DUE boards
  1.0.3   K Hoang      22/04/2020 Add support to nRF52 boards, such as AdaFruit Feather nRF52832, nRF52840 Express, BlueFruit Sense, 
                                  Itsy-Bitsy nRF52840 Express, Metro nRF52840 Express, NINA_B30_ublox, etc. 
  1.0.4   K Hoang      23/04/2020 Add support to MKR1000 boards using WiFi101 and custom WiFi libraries.
  1.0.5   K Hoang      21/07/2020 Fix bug not closing client and releasing socket.    
  1.0.6   K Hoang      24/07/2020 Add support to all STM32F/L/H/G/WB/MP1 and Seeeduino SAMD21/SAMD51 boards. Restructure examples 
  1.0.7   K Hoang      25/09/2020 Restore support to PROGMEM-related commands, such as sendContent_P() and send_P()
  1.1.0   K Hoang      17/11/2020 Add basic HTTP and WebSockets Client by merging ArduinoHttpClient
  1.1.1   K Hoang      27/12/2020 Suppress all possible compiler warnings
  1.2.0   K Hoang      26/05/2021 Add support to RP2040-based boards using Arduino-pico and Arduino mbed_rp2040 core
  1.3.0   K Hoang      14/08/2021 Add support to Adafruit nRF52 core v0.22.0+
  1.3.1   K Hoang      06/09/2021 Add support to ESP32/ESP8266 to use in some rare use-cases
  1.4.0   K Hoang      07/09/2021 Add support to Portenta H7
  1.4.1   K Hoang      04/10/2021 Change option for PIO `lib_compat_mode` from default `soft` to `strict`. Update Packages Patches
  1.4.2   K Hoang      12/10/2021 Update `platform.ini` and `library.json`
  1.5.0   K Hoang      19/12/2021 Reduce usage of Arduino String with std::string
  1.5.1   K Hoang      25/12/2021 Fix bug
  1.5.2   K Hoang      27/12/2021 Fix wrong http status header bug
  1.5.3   K Hoang      27/12/2021 Fix authenticate issue caused by libb64
  1.5.4   K Hoang      12/01/2022 Fix libb64 fallthrough compile warning
 ***************************************************************************************************************************************/

#include "base64.h"

/* Simple test program
#include <stdio.h>
void main()
{
    char* in = "amcewen";
    char out[22];

    b64_encode(in, 15, out, 22);
    out[21] = '\0';

    printf(out);
}
*/

int base64_encode(const unsigned char* aInput, int aInputLen, unsigned char* aOutput, int aOutputLen)
{
    // Work out if we've got enough space to encode the input
    // Every 6 bits of input becomes a byte of output
    if (aOutputLen < (aInputLen*8)/6)
    {
        // FIXME Should we return an error here, or just the length
        return (aInputLen*8)/6;
    }

    // If we get here we've got enough space to do the encoding

    const char* b64_dictionary = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (aInputLen == 3)
    {
        aOutput[0] = b64_dictionary[aInput[0] >> 2];
        aOutput[1] = b64_dictionary[(aInput[0] & 0x3)<<4|(aInput[1]>>4)];
        aOutput[2] = b64_dictionary[(aInput[1]&0x0F)<<2|(aInput[2]>>6)];
        aOutput[3] = b64_dictionary[aInput[2]&0x3F];
    }
    else if (aInputLen == 2)
    {
        aOutput[0] = b64_dictionary[aInput[0] >> 2];
        aOutput[1] = b64_dictionary[(aInput[0] & 0x3)<<4|(aInput[1]>>4)];
        aOutput[2] = b64_dictionary[(aInput[1]&0x0F)<<2];
        aOutput[3] = '=';
    }
    else if (aInputLen == 1)
    {
        aOutput[0] = b64_dictionary[aInput[0] >> 2];
        aOutput[1] = b64_dictionary[(aInput[0] & 0x3)<<4];
        aOutput[2] = '=';
        aOutput[3] = '=';
    }
    else
    {
        // Break the input into 3-byte chunks and process each of them
        int i;
        for (i = 0; i < aInputLen/3; i++)
        {
            base64_encode(&aInput[i*3], 3, &aOutput[i*4], 4);
        }
        if (aInputLen % 3 > 0)
        {
            // It doesn't fit neatly into a 3-byte chunk, so process what's left
            base64_encode(&aInput[i*3], aInputLen % 3, &aOutput[i*4], aOutputLen - (i*4));
        }
    }

    return ((aInputLen+2)/3)*4;
}

