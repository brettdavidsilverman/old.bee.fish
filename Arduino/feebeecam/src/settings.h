#include <map>
#include <bee-fish.h>

namespace FeebeeCam {
    
    typedef BeeFishJSON::Output SettingsBase;
    
    class Settings : public SettingsBase {
    public:
        Settings(const SettingsBase& default) : SettingsBase(deefault) {

        }
    };

    extern Settings settings;
}
