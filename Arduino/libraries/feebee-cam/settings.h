#pragma once
#include "FS.h"
#include "SPIFFS.h"
#include <bee-fish.h>

namespace FeebeeCam {

    class Settings : public BeeFishJSON::Object {
    private:

    public:
        Settings() {

        }

        void initialize() {

        }
    }
}