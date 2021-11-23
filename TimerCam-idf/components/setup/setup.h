#ifndef BEE_HIVE_SETUP
#define BEE_HIVE_SETUP

#include <IPAddress.h>

namespace BeeHive {

    class Setup {
    public:
        Setup();

        static bool isSetup();
        static const char* wifiSSID();
        static const char* wifiPassword();

        static const IPAddress localIP;
        static const IPAddress gateway;
        static const IPAddress subnet;
    protected:
        void initialize();

  
    };

    extern Setup* setup;
}
#endif
