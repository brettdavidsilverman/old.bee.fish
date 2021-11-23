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
        printf("%s: ", tag); \
        printf(error, ##__VA_ARGS__); \
        printf(" \"%s\"", esp_err_to_name(ret)); \
        printf("\n"); \
        while (1) \
            ; \
    } \
}

//        ERROR(tag, error, ##__VA_ARGS__);

//portTICK_PERIOD_MS = 10;
#define delay(x) vTaskDelay(x / portTICK_PERIOD_MS)
//vTaskDelay(1000 / portTICK_PERIOD_MS);