#include <iostream>
#include "two-wire.h"

namespace FeebeeCam {

    TwoWire* twoWire = nullptr;

    bool twoWireInitialized = false;

    bool initializeTwoWire(uint8_t i2cNum, uint8_t sda, uint8_t scl) {

        i2cNum = 0;

        Wire.end();
        Wire.setPins(sda, scl);
        Wire.begin();

        twoWire = &Wire;

        return true;

        using namespace std;

        cerr << "Initializing Two Wire" << endl;
        if (twoWire != nullptr) {
            cerr << "Ending two wire..." << flush;
            if (!twoWire->end()) {
                cerr << "Error" << endl;
                return false;
            }
            delete twoWire;
            twoWire = nullptr;
            cerr << "Ok" << endl;
        }

        cerr << "Creating TwoWire" << endl;

        twoWire = new TwoWire(i2cNum);

        cerr << "Setting pins..." << flush;
        
        twoWire->setPins(sda, scl);

        cerr << "Starting..." << flush;

        twoWireInitialized = twoWire->begin();

        if (!twoWireInitialized) {
            cerr << "Error starting Wire" << endl;
            return false;
        }

        cerr << "Ok" << endl;

        return true;

    }
}
