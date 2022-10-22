#ifndef FEEBEE_CAM__LOCAL_TIME_H
#define FEEBEE_CAM__LOCAL_TIME_H

#include <ctime>
#include <bee-fish.h>

namespace FeebeeCam {

    using namespace BeeFishBString;
    
    bool initializeTime();
    bool isTimeInitialized();

    BString getTime(std::time_t* now = nullptr);
    BString getDate(std::time_t* now = nullptr);
    BString getDateTime(std::time_t* now = nullptr);
    double getEpoch();

}

#endif