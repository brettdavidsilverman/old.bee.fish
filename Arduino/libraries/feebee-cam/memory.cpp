#include <iostream>
#include "memory.h"
#include "esp-memory.h"

namespace FeebeeCam {

    
    using namespace std;
    
    bool initializeMemory() {
        cerr << "Initializing memory" << endl;
        bool result;
        if (result = psramInit())
            cerr << "Memory Initialized" << endl;
        else
            cerr << "Memory initialization failed" << endl;

        return result;
    }
}