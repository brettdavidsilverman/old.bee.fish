#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "camera_pins.h"
#include "battery.h"
#include "led.h"
#include <bee-rtc.h>
#include <light.h>

void led_breathe_test() {
  for (int16_t i = 0; i < 1024; i++) {
    led_brightness(i);
    vTaskDelay(pdMS_TO_TICKS(1));
  }

  for (int16_t i = 1023; i >= 0; i--) {
    led_brightness(i);
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void setup() {

  // will hold bat output
  bat_init();

  Serial.begin(1500000);

  std::cerr << "Wake up example" << std::endl;

  FeebeeCam::initializeSetup();
  FeebeeCam::initializeRTC();

  rtc_time_type rtcTime;
  rtc_date_type rtcDate;

  FeebeeCam::RTC.getTime(&rtcTime);
  FeebeeCam::RTC.getDate(&rtcDate);
  std::cerr << "RTC date   " << (int)rtcDate.Year << "/" << (int)rtcDate.Month << "/" << (int)rtcDate.Date << std::endl;
  std::cerr << "RTC time   " << (int)rtcTime.Hours << ":" << (int)rtcTime.Minutes << ":" << (int)rtcTime.Seconds << std::endl;
  Serial.printf("volt: %d mv\r\n", bat_get_voltage());

  FeebeeCam::initializeLight();

  FeebeeCam::light->turnOn();
  delay(10);
  FeebeeCam::light->turnOff();

  //led_init(CAMERA_LED_GPIO);

  // 5 sec later will wake up
  FeebeeCam::RTC.setAlarmIRQ(5);
  
  // disable bat output, will wake up after 5 sec, Sleep current is 1~2μA
  bat_disable_output();

  // if usb not connect, will not in here;
  esp_deep_sleep(1000L * 1000L * 10L);
  esp_deep_sleep_start();

}

void loop() {


}
