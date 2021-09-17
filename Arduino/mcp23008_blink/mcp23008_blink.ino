#include <Arduino.h>
#include "battery.h"

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
  mcp.begin();

  // configure pin for output
  mcp.pinMode(LED_PIN, OUTPUT);

  bat_init();
  bat_hold_output();

  Serial.println("Looping...");
}

void loop() {
  mcp.digitalWrite(LED_PIN, HIGH);
  delay(5000);
  mcp.digitalWrite(LED_PIN, LOW);
  delay(5000);
}