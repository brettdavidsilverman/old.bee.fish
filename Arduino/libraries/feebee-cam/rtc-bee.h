#ifndef FEEBEE_CAM__RTC_H
#define FEEBEE_CAM__RTC_H

#include <I2C_BM8563.h>
#include <bee-fish.h>

namespace FeebeeCam {

    bool initializeRTC();

    void displayNow();

    void i2c_write_byte(uint8_t slave_addr, uint8_t addr, uint8_t data);
    void i2c_write(uint8_t slave_addr, uint8_t addr, uint8_t* buf, uint8_t len);
    uint8_t i2c_read(uint8_t slave_addr, uint8_t addr, uint8_t* buf, uint8_t len);
    typedef struct _rtc_data_t {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    } rtc_date_t;

    // Clear irq, Init
    void bmm8563_init();

    void bmm8563_setTime(rtc_date_t* data);

    void bmm8563_getTime(rtc_date_t* data);

    // -1: disable
    void bmm8563_setDateIRQ(int8_t minute, int8_t hour, int8_t day, int8_t week);

    // sec, max time is 255 * 60
    int16_t bmm8563_setTimerIRQ(int16_t value);

    // sec, get timer reg value
    int16_t bmm8563_getTimerTime();

    // get irq status
    uint8_t bmm8563_getIRQ();

    // clear irq status
    void bmm8563_clearIRQ();
}

#endif
