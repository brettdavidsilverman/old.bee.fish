#ifndef BEE_FISH_WEB__WEB_RESPONSE_H
#define BEE_FISH_WEB__WEB_RESPONSE_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "../json/json.h"
#include "../database/path.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;
      
namespace BeeFishWeb {

   class WebResponse : public And {
   public:
      WebResponse() {

      }
   };

};

#endif
