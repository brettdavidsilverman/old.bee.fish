/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                           Equipped with M5Core sample source code
*                                       配套   M5Core 示例源代码
* Visit the website for more information：https://docs.m5stack.com/en/core/gray
* 获取更多资料请访问：https://docs.m5stack.com/zh_CN/core/gray
*
* describe：NTP TIME.
* date：2021/8/3
*******************************************************************************/
#include <iostream>
#include <bee-fish.h>
#include <Arduino.h>
#include <WiFi.h> // we need wifi to get internet access
#include <M5Stack.h>
#include <WiFi.h>
#include <time.h> // time() ctime()

#define BAT_OUTPUT_HOLD_PIN 33

/* Configuration of NTP */
// choose the best fitting NTP server pool for your country
#define MY_NTP_SERVER "au.pool.ntp.org"

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define MY_TZ "AEST-10"

// Set the name and password of the wifi to be connected.   配置所连接wifi的名称和密码
const char *ssid = "laptop";
const char *password = "feebeegeeb3";

time_t now;    // this is the epoch
tm localTime; // the structure tm holds time information in a more convenient way

bool printLocalTime()
{ // Output current time.   输出当前时间

   time(&now);                              // read the current time
   localtime_r(&now, &localTime); // update the structure tm with the current time

   if (localTime.tm_year + 1900 < 2000)
   {
      return false;
   }

   std::cerr << "Year:      " << localTime.tm_year << std::endl;
   std::cerr << "Month:    " << localTime.tm_mon << std::endl;
   std::cerr << "Day:       " << localTime.tm_mday << std::endl;
   std::cerr << "Hours:    " << localTime.tm_hour << std::endl;
   std::cerr << "Minutes: " << localTime.tm_min << std::endl;
   std::cerr << "Seconds: " << localTime.tm_sec << std::endl;
   std::cerr << std::endl;

   return true;
}

#define GPIO_BIT_MASK ((1ULL<<GPIO_NUM_32) | (1ULL<<GPIO_NUM_33)) 
#define BUTTON_PIN_BITMASK 0x200000000

void setup()
{
   M5.begin();          // Init M5Core.   初始化 M5Core
   M5.Power.begin(); // Init power   初始化电源模块
   
//   M5.Power.setWakeupButton(37);
/*
   gpio_pad_select_gpio(BAT_OUTPUT_HOLD_PIN);
   gpio_set_direction((gpio_num_t)BAT_OUTPUT_HOLD_PIN, GPIO_MODE_OUTPUT);
   gpio_set_level((gpio_num_t)BAT_OUTPUT_HOLD_PIN, 1);
   std::cerr << "Battery level: " << (int)M5.Power.getBatteryLevel() << std::endl;

   if (M5.Power.isChargeFull())
      std::cerr << "Battery full" << std::endl;
   else if (M5.Power.isCharging())
      std::cerr << "Battery charging" << std::endl;
   else {
      std::cerr << "Battery not full and not charging!" << std::endl;
   }
*/
   if (M5.Power.isResetbyPowerSW()) {

      std::cerr << "Connecting to " << ssid << std::endl;

      WiFi.begin(ssid, password); // Connect wifi and return connection status.   连接wifi并返回连接状态
      while (WiFi.status() != WL_CONNECTED)
      {                   // If the wifi connection fails.   若wifi未连接成功
         delay(500); // delay 0.5s.   延迟0.5s
         std::cerr << ".";
      }
      std::cerr << std::endl << "Connected" << std::endl;
      std::cerr << "Syncing clocks" << std::endl;
      configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
      setenv("TZ", MY_TZ, 1);               // Set environment variable with your time zone
      tzset();
      while (!printLocalTime())
      {
         std::cerr << "#";
         delay(500);
      }
      std::cerr << std::endl;

      WiFi.disconnect(true); // Disconnect wifi.   断开wifi连接
      WiFi.mode(WIFI_OFF);    // Set the wifi mode to off.   设置wifi模式为关闭

      //esp_sleep_enable_ext0_wakeup((gpio_num_t)33, LOW);
      
      gpio_config_t io_conf1;
      io_conf1.intr_type = GPIO_INTR_DISABLE;
      io_conf1.mode = GPIO_MODE_INPUT;
      io_conf1.pin_bit_mask = GPIO_BIT_MASK;
      io_conf1.pull_down_en = GPIO_PULLDOWN_DISABLE;
      io_conf1.pull_up_en = GPIO_PULLUP_ENABLE;
   
      esp_err_t result = gpio_config(&io_conf1);
      if (result != ESP_OK)
      {
         std::cerr << "Error setting gpio_config 1" << std::endl;
         while (1)
            ;
      }
   
      while (digitalRead(GPIO_NUM_33) == LOW) {
         cerr << "~";
         delay(500);
      }

      result = esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, LOW);
      if (result != ESP_OK)
      {
         std::cerr << "Error setting esp_sleep_enable_ext0_wakeup" << std::endl;
         while (1)
            ;
      }

      gpio_config_t io_conf2;
      io_conf2.intr_type = GPIO_INTR_DISABLE;
      io_conf2.mode = GPIO_MODE_INPUT;
      io_conf2.pin_bit_mask = (1ULL << GPIO_NUM_37);
      io_conf2.pull_down_en = GPIO_PULLDOWN_DISABLE;
      io_conf2.pull_up_en = GPIO_PULLUP_ENABLE;
   
      result = gpio_config(&io_conf2);

      if (result != ESP_OK)
      {
         std::cerr << "Error setting gpio_config 2" << std::endl;
         while (1)
            ;
      }

      while (digitalRead(GPIO_NUM_37) == HIGH) {
         cerr << "@";
         delay(500);
      }

      result = esp_sleep_enable_ext1_wakeup((1ULL << GPIO_NUM_37), ESP_EXT1_WAKEUP_ALL_LOW);

      if (result != ESP_OK)
      {
         std::cerr << "Error setting esp_sleep_enable_ext1_wakeup" << std::endl;
         while (1)
            ;
      }

      const unsigned long long sleepFor = 10ULL * 1000ULL * 1000ULL;


      esp_sleep_enable_timer_wakeup(sleepFor);



      std::cerr << "Putting to deep sleep" << std::endl;

      esp_deep_sleep_start();
      //esp_deep_sleep(sleepFor);

      //M5.Power.deepSleep();
   }
   else if (M5.Power.isResetbyDeepsleep()) {
      std::cerr << "Woke up from deep sleep" << std::endl;
   }
   else if (M5.Power.isResetbySoftware()) {
      std::cerr << "Woke up from light sleep" << std::endl;
   }
   printLocalTime();
}

void loop()
{
   delay(1000);
   printLocalTime();
}