#include "light.h"

namespace FeebeeCam {   

   Light* light = nullptr;

   bool initializeLight() {

      if (light)
         delete light;

      light = new Light();

      if (light && light->initialize()) {
         light->turnOff();
         light->flashOff();
         return true;
      }
      else
         return false;

   }    


}
