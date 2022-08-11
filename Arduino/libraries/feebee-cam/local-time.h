#ifndef FEEBEE_CAM__LOCAL_TIME_H
#define fEEBEE_CAM__LOCAL_TIME_H

#include <ctime>

namespace FeebeeCam {

    extern time_t lastTimeCameraUsed;

    bool initializeTime();
    bool isTimeInitialized();
    bool setLastTimeCameraUsed();

}

#endif