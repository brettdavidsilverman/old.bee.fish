#include <Arduino.h>
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/adc_channel.h"
#include "stdlib.h"
#include "battery.h"

#define BAT_OUTPUT_HOLD_PIN 33
#define BAT_ADC_PIN 38

#define BAT_ADC_CHANNEL ADC1_GPIO38_CHANNEL

#define BASE_VOLATAGE 3600
#define SCALE 0.661
#define ADC_FILTER_SAMPLE 64

#ifdef __cplusplus
extern "C" {
#endif

static esp_adc_cal_characteristics_t *adc_chars;

namespace FeebeeCam {
    bool initializeBattery() {
        // hold bat power, if set 0, bat power disable
        pinMode(BAT_OUTPUT_HOLD_PIN, OUTPUT);
        digitalWrite(BAT_OUTPUT_HOLD_PIN, 1);

        // init adc to read bat voltage 
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(BAT_ADC_CHANNEL, ADC_ATTEN_DB_11);
        adc_chars = (esp_adc_cal_characteristics_t *)malloc(sizeof(esp_adc_cal_characteristics_t));
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, BASE_VOLATAGE, adc_chars);
        holdBatteryOutput();
        return true;
    }

    void holdBatteryOutput() {
        digitalWrite(BAT_OUTPUT_HOLD_PIN, 1);
    }

    void disableBatteryOutput() {
        digitalWrite(BAT_OUTPUT_HOLD_PIN, 0);
    }

    float getBatteryVoltage() {
        uint32_t adc_raw_value = 0;
        for (uint16_t i = 0; i < ADC_FILTER_SAMPLE; i++) {
            adc_raw_value += adc1_get_raw(BAT_ADC_CHANNEL);
        }

        adc_raw_value = adc_raw_value / ADC_FILTER_SAMPLE;
        uint32_t voltage = (uint32_t)(esp_adc_cal_raw_to_voltage(adc_raw_value, adc_chars) / SCALE);
        return voltage / 1000.0;
    }
}

#ifdef __cplusplus
}
#endif