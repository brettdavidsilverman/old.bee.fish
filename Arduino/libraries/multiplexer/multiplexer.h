#pragma once
#include <DFRobot_I2CMultiplexer.h>

namespace FeebeeCam {

    class Multiplexer : public DFRobot_I2CMultiplexer
    {
    protected:
        static uint8_t _port;

    public:
        Multiplexer() : DFRobot_I2CMultiplexer(0x70) {
        }

        void selectPort(uint8_t port) {

            if (port == Multiplexer::_port)
                return;

            Serial.print("Changing port to ");
            Serial.println(port);

            DFRobot_I2CMultiplexer::selectPort(port);

            Multiplexer::_port = port;
        }
        
    };
}