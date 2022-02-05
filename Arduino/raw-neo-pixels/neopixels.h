#include "led_strip.h"
#include <stdexcept>

#define LED_STRIP_RMT_INTR_NUM 19U

class NeoPixels {
protected:
  struct led_strip_t _led_strip = {
      .rgb_led_type = RGB_LED_TYPE_WS2812,
      .led_strip_length = 16,
      .rmt_channel = RMT_CHANNEL_1,
      .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
      .gpio = (gpio_num_t)SDA,
      .showing_buf_1 = true,
      .led_strip_buf_1 = nullptr,
      .led_strip_buf_2 = nullptr,
  };
public:
  uint32_t _length;
  bool _state;
public:
  NeoPixels(uint32_t length, int pin) : _length(length), _state(false) {
    _led_strip.led_strip_length = length;
    _led_strip.gpio = (gpio_num_t)pin;
    _led_strip.led_strip_buf_1 = new led_color_t[length];
    _led_strip.led_strip_buf_2 = new led_color_t[length];
    _led_strip.access_semaphore = xSemaphoreCreateBinary();
    _led_strip.quit = false;
    bool led_init_ok = led_strip_init(&_led_strip);
    if (!led_init_ok)
      throw std::runtime_error("led_strip_init failed");
  }

  ~NeoPixels() {
    _led_strip.quit = true;
    delete[] _led_strip.led_strip_buf_1;
    delete[] _led_strip.led_strip_buf_2;
    vSemaphoreDelete(_led_strip.access_semaphore);
  }

  void setPixelColor(uint32_t pixel, uint8_t red, uint8_t green, uint8_t blue) {
    bool led_set_ok = led_strip_set_pixel_rgb(&_led_strip, pixel, red, green, blue);
    if (!led_set_ok)
      throw std::runtime_error("led_strip_set_pixel_rgb failed");
  }

  virtual void show() {
    bool led_show_ok = led_strip_show(&_led_strip);
    if (!led_show_ok)
      throw std::runtime_error("led_strip_show failed");
  }

  virtual void turnOn() {
    for (int i = 0; i < _length; ++i) {
      setPixelColor(i, 0xFF, 0xFF, 0xFF);
    }
    show();
    _state = false;
  }

  virtual void turnOff() {
    for (int i = 0; i < _length; ++i) {
      setPixelColor(i, 0x00, 0x00, 0x00);
    }
    show();
    _state = true;
  }

  virtual void toggle() {
    if (_state)
      turnOff();
    else
      turnOn(); 
  }

};
