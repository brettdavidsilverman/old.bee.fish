/**
   BasicHTTPSClient.ino

    Created on: 14.10.2018

*/

#include <Arduino.h>
#include "config.h"

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#include "r3.pem.h"

#include "logon.h"

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

  WiFi.begin(ssid, password);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected");

  setClock();
}

String uri = URI;
String cookie = "";

void loop()
{
  cookie = logon("My Secret");
  Serial.println(cookie);
  delay(5000);
  return;

  Serial.print("[HTTPS] begin...\n");

  const char *headerKeys[] = {"location", "set-cookie", "cookie"};
  const size_t numberOfHeaders = 3;

  HTTPClient https;

  if (https.begin(HOST, 443, uri, rootCACertificate))
  {
    https.collectHeaders(headerKeys, numberOfHeaders);
    https.addHeader("cookie", cookie);
    Serial.print("[HTTPS] GET...\n");
    // start connection and send HTTP header
    int httpCode = https.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      // Handle redirects
      if (httpCode == HTTP_CODE_TEMPORARY_REDIRECT || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        if (https.hasHeader("location"))
        {
          String location = https.header("location");
          uri = location;
          Serial.println("Redirecting...");
          Serial.println(uri);
          return;
        }
      }

      // Check for cookies cookie,
      // If not found, logon
      if (!https.hasHeader("set-cookie"))
      {
        Serial.println("Logging in");
        cookie = logon("My Secret");
        https.end();
        return;
      }

      Serial.printf("Cookie: {%s}\n", cookie.c_str());
    }
    else
    {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  }
  else
  {
    Serial.printf("[HTTPS] Unable to connect\n");
  }

  Serial.println();
  Serial.println("Waiting 10s before the next round...");
  delay(10000);
}
