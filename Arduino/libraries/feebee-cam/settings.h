#include <bee-fish.h>

namespace FeebeeCam {
    
    class Settings : public BeeFishBScript::Object
    {

    public:
        Settings() : BeeFishBScript::Object() {

        }

        bool save() {
            cerr << "Saving settings" << endl;
            BeeFishStorage storage = BeeFishStorage("/beehive/");
            if (!storage.setItem("settings", *this)) {
                std::cerr << "Couldnt save settings" << std::endl;
                return false;
            }
            return true;
        }

    };

    extern Settings settings;
    
}
