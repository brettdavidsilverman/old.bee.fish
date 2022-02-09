/**
   BasicHTTPSClient.ino

    Created on: 14.10.2018

*/

#include <Arduino.h>
#include "config.h"

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#include "logon.h"

void connectToWiFi();

String host = HOST;
String uri = URI;
String sessionId;
String secret = "My Secret";

// Not sure if WiFiClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...
void setClock()
{
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

void setup()
{

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  connectToWiFi();
  setClock();
}

NullStream stream;

void loop() {
  
  // check WiFi connection  
  if ((WiFi.status() != WL_CONNECTED)) {
    // Connect to wifi
    connectToWiFi();
    // Reset the https client
    delete https;
    https = nullptr;
  }

  httpRequest(host, uri, secret, stream);  

}


bool httpRequest(String host, String uri, String secret, Stream& stream)
{

  if (https == nullptr) {
      https = new HTTPClient();
      https->setReuse(true);
  }
  
  const char *headerKeys[] = {"location", "set-cookie", "cookie"};
  const size_t numberOfHeaders = 3;

  Serial.print("Get https://");
  Serial.print(host);
  Serial.print(uri);
  Serial.print("...");

  if (!https->begin(host, 443, uri, rootCACertificate)) {
    Serial.printf("[HTTPS] Unable to connect\n");
    delete https;
    https = nullptr;
    return false;
  }

  https->collectHeaders(headerKeys, numberOfHeaders);
  
  if (secret.length() && !sessionId.length()) {
    sessionId = logon(host, secret);
  }

  if (sessionId.length()) {
    String cookie = "sessionId=" + sessionId + ";";
    https->addHeader("cookie", cookie);
  }

  https->addHeader("connection", "keep-alive");

  // start connection and send HTTP header
  int httpCode = https->GET();

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    // Handle redirects
    if (httpCode == HTTP_CODE_TEMPORARY_REDIRECT || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
    {
      if (https->hasHeader("location"))
      {
        String location = https->header("location");
        Serial.println("redirecting");
        return httpRequest(host, location, secret, stream);
      }
    }

    if (httpCode == HTTP_CODE_UNAUTHORIZED) {
      Serial.println("logging in");
      sessionId = logon(host, secret);
      return httpRequest(host, uri, secret, stream);
    }
    
    
    if (httpCode == HTTP_CODE_OK && https->hasHeader("set-cookie")) {
      String cookie = https->header("set-cookie");
      sessionId = getSessionIdFromCookie(cookie);
    }

    if (httpCode == HTTP_CODE_OK) {
      Serial.println("OK");
      https->writeToStream(&stream);

      return true;
    }

  }
  else
  {
    Serial.printf("failed, error: %i:%s\n", httpCode, https->errorToString(httpCode).c_str());
    https->end();
    delete https;
    https = nullptr;  
    return false;
  }

}

void connectToWiFi() {

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  WiFi.begin(ssid, password);

  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected");

}