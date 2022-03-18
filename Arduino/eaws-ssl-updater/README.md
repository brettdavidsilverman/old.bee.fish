# EAWS SSL-Updater

This is a basic sketch which provides a lightweight update-mechanism for ESP devices.
It can be used as an intermediate sketch if yours is too large for OTA update.
It is based on [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer), supports SSL and HTTP basic AUTH

## usage ##
1. adapt credentials.h to suit your needs, an example is provided
1. compile and flash to your ESP8266
1. point your browser to https://<your_ESP_Address>:5555/
1. select one of the features:
   * output build id
   * reboot (requires authentication)
   * update firmware (requires authentication)

## caveats ##

Here's an extract from https://github.com/me-no-dev/ESPAsyncWebServer/issues/899 where I documented caveats and workarounds to make this work.

Setting-up an SSL/TLS enabled Server using the ESPAsyncWebServer library is not yet fully documented and straight-forward. I spent a lot of time and finally got it to work. I'd like to share what I have learned, so you can make it work, too.

## My setup and basic requirements
* Sonoff-basic (ESP8266)
* Arduino IDE with debugging enabled
* Create a sketch which implements a https Server allowing upload of an updated sketch
* Install [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) and its dependency [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) manually into the Arduino IDE
* starting point is the example sketch [Setting up the Server](https://github.com/me-no-dev/ESPAsyncWebServer#setting-up-the-server)
* Adding SSL functionality according to [example needed for TLS #75](https://github.com/me-no-dev/ESPAsyncWebServer/issues/75)

## Here's an account of the caveats and how to work around them
1. Compile error "class AsyncServer has no member named beginSecure" 
    * This is ['class AsyncServer' has no member named 'beginSecure' #392](https://github.com/me-no-dev/ESPAsyncWebServer/issues/392): Solution: `#define ASYNC_TCP_SSL_ENABLED 1` **before** `#include <ESPAsyncTCP.h>`
1. Link error: `no matching function for call to AsyncClient::_recv(tcp_pcb*&, pbuf*&, int)` or similar
    * This is a bug: https://github.com/me-no-dev/ESPAsyncTCP/issues/131
    * Fix: Install the patched Version of ESPAsyncTCP: https://github.com/jeroenst/ESPAsyncTCP
1. Link error: `undefined reference to AsyncWebServer::onSslFileRequest(std::function<int (void*, char const*, unsigned char**)>, void*)` or similar
    * This is a bug: See [SSL TLS Server Secure problem #753](https://github.com/me-no-dev/ESPAsyncWebServer/issues/753) or https://gitter.im/me-no-dev/ESPAsyncWebServer?at=5d746080c59390272030908c 
    * add `#define ASYNC_TCP_SSL_ENABLED 1` to the top of `libraries/ESPAsyncTCP/src/async_config.h` (Arduino IDE)
    * This will increase code size even for projects not using SSL, but I didn't find another solution for it.
    * don't add it as a compiler flag in platform.local.txt (Arduino IDE), it won't work!
1. Now it compiles, but you may get a runtime error: `Error: Feature not supported` after calling `server.beginSecure()`
    * I tracked that one down to https://github.com/igrr/axtls-8266.
    * I thought it just didn't work because I used an unsupported cypher suite. But it wasn't that and I couldn't figure out the reason
    * but I found a configuration which works:
        * `openssl genrsa -out Key.pem 1024`
        * `openssl req -x509 -out Cert.pem -key Key.pem -new -sha256 -subj /CN=your.domain -addext "keyUsage=digitalSignature,keyEncipherment" -addext extendedKeyUsage=serverAuth`

I hope I could help somebody trying to set up an SSL-protected update-server
