// https://github.com/fhessel/esp32_https_server
#include "ssl-cert.h"

httpsserver::SSLCert* sslCert = nullptr;

void initializeSSLCert() {

   using namespace httpsserver;
   
   Serial.print("Creating certificate...");
   
   sslCert = new SSLCert();
   
   int createCertResult = createSelfSignedCert(
      *sslCert ,
      KEYSIZE_1024,
      "CN=bee.fish,O=bee.fish,C=AU");
   
   if (createCertResult != 0) {
      Serial.println("Error");
      return; 
   }
 
   Serial.println("Ok");

}
