#include "web-request.h"
#include "web-client.h"

namespace FeebeeCam {
    
    SSLConnection* WebRequest::_connection = nullptr;

    const BString BeeFishWebRequest::_host = HOST_NAME;
    RTC_DATA_ATTR bool BeeFishWebRequest::Logon::_authenticated = false;
    RTC_DATA_ATTR long BeeFishWebRequest::Logon::_timeout;
    const BString BeeFishWebRequest::Logon::PUBLIC_SECRET = PUBLIC_SECRET_HASH;
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
