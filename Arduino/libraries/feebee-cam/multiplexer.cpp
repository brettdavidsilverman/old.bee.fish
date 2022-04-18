#include "multiplexer.h"


namespace FeebeeCam {
    Adafruit_MCP23008 _multiplexer;

    void initializeMultiplexer() {
        _multiplexer.begin();
    }
}
