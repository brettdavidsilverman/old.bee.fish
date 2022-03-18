#ifndef NEO_PIXELS
#define NEO_PIXELS

#include "light.h"
#include "led-strip.h"
#include <bee-fish.h>
#include <stdexcept>

#define LED_STRIP_RMT_INTR_NUM 19U

#ifndef SDA
#define SDA (04U)
#endif

#define TAG "NeoPixels"

class NeoPixels : public Light
{
protected:
   bool _initialized = false;

   struct led_strip_t _led_strip = {
      .rgb_led_type = RGB_LED_TYPE_WS2812,
      .led_strip_length = 16,
      .rmt_channel = RMT_CHANNEL_1,
      .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
      .gpio = (gpio_num_t)SDA,
      .showing_buf_1 = true,
      .led_strip_buf_1 = nullptr,
      .led_strip_buf_2 = nullptr
   };

public:
   uint32_t _length;
   gpio_num_t _pin;

public:
   NeoPixels(uint32_t length, int pin) : _length(length), _pin((gpio_num_t)pin)
   {
   }

   virtual ~NeoPixels()
   {
      INFO(TAG, "~NeoPixels");
      if (_initialized) {
         
         vTaskDelete(_led_strip.task_handle);
         rmt_driver_uninstall(_led_strip.rmt_channel);
         delete[] _led_strip.led_strip_buf_1;
         delete[] _led_strip.led_strip_buf_2;
         if (_led_strip.rmt_items) {}
            free(_led_strip.rmt_items);
         vSemaphoreDelete(_led_strip.access_semaphore);

      }

      INFO(TAG, "~~NeoPixels");
   }

   virtual void initialize() {
      INFO(TAG, "initialize");
      _led_strip.led_strip_length = _length;
      _led_strip.gpio = _pin;
      _led_strip.led_strip_buf_1 = new led_color_t[_length];
      _led_strip.led_strip_buf_2 = new led_color_t[_length];
      _led_strip.access_semaphore = xSemaphoreCreateBinary();
      _led_strip.task_handle = nullptr;
      _led_strip.rmt_items = nullptr;

      bool led_init_ok = led_strip_init(&_led_strip);

      if (!led_init_ok)
         ERROR(ESP_FAIL, TAG, "led_strip_init failed");

      show();

      _initialized = true;
   }

   void setPixelColor(uint32_t pixel, uint8_t red, uint8_t green, uint8_t blue)
   {
      bool led_set_ok = led_strip_set_pixel_rgb(&_led_strip, pixel, red, green, blue);
      if (!led_set_ok)
         throw std::runtime_error("led_strip_set_pixel_rgb failed");
   }

   virtual void show()
   {
      bool led_show_ok = led_strip_show(&_led_strip);
      if (!led_show_ok)
         throw std::runtime_error("led_strip_show failed");
   }

   virtual void turnOn(uint8_t red, uint8_t green, uint8_t blue) {
      INFO(TAG, "turnOn")
      for (int i = 0; i < _length; ++i)
      {
         setPixelColor(i, red, green, blue);
      }
      show();
      Light::turnOn();
   }

   virtual void turnOn()
   {
      turnOn(0xFF, 0x00, 0x00);
   }

   virtual void turnOff()
   {
      INFO(TAG, "turnOff")
      for (int i = 0; i < _length; ++i)
      {
         setPixelColor(i, 0x00, 0x00, 0x00);
      }
      show();
      Light::turnOff();
   }

   virtual void flashOn() {
      turnOn(0xFF, 0xFF, 0xFF);
   }


};


#endif