#include <iostream>
#include <sys/time.h>
#include <sstream>
#include "rtc-bee.h"
#include "local-time.h"
#include "setup.h"
#include "config.h"
#include "driver/i2c.h"
#include <string.h>

#define BM8563_I2C_SDA 12
#define BM8563_I2C_SCL 14

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)                                              \
    do {                                                                \
        esp_err_t rc = (x);                                             \
        if (rc != ESP_OK) {                                             \
            ESP_LOGE("err", "esp_err_t = %d, line = %d", rc, __LINE__); \
        }                                                               \
    } while (0);
#define I2C_NUM 0

namespace FeebeeCam {

    I2C_BM8563* rtc = nullptr;

    bool setRTCDateTimeFromInternet();

    bool initializeRTC() {

        using namespace std;

        cerr  << "Initializing RTC" << endl;

        static bool initialized = false;
        static TwoWire* myWire;

        if (initialized) {
            myWire->end();
            delete rtc;
            delete myWire;
            rtc = nullptr;
            initialized = false;
        }

        myWire = new TwoWire(2);

        //myWire->setPins(BM8563_I2C_SDA, BM8563_I2C_SCL);
        initialized = myWire->begin(BM8563_I2C_SDA, BM8563_I2C_SCL, (uint32_t)400000);
        
        if (!initialized) {
            cerr << "Error starting Wire 2" << endl;
            return false;
        }

        rtc = new I2C_BM8563(I2C_BM8563_DEFAULT_ADDRESS, *myWire);
        rtc->begin();
        rtc->clearIRQ();

        initialized = true;

        return initialized && setRTCDateTimeFromInternet();

/*
        std::tm localTime{};

        I2C_BM8563_TimeTypeDef rtcTime;
        I2C_BM8563_DateTypeDef rtcDate;
        
        rtc->getTime(&rtcTime);
        delay(10);
        rtc->getDate(&rtcDate);
        delay(10);

        localTime.tm_year = rtcDate.year - 1900;
        localTime.tm_mon  = rtcDate.month - 1;
        localTime.tm_mday = rtcDate.date;
        localTime.tm_wday = rtcDate.weekDay;
        localTime.tm_hour = rtcTime.hours;
        localTime.tm_min  = rtcTime.minutes;
        localTime.tm_sec  = rtcTime.seconds;

        // Convert the local time to epoch
        time_t now = mktime(&localTime);

        struct timeval timeValue {};
        timeValue.tv_sec = now;
        cerr << "Set time of day from rtc" << endl;
        int ret = settimeofday(&timeValue, NULL);
        if (ret == 0) {
            // Success
            cout << getDateTime() << std::endl;
        }
        else {
            std::cerr << "settimeofday failed with return value of " << ret << std::endl;
        }

        if (rtcDate.year >= 2020) {
            return true;
        }
  */      


    }

    bool setRTCDateTimeFromInternet() {
        time_t now;
        time(&now);
        std::tm* timeInfo; // the structure tm holds time information in a more convenient way
        timeInfo = localtime(&now); // update the structure tm with the current time

        // Set RTC Date
        I2C_BM8563_DateTypeDef dateStruct;
        dateStruct.weekDay  = timeInfo->tm_wday;
        dateStruct.month    = timeInfo->tm_mon + 1;
        dateStruct.date     = timeInfo->tm_mday;
        dateStruct.year     = timeInfo->tm_year + 1900;
        rtc->setDate(&dateStruct);
        delay(10);

        I2C_BM8563_TimeTypeDef timeStruct;
        timeStruct.hours    = timeInfo->tm_hour;
        timeStruct.minutes  = timeInfo->tm_min;
        timeStruct.seconds  = timeInfo->tm_sec;
        rtc->setTime(&timeStruct);
        delay(10);

        return true;

    }

/*
    bool isRTCSetup() {

        I2C_BM8563_DateTypeDef dateStruct{};
        rtc->getDate(&dateStruct);

        return (dateStruct.year > 2021); // arbitrary year > 0

        //return FeebeeCam::_setup->_isRTCSetup;
    }
*/
    void displayNow() {

        time_t now = time(NULL);
        std::tm* localTime = localtime(&now);

        I2C_BM8563_TimeTypeDef timeStruct;
        rtc->getTime(&timeStruct);

        I2C_BM8563_DateTypeDef dateStruct;
        rtc->getDate(&dateStruct);

        std::cerr << "System clock Date: " << localTime->tm_year + 1900 << "/" << localTime->tm_mon + 1 << "/" << localTime->tm_mday << std::endl;
        std::cerr << "System clock Time: " << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec << std::endl;

        std::cerr << "RTC Date:          " << dateStruct.year << "/" << (int)dateStruct.month << "/" << (int)dateStruct.date << std::endl;
        std::cerr << "RTC clock Time:    " << (int)timeStruct.hours << ":" << (int)timeStruct.minutes << ":" << (int)timeStruct.seconds << std::endl;
    }


    void i2c_write(uint8_t slave_addr, uint8_t addr, uint8_t* buf, uint8_t len) {
        i2c_cmd_handle_t cmd;
        cmd = i2c_cmd_link_create();
        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(
            i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, 1));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, addr, 1));
        ESP_ERROR_CHECK(i2c_master_write(cmd, buf, len, 1));
        ESP_ERROR_CHECK(i2c_master_stop(cmd));
        ESP_ERROR_CHECK(
            i2c_master_cmd_begin(I2C_NUM, cmd, 1000 / portTICK_PERIOD_MS));
        i2c_cmd_link_delete(cmd);
    }

    void i2c_write_byte(uint8_t slave_addr, uint8_t addr, uint8_t data) {
        i2c_write(slave_addr, addr, &data, 1);
    }

    uint8_t i2c_read(uint8_t slave_addr, uint8_t addr, uint8_t* buf, uint8_t len) {
        i2c_cmd_handle_t cmd;
        cmd = i2c_cmd_link_create();
        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(
            i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, 1));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, addr, 1));
        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(
            i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, 1));

        if (len > 1) {
            ESP_ERROR_CHECK(i2c_master_read(cmd, buf, len - 1, I2C_MASTER_ACK));
        }

        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &buf[len - 1], I2C_MASTER_NACK));
        ESP_ERROR_CHECK(i2c_master_stop(cmd));
        ESP_ERROR_CHECK(
            i2c_master_cmd_begin(I2C_NUM, cmd, 1000 / portTICK_PERIOD_MS));
        i2c_cmd_link_delete(cmd);
        return 0;
    }

    static uint8_t byte2BCD(uint8_t data) {
        return ((data / 10) << 4) + data % 10;
    }

    static uint8_t BCD2Byte(uint8_t data) {
        return (data >> 4) * 10 + (data & 0x0f);
    }

    void bmm8563_init() {
        Wire.end();
        Wire.setPins(BM8563_I2C_SDA, BM8563_I2C_SCL);
        Wire.begin();
        i2c_write_byte(0x51, 0x00, 0x00);
        i2c_write_byte(0x51, 0x01, 0x00);
        i2c_write_byte(0x51, 0x0D, 0x00);
    }

    void bmm8563_setTime(rtc_date_t* data) {
        if (data == NULL) {
            return;
        }
        uint8_t time_buf[7];
        time_buf[0] = byte2BCD(data->second);
        time_buf[1] = byte2BCD(data->minute);
        time_buf[2] = byte2BCD(data->hour);
        time_buf[3] = byte2BCD(data->day);
        time_buf[5] = byte2BCD(data->month) | (data->year >= 2000 ? 0x00 : 0x80);
        time_buf[6] = byte2BCD(data->year % 100);
        i2c_write(0x51, 0x02, time_buf, 7);
    }

    void bmm8563_getTime(rtc_date_t* data) {
        if (data == NULL) {
            return;
        }
        uint8_t time_buf[7];
        i2c_read(0x51, 0x02, time_buf, 7);
        data->second = BCD2Byte(time_buf[0] & 0x7f);
        data->minute = BCD2Byte(time_buf[1] & 0x7f);
        data->hour   = BCD2Byte(time_buf[2] & 0x3f);
        data->day    = BCD2Byte(time_buf[3] & 0x3f);
        data->month  = BCD2Byte(time_buf[5] & 0x1f);
        data->year   = BCD2Byte(time_buf[6]) + (time_buf[5] & 0x80 ? 1900 : 2000);
    }

    // -1 :disable
    void bmm8563_setDateIRQ(int8_t minute, int8_t hour, int8_t day, int8_t week) {
        uint8_t irq_enable = false;
        uint8_t out_buf[4] = {0x80, 0x80, 0x80, 0x80};
        if (minute >= 0) {
            irq_enable = true;
            out_buf[0] = byte2BCD(minute) & 0x7f;
        }

        if (hour >= 0) {
            irq_enable = true;
            out_buf[1] = byte2BCD(hour) & 0x3f;
        }

        if (day >= 0) {
            irq_enable = true;
            out_buf[2] = byte2BCD(day) & 0x3f;
        }

        if (week >= 0) {
            irq_enable = true;
            out_buf[3] = byte2BCD(week) & 0x07;
        }

        uint8_t reg_value = 0;
        i2c_read(0x51, 0x01, &reg_value, 1);
        if (irq_enable) {
            reg_value |= (1 << 1);
        } else {
            reg_value &= ~(1 << 1);
        }

        i2c_write(0x51, 0x09, out_buf, 4);
        i2c_write(0x51, 0x01, &reg_value, 1);
    }

    // -1: disable
    int16_t bmm8563_setTimerIRQ(int16_t value) {
        uint8_t reg_value = 0;
        i2c_read(0x51, 0x01, &reg_value, 1);

        if (value < 0) {
            reg_value &= ~(1 << 0);
            i2c_write(0x51, 0x01, &reg_value, 1);
            reg_value = 0x03;
            i2c_write(0x51, 0x0E, &reg_value, 1);
            return -1;
        }

        uint8_t type_value = 2;
        uint8_t div        = 1;
        if (value > 255) {
            div        = 60;
            type_value = 0x83;
        } else {
            type_value = 0x82;
        }
        value = (value / div) & 0xFF;
        i2c_write(0x51, 0x0F, (uint8_t*)&value, 1);
        i2c_write(0x51, 0x0E, &type_value, 1);

        reg_value |= (1 << 0);
        reg_value &= ~(1 << 7);
        i2c_write(0x51, 0x01, &reg_value, 1);
        return value * div;
    }

    int16_t bmm8563_getTimerTime() {
        uint8_t value      = 0;
        uint8_t type_value = 0;
        i2c_read(0x51, 0x0f, &value, 1);
        i2c_read(0x51, 0x0e, &type_value, 1);

        if ((type_value & 0x03) == 3) {
            return value * 60;
        } else {
            return value;
        }
    }

    uint8_t bmm8563_getIRQ() {
        uint8_t data;
        i2c_read(0x51, 0x01, &data, 1);
        return data;
    }

    void bmm8563_clearIRQ() {
        uint8_t data;
        i2c_read(0x51, 0x01, &data, 1);
        i2c_write_byte(0x51, 0x01, data & 0xf3);
    }

}
