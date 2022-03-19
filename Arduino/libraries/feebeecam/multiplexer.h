#pragma once
#include <DFRobot_I2CMultiplexer.h>

namespace FeebeeCam {

    class Multiplexer
    {
    protected:
        static uint8_t _port;
        static DFRobot_I2CMultiplexer* _multiplexer;

    public:
        Multiplexer() {
            if (_multiplexer == nullptr)
                _multiplexer = new DFRobot_I2CMultiplexer(0x70);
        }

        void selectPort(uint8_t port) {


            if (port == Multiplexer::_port)
                return;

            Serial.print("Changing port to ");
            Serial.println(port);

            _multiplexer->selectPort(port);

            Multiplexer::_port = port;
        }
        
    };
}