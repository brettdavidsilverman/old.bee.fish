#include "neo-pixels.h"

Light* createLight() {
    Light* light = new NeoPixels(16, SDA);
    light->initialize();
    return light;
}