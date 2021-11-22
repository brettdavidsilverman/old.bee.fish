#ifndef BEE_FISH_HIVE_SETUP
#define BEE_FISH_HIVE_SETUP

#include <IPAddress.h>

namespace BeeFishHive {

    class Setup {
    public:
        Setup();

        static bool isSetup();
        static const char* wifiSSID();
        static const char* wifiPassword();

        static const IPAddress localIP(192, 168, 4, 1);
        static const IPAddress gateway(192, 168, 4, 9);
        static const IPAddress subnet(255, 255, 255, 0);
    protected:
        void initialize();

  
    };

    extern Setup* setup;
}
#endif
