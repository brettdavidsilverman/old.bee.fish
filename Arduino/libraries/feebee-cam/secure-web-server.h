#pragma once
#include <WiFi.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <map>
#include <bee-fish.h>

namespace FeebeeCam {

    using namespace httpsserver;

    class SecureWebServer {
    protected:
        SSLCert * _cert;
        HTTPSServer* _secureServer;        
    public:
        SecureWebServer() {

            _cert = new SSLCert();
 
            int createCertResult =
                createSelfSignedCert(
                    *_cert,
                    KEYSIZE_2048,
                    "CN=feebee-cam.local,O=bee.fish,C=AU"
                );
    
            if (createCertResult != 0) {
                Serial.printf("Error generating certificate");
                return; 
            }
                
            Serial.println("Certificate created with success");

            _secureServer = new HTTPSServer(_cert);
    
        }

        void start() {
            _secureServer->start();
        }

        void loop() {
            _secureServer->loop();
        }

        bool isRunning() {
            return _secureServer->isRunning();
        }

        void registerNode(ResourceNode* resourceNode) {
            _secureServer->registerNode(resourceNode);
        }


    };
    
}