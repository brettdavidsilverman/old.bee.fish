#include "web-request.h"

namespace FeebeeCam {
    
    SSLConnection* WebRequest::_connection = nullptr;

    const BString BeeFishWebRequest::_host = HOST_NAME;
    RTC_DATA_ATTR bool BeeFishWebRequest::_authenticated = false;
    const BString BeeFishWebRequest::Logon::PUBLIC_SECRET = "4db14a0e15e8a6a1bf1eda4dcb5e41c4db7ec311575722b88ac8b3fc0019e2f57ba2518a042f8f6292955f6187f675cee3e94564903faa069194720ff374ca55";
    RTC_DATA_ATTR char BeeFishWebRequest::Logon::_lastSecret[512] = "";
    RTC_DATA_ATTR char WebRequest::_cookieData[512] = "";


}

void mbedtls_net_close_bds( mbedtls_net_context *ctx )
{
    if( ctx->fd == -1 )
        return;

    close( ctx->fd );

    ctx->fd = -1;
}
