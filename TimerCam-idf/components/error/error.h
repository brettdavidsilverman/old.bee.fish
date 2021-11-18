#pragma once
#include <stdexcept>
#include "esp_log.h"
#include "esp_err.h"

#define ERROR(tag, error, ...) { \
    ESP_LOGE(tag, error, ##__VA_ARGS__); \
    throw std::runtime_error(error); \
}

#define CHECK_ERROR(ret, tag, error, ...) { \
    if (ret != ESP_OK) {\
        ERROR(tag, error, ##__VA_ARGS__); \
    } \
}
