#ifndef FEEBEE_CAM__LOCAL_TIME_H
#define fEEBEE_CAM__LOCAL_TIME_H

#include <ctime>
#include <bee-fish.h>

namespace FeebeeCam {

    using namespace BeeFishBString;
    
    bool initializeTime();
    bool isTimeInitialized();

    BString getTime();
    BString getDate();
    BString getDateTime();

}

#endif