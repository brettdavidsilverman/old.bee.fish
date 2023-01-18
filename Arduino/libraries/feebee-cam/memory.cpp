#include <iostream>
#include "memory.h"
#include "esp-memory.h"

namespace FeebeeCam {

    
    using namespace std;
    
    bool initializeMemory() {
        cerr << "Initializing memory" << std::flush;
        bool result;
        if (result = psramInit())
            cerr << " Ok" << endl;
        else
            cerr << " Fail" << endl;

        return result;
    }
}