#include "neo-pixels.h"

Light* createLight() {
    Light* light = new SimpleLight(); //NeoPixels(16, SDA);
    light->initialize();
    return light;
}