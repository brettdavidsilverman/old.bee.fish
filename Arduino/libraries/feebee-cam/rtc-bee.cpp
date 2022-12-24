#include <iostream>
#include <sys/time.h>
#include <sstream>
#include <Wire.h>
#include "rtc-bee.h"
#include "local-time.h"
#include "setup.h"
#include "config.h"
#include "driver/i2c.h"
#include "two-wire.h"
#include <string.h>

#define BM8563_I2C_SDA 12
#define BM8563_I2C_SCL 14
#define I2C_MASTER_FREQ_HZ 0
#define I2C_NUM 0

#undef ESP_ERROR_CHECK

#define ESP_ERROR_CHECK(x)                                              \
    do {                                                                \
        esp_err_t rc = (x);                                             \
        if (rc != ESP_OK) {                                             \
            ESP_LOGE("err", "esp_err_t = %d, line = %d", rc, __LINE__); \
        }                                                               \
    } while (0);


namespace FeebeeCam {

    bool initializeRTC() {

        return false;
        
        using namespace std;

        bmm8563_init();

//        bmm8563_clearIRQ();

        rtc_date_t date;
        bmm8563_getTime(&date);

        if (date.year < 2022) // Arbitrary year
        {
            std::cerr << "Set bmm8563 from internal clock..." << flush;

            std::time_t now;        
            std::time(&now);
            std::tm* timeInfo;
            timeInfo = localtime(&now); // update the structure tm with the current time

            rtc_date_t date;
            date.year   = timeInfo->tm_year + 1900;
            date.month  = timeInfo->tm_mon + 1;
            date.day    = timeInfo->tm_mday;
            date.hour   = timeInfo->tm_hour;
            date.minute = timeInfo->tm_min;
            date.second = timeInfo->tm_sec;
            bmm8563_setTime(&date);
        }
        else {
            std::cerr << "Set internal clock from bmm8563 rtc..." << flush;

            std::tm localTime{};
            localTime.tm_year  = date.year - 1900;
            localTime.tm_mon   = date.month - 1;
            localTime.tm_mday  = date.day;
            localTime.tm_hour  = date.hour;
            localTime.tm_min   = date.minute;
            localTime.tm_sec   = date.second;
            localTime.tm_wday  = 0;
            localTime.tm_yday  = 0;
            localTime.tm_isdst = 0;

            // Convert the local time to epoch
            time_t now = mktime(&localTime);

            struct timeval timeValue {};
            memset(&timeValue, 0, sizeof(timeValue));
            timeValue.tv_sec = now;
            int ret = settimeofday(&timeValue, NULL);

        }

        bmm8563_getTime(&date);

        if (date.year >= 2022) {
            if (!FeebeeCam::_setup->_isRTCSetup) {
                FeebeeCam::_setup->_isRTCSetup = true;
                FeebeeCam::_setup->save();
            }
            cerr << FeebeeCam::getDateTime() << endl;
        }
        else {
            cerr << "Error with RTC" << endl;
            return false;
        }

        cerr << "Ok" << endl;

        return true;

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

    bool bmm8563_init() {

        using namespace std;

        cerr  << "Initializing RTC..." << flush;

        if (!FeebeeCam::initializeTwoWire(I2C_NUM, BM8563_I2C_SDA, BM8563_I2C_SCL))
            return false;

//        Wire.end();
//        Wire.setPins(BM8563_I2C_SDA, BM8563_I2C_SCL);
//        Wire.begin();

        i2c_write_byte(0x51, 0x00, 0x00);
        i2c_write_byte(0x51, 0x01, 0x00);
        i2c_write_byte(0x51, 0x0D, 0x00);

        cerr << "Ok" << endl;

        return true;
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
