#include "neopixels.h"

#define LED_STRIP_LENGTH 16U
#define LED_STRIP_RMT_INTR_NUM 19U

static struct led_color_t led_strip_buf_1[LED_STRIP_LENGTH];
static struct led_color_t led_strip_buf_2[LED_STRIP_LENGTH];

struct led_strip_t led_strip = {
    .rgb_led_type = RGB_LED_TYPE_WS2812,
    .led_strip_length = LED_STRIP_LENGTH,
    .rmt_channel = RMT_CHANNEL_1,
    .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
    .gpio = (gpio_num_t)SDA,
    .showing_buf_1 = true,
    .led_strip_buf_1 = led_strip_buf_1,
    .led_strip_buf_2 = led_strip_buf_2,
};

NeoPixels neoPixels(16, 04);

void setup() {
  Serial.begin(1500000);

  while (!Serial)
    ;

  Serial.println("Starting");

  for (int i = 0; i < neoPixels._length; ++i)
  {
    neoPixels.setPixelColor(i, 0x00, 0x88, 0x00);
  }
 
  neoPixels.show();
}

void loop() {
  delay(500);
  neoPixels.turnOff();
  delay(500);
  neoPixels.turnOn();
}
