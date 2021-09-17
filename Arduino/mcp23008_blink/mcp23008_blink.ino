#include <Arduino.h>

// Blinks an LED attached to a MCP23008 pin.
#include "Adafruit_MCP23008.h"

#define LED_PIN 0     // MCP23XXX pin LED is attached to


// uncomment appropriate line
Adafruit_MCP23008 mcp;
//Adafruit_MCP23X17 mcp;

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  Serial.println("MCP23008 Blink Test!");

  // uncomment appropriate mcp.begin
  if (!mcp.begin()) {
    Serial.println("Error.");
    while (1);
  }

  // configure pin for output
  mcp.pinMode(LED_PIN, OUTPUT);

  Serial.println("Looping...");
}

void loop() {
  mcp.digitalWrite(LED_PIN, HIGH);
  delay(5000);
  mcp.digitalWrite(LED_PIN, LOW);
  delay(5000);
}