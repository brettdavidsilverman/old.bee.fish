#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <ssl-cert.h>

int    HTTP_PORT   = 443;
String HTTP_METHOD = "GET"; // or "POST"
char   HOST_NAME[] = "google.com"; // hostname of web server:
String PATH_NAME   = "/";

void setup() {

  Serial.begin(1500000);

  while (!Serial)
    delay(10);

  initializeSSLCert();

  Serial.print("Connecting to WiFi...");
  
  WiFi.begin("Laptop", "feebeegeeb3");
  while (!WiFi.isConnected()) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected");
  
  
  WiFiClient client;
  
  //client.setCertificate((char*)(sslCert->getCertData()));
  //client.setPrivateKey((char*)(sslCert->getPKData()));

  if(client.connectSSL(HOST_NAME, HTTP_PORT)) {
    Serial.println("Connected to server");
  }
  else {
    Serial.println("connection failed");
  }

  client.println(HTTP_METHOD + " " + PATH_NAME + " HTTP/1.1");
  client.println("Host: " + String(HOST_NAME));
  client.println("Connection: close");
  client.println(); // end HTTP request header
  
  while (client.connected()) {
    
    while(client.available()) {
      // read an incoming byte from the server and print them to serial monitor:
      char c = client.read();
      Serial.print(c);
    }

    delay(10);
  }
  
  if(!client.connected())
  {
    // if the server's disconnected, stop the client:
    Serial.println("disconnected");
    client.stop();
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
