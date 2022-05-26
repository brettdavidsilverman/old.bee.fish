#include "memory.h"
#include "esp-memory.h"

namespace FeebeeCam {

    bool initializeMemory() {
        return psramInit();
    }
}