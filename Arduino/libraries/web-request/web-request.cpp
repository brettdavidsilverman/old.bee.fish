#include "web-request.h"

namespace FeebeeCam {
    BeeFishMisc::optional<BString>  WebRequest::_cookie;
    BeeFishBString::BString BeeFishWebRequest::_host = HOST;
    bool BeeFishWebRequest::_authenticated = false;
    const BeeFishBString::BString BeeFishWebRequest::Logon::PUBLIC_SECRET = "4db14a0e15e8a6a1bf1eda4dcb5e41c4db7ec311575722b88ac8b3fc0019e2f57ba2518a042f8f6292955f6187f675cee3e94564903faa069194720ff374ca55";

}
