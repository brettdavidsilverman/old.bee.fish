
#include "error.h"

#include "light.h"
#include "i2c.h"

static const char* TAG = "Light";

Light* light = nullptr;

Light::Light() {

    _mcp23008.port = I2C_MASTER_NUM;
    _mcp23008.address = 0x20;
    _mcp23008.current = 0b00000000;
    _state = false;
    
    esp_err_t ret = ESP_OK;

    ret = mcp23008_init(&_mcp23008);
    CHECK_ERROR(ret, TAG, "Error initializing mcp23008 for Light");

    ret = mcp23008_write_port(&_mcp23008, 0b00000000);
    CHECK_ERROR(ret, TAG, "Error writing to mcp23008 for Light");

    ESP_LOGI(TAG,  "Light initialized");

}

void Light::turnOn() {

    esp_err_t ret = mcp23008_write_port(&_mcp23008, 0b00000001);
    CHECK_ERROR(ret, TAG, "Error initializing to mcp23008 port for Light");

    ESP_LOGI(TAG,  "Light turn on success");

    _state = true;
}


void Light::turnOff() {

    esp_err_t ret = mcp23008_write_port(&_mcp23008, 0b00000000);
    CHECK_ERROR(ret, TAG, "Error writting to mcp23008 port for Light");

    ESP_LOGI(TAG,  "Light turn off success");

    _state = false;
}

void Light::toggle() {
    if (_state)
        turnOff();
    else
        turnOn();
}

bool Light::state() {
    return _state;
}