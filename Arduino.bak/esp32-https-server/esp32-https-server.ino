#include <WiFi.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
 
using namespace httpsserver;
 
const char* ssid = "Laptop";
const char* password =  "feebeegeeb3";
 
SSLCert * cert;
HTTPSServer * secureServer;
 
void setup() {
 
  Serial.begin(115200);
 
  Serial.println("Creating certificate...");
   
  cert = new SSLCert();
 
  int createCertResult = createSelfSignedCert(
    *cert,
    KEYSIZE_2048,
    "CN=feebeecam.local,O=bee.fish,C=AU");
   
  if (createCertResult != 0) {
    Serial.printf("Error generating certificate");
    return; 
  }
 
  Serial.println("Certificate created with success");
   
  secureServer = new HTTPSServer(cert);
 
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println(WiFi.localIP());
 
 
  ResourceNode * nodeRoot = new ResourceNode("/", "GET", [](HTTPRequest * req, HTTPResponse * res){
    res->println("Secure Hello World!!!");
  });
 
  secureServer->registerNode(nodeRoot);
 
  secureServer->start();
   
  if (secureServer->isRunning()) {
    Serial.println("Server ready.");
  }
}
 
void loop() {
  
  secureServer->loop();
  
  delay(10);
}
