#include "M5_BM8563.h"

BM8563::BM8563()
{
    _addr = DEVICE_ADDR;
}

BM8563::BM8563(uint8_t addr)
{
    _addr = addr;
}

void BM8563::begin()
{
    _wire = &Wire;
    _wire->begin();
    writeReg(0x00,0x00);
    writeReg(0x01,0x00);
    writeReg(0x0D,0x00);
}

void BM8563::begin(TwoWire *wire)
{
    _wire = wire;
    _wire->begin();
    writeReg(0x00,0x00);
    writeReg(0x01,0x00);
    writeReg(0x0D,0x00);
}

void BM8563::begin(TwoWire *wire, uint8_t scl, uint8_t sda, uint32_t i2c_freq)
{
    _wire = wire;
    _wire->begin(sda, scl, i2c_freq);
    writeReg(0x00,0x00);
    writeReg(0x01,0x00);
    writeReg(0x0D,0x00);
}

void BM8563::writeReg(uint8_t reg, uint8_t data)
{
    _wire->beginTransmission(0x51);
    _wire->write(reg);
    _wire->write(data);
    _wire->endTransmission();
}

uint8_t BM8563::ReadReg(uint8_t reg)
{
    _wire->beginTransmission(0x51);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(0x51, 1);
    return _wire->read();
}

void BM8563::getBm8563Time(void)
{
    _wire->beginTransmission(0x51);
    _wire->write(0x02);
    _wire->endTransmission(false);
    _wire->requestFrom(0x51, 7);
    while (_wire->available())
    {

        _trdata[0] = _wire->read();
        _trdata[1] = _wire->read();
        _trdata[2] = _wire->read();
        _trdata[3] = _wire->read();
        _trdata[4] = _wire->read();
        _trdata[5] = _wire->read();
        _trdata[6] = _wire->read();
    }

    DataMask();
    Bcd2asc();
    Str2Time();
}

void BM8563::Str2Time(void)
{

    Second = (asc[0] - 0x30) * 10 + asc[1] - 0x30;
    Minute = (asc[2] - 0x30) * 10 + asc[3] - 0x30;
    Hour = (asc[4] - 0x30) * 10 + asc[5] - 0x30;
    /*
  uint8_t Hour;
  uint8_t Week;
  uint8_t Day;
  uint8_t Month;
  uint8_t  Year;
  */
}

void BM8563::DataMask()
{

    _trdata[0] = _trdata[0] & 0x7f; //秒
    _trdata[1] = _trdata[1] & 0x7f; //分
    _trdata[2] = _trdata[2] & 0x3f; //时

    _trdata[3] = _trdata[3] & 0x3f; //日
    _trdata[4] = _trdata[4] & 0x07; //星期
    _trdata[5] = _trdata[5] & 0x1f; //月

    _trdata[6] = _trdata[6] & 0xff; //年
}
/********************************************************************
函 数 名： void Bcd2asc(void)
功 能： bcd 码转换成 asc 码，供Lcd显示用
说 明：
调 用：
入口参数：
返 回 值：无
***********************************************************************/
void BM8563::Bcd2asc(void)
{
    uint8_t i, j;
    for (j = 0, i = 0; i < 7; i++)
    {
        asc[j++] = (_trdata[i] & 0xf0) >> 4 | 0x30; /*格式为: 秒 分 时 日 月 星期 年 */
        asc[j++] = (_trdata[i] & 0x0f) | 0x30;
    }
}

uint8_t BM8563::Bcd2ToByte(uint8_t Value)
{
    uint8_t tmp = 0;
    tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
    return (tmp + (Value & (uint8_t)0x0F));
}

uint8_t BM8563::ByteToBcd2(uint8_t Value)
{
    uint8_t bcdhigh = 0;

    while (Value >= 10)
    {
        bcdhigh++;
        Value -= 10;
    }

    return ((uint8_t)(bcdhigh << 4) | Value);
}

void BM8563::getTime(rtc_time_type *RTC_TimeStruct)
{

    //if()
    uint8_t buf[3] = {0};

    _wire->beginTransmission(0x51);
    _wire->write(0x02);
    _wire->endTransmission(false);
    _wire->requestFrom(0x51, 3);

    while (_wire->available())
    {

        buf[0] = _wire->read();
        buf[1] = _wire->read();
        buf[2] = _wire->read();
    }

    RTC_TimeStruct->Seconds = Bcd2ToByte(buf[0] & 0x7f); //秒
    RTC_TimeStruct->Minutes = Bcd2ToByte(buf[1] & 0x7f); //分
    RTC_TimeStruct->Hours = Bcd2ToByte(buf[2] & 0x3f);   //时
}

void BM8563::setTime(rtc_time_type *RTC_TimeStruct)
{

    if (RTC_TimeStruct == NULL)
        return;

    _wire->beginTransmission(0x51);
    _wire->write(0x02);
    _wire->write(ByteToBcd2(RTC_TimeStruct->Seconds));
    _wire->write(ByteToBcd2(RTC_TimeStruct->Minutes));
    _wire->write(ByteToBcd2(RTC_TimeStruct->Hours));
    _wire->endTransmission();
}

void BM8563::getDate(rtc_date_type *RTC_DateStruct)
{

    uint8_t buf[4] = {0};

    _wire->beginTransmission(0x51);
    _wire->write(0x05);
    _wire->endTransmission(false);
    _wire->requestFrom(0x51, 4);

    while (_wire->available())
    {

        buf[0] = _wire->read();
        buf[1] = _wire->read();
        buf[2] = _wire->read();
        buf[3] = _wire->read();
    }

    RTC_DateStruct->Date = Bcd2ToByte(buf[0] & 0x3f);
    RTC_DateStruct->WeekDay = Bcd2ToByte(buf[1] & 0x07);
    RTC_DateStruct->Month = Bcd2ToByte(buf[2] & 0x1f);

    if (buf[2] & 0x80)
    {
        RTC_DateStruct->Year = 1900 + Bcd2ToByte(buf[3] & 0xff);
    }
    else
    {
        RTC_DateStruct->Year = 2000 + Bcd2ToByte(buf[3] & 0xff);
    }
}

void BM8563::setDate(rtc_date_type *RTC_DateStruct)
{

    if (RTC_DateStruct == NULL)
        return;
    _wire->beginTransmission(0x51);
    _wire->write(0x05);
    _wire->write(ByteToBcd2(RTC_DateStruct->Date));
    _wire->write(ByteToBcd2(RTC_DateStruct->WeekDay));

    if (RTC_DateStruct->Year < 2000)
    {

        _wire->write(ByteToBcd2(RTC_DateStruct->Month) | 0x80);
        _wire->write(ByteToBcd2((uint8_t)(RTC_DateStruct->Year % 100)));
    }
    else
    {
        /* code */
        _wire->write(ByteToBcd2(RTC_DateStruct->Month) | 0x00);
        _wire->write(ByteToBcd2((uint8_t)(RTC_DateStruct->Year % 100)));
    }

    _wire->endTransmission();
}

int BM8563::setAlarmIRQ(int afterSeconds)
{
    uint8_t reg_value = 0;
    reg_value = ReadReg(0x01);

    if (afterSeconds < 0)
    {
        reg_value &= ~(1 << 0);
        writeReg(0x01, reg_value);
        reg_value = 0x03;
        writeReg(0x0E, reg_value);
        return -1;
    }

    uint8_t type_value = 2;
    uint8_t div = 1;
    if (afterSeconds > 255)
    {
        div = 60;
        type_value = 0x83;
    }
    else
    {
        type_value = 0x82;
    }

    afterSeconds = (afterSeconds / div) & 0xFF;
    writeReg(0x0F, afterSeconds);
    writeReg(0x0E, type_value);

    reg_value |= (1 << 0);
    reg_value &= ~(1 << 7);
    writeReg(0x01, reg_value);
    return afterSeconds * div;
}

int BM8563::setAlarmIRQ(const rtc_time_type &RTC_TimeStruct)
{
    uint8_t irq_enable = false;
    uint8_t out_buf[4] = {0x80, 0x80, 0x80, 0x80};

    if (RTC_TimeStruct.Minutes >= 0)
    {
        irq_enable = true;
        out_buf[0] = ByteToBcd2(RTC_TimeStruct.Minutes) & 0x7f;
    }

    if (RTC_TimeStruct.Hours >= 0)
    {
        irq_enable = true;
        out_buf[1] = ByteToBcd2(RTC_TimeStruct.Hours) & 0x3f;
    }

    out_buf[2] = 0x00;
    out_buf[3] = 0x00;

    uint8_t reg_value = ReadReg(0x01);

    if (irq_enable)
    {
        reg_value |= (1 << 1);
    }
    else
    {
        reg_value &= ~(1 << 1);
    }

    for (int i = 0; i < 4; i++)
    {
        writeReg(0x09 + i, out_buf[i]);
    }
    writeReg(0x01, reg_value);

    return irq_enable ? 1 : 0;
}

int BM8563::setAlarmIRQ(const rtc_date_type &RTC_DateStruct, const rtc_time_type &RTC_TimeStruct)
{
    uint8_t irq_enable = false;
    uint8_t out_buf[4] = {0x80, 0x80, 0x80, 0x80};

    if (RTC_TimeStruct.Minutes >= 0)
    {
        irq_enable = true;
        out_buf[0] = ByteToBcd2(RTC_TimeStruct.Minutes) & 0x7f;
    }

    if (RTC_TimeStruct.Hours >= 0)
    {
        irq_enable = true;
        out_buf[1] = ByteToBcd2(RTC_TimeStruct.Hours) & 0x3f;
    }

    if (RTC_DateStruct.Date >= 0)
    {
        irq_enable = true;
        out_buf[2] = ByteToBcd2(RTC_DateStruct.Date) & 0x3f;
    }

    if (RTC_DateStruct.WeekDay >= 0)
    {
        irq_enable = true;
        out_buf[3] = ByteToBcd2(RTC_DateStruct.WeekDay) & 0x07;
    }

    uint8_t reg_value = ReadReg(0x01);

    if (irq_enable)
    {
        reg_value |= (1 << 1);
    }
    else
    {
        reg_value &= ~(1 << 1);
    }

    for (int i = 0; i < 4; i++)
    {
        writeReg(0x09 + i, out_buf[i]);
    }
    writeReg(0x01, reg_value);

    return irq_enable ? 1 : 0;
}

void BM8563::clearIRQ()
{
    uint8_t data = ReadReg(0x01);
    writeReg(0x01, data & 0xf3);
}
void BM8563::disableIRQ()
{
    clearIRQ();
    uint8_t data = ReadReg(0x01);
    writeReg(0x01, data & 0xfC);
}
