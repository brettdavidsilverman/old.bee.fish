#include <map>
#include <bee-fish.h>

namespace FeebeeCam {
    
    typedef BeeFishJSONOutput::Object SettingsBase;

    class Settings : public SettingsBase {
    public:
        Settings() : SettingsBase() {

        }

        Settings(const SettingsBase& object) : SettingsBase(object) {

        }
    };

    extern Settings settings;
}
