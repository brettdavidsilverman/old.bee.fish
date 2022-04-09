#pragma once
#include <WiFi.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <map>
#include <bee-fish.h>

#define MAX_CONNECTIONS 10

namespace FeebeeCam {

    using namespace httpsserver;

    class SecureWebServer : public HTTPSServer {
    protected:
        static SSLCert* _cert;
        int _core;

        static SSLCert* getSSLCert() {

            if (SecureWebServer::_cert)
                return SecureWebServer::_cert;

            _cert = new SSLCert();
 
            int createCertResult =
                createSelfSignedCert(
                    *SecureWebServer::_cert,
                    KEYSIZE_2048,
                    "CN=feebee-cam.local,O=bee.fish,C=AU"
                );
    
            if (createCertResult != 0) {
                Serial.printf("Error generating certificate");
                delete SecureWebServer::_cert;
                SecureWebServer::_cert = nullptr;
                return nullptr; 
            }
                
            Serial.println("Certificate created successfully");

            return _cert;

        }
    public:

        SecureWebServer(int core) :
            HTTPSServer(getSSLCert(), 443, MAX_CONNECTIONS),
            _core(core)
        {
        }

        virtual void start() {
            HTTPSServer::start();
            
            BaseType_t xReturned;
            TaskHandle_t xHandle = NULL;

            xReturned = xTaskCreatePinnedToCore(
                SecureWebServer::webServerTask,         /* Task function. */
                "SecureWebServer",      /* String with name of task. */
                20000,               /* Stack size in bytes. */
                this,                /* Parameter passed as input of the task */
                1,                     /* Priority of the task. */
                &xHandle            /* Task handle. */,
                _core                      /* Pinned to core */
            );         
        }

        static void webServerTask( void * input ) {

            SecureWebServer* secureServer = (SecureWebServer*)input;

            for (;;) {
                // listen for incoming clients
                secureServer->loop();
                delay(10);
            }

        }

    };
    
}