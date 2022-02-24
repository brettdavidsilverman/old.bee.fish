#include "multiplexer.h"

namespace FeebeeCam {
    uint8_t Multiplexer::_port = -1;
    DFRobot_I2CMultiplexer* Multiplexer::_multiplexer = nullptr;
}
