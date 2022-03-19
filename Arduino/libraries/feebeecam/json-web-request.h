#ifndef FEEBEECAM__JSON_WEB_REQUEST
#define FEEBEECAM__JSON_WEB_REQUEST
#include <bee-fish.h>
#include "web-request-base.h"

namespace FeebeeCam {

    class JSONWebResponseBody :
         public BeeFishJSON::Object,
         public BeeFishWeb::ContentLength
    {

    public:
        JSONWebResponseBody() {
            
        }

    };


}1

#endif
