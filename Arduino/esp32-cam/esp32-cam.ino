#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>
#include "m5timer.h"
#include "light.h"

const char* WIFI_SSID = "Bee";
const char* WIFI_PASS = "feebeegeeb3";

WebServer server(80);

static auto loRes = esp32cam::Resolution::find(320, 240);
static auto medRes = esp32cam::Resolution::find(800, 600);
static auto hiRes = esp32cam::Resolution::find(1280, 1024);


void
handleBmp()
{
  if (!esp32cam::Camera.changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }

  light.turnOn();
  
  auto frame = esp32cam::capture();
  
  light.turnOff();

  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  if (!frame->toBmp()) {
    Serial.println("CONVERT FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.printf("CONVERT OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/bmp");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void
serveJpg()
{
  light.turnOn();

  auto frame = esp32cam::capture();
  
  light.turnOff();
  
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void
handleJpgLo()
{
  if (!esp32cam::Camera.changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  serveJpg();
}

void
handleJpgHi()
{
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}

void
handleJpg()
{
  if (!esp32cam::Camera.changeResolution(medRes)) {
    Serial.println("SET-MED-RES FAIL");
  }
  serveJpg();
}

void
handleMjpeg()
{
  if (!esp32cam::Camera.changeResolution(medRes)) {
    Serial.println("SET-MED-RES FAIL");
  }

  Serial.println("STREAM BEGIN");
  WiFiClient client = server.client();
  auto startTime = millis();

  light.turnOn();

  int res = esp32cam::Camera.streamMjpeg(client);
  
  light.turnOff();
  
  if (res <= 0) {
    Serial.printf("STREAM ERROR %d\n", res);
    return;
  }
  auto duration = millis() - startTime;
  Serial.printf("STREAM END %dfrm %0.2ffps\n", res, 1000.0 * res / duration);
}

void
setup()
{
  Serial.begin(115200);
  Serial.println();

  {
    using namespace esp32cam;

    Config cfg;
    cfg.setPins(M5Timer);
    //cfg.setPins(pins::M5Camera);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(90);
    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /image.bmp");
  Serial.println("  /image-lo");
  Serial.println("  /image-hi");
  Serial.println("  /stream");

  server.on("/image.bmp", handleBmp);
  server.on("/image-lo", handleJpgLo);
  server.on("/image-hi", handleJpgHi);
  server.on("/image", handleJpg);
  server.on("/stream", handleMjpeg);

  server.begin();
}

void
loop()
{
  server.handleClient();
}
