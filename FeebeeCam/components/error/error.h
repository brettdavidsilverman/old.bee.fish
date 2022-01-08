#pragma once
#include <stdexcept>
#include <bee-fish.h>
#include "esp_log.h"
#include "esp_err.h"

#ifdef DEBUG
    inline bool filterTag(BString tag) {
      //  if (tag == "WebRequest")
      //      return true;
      //  else
            return false;
    }
#else
    #define filterTag(x) (false)
#endif

#define ERROR(tag, error, ...) { \
    ESP_LOGE(tag, error, ##__VA_ARGS__); \
    throw std::runtime_error(error); \
}

#define INFO(tag, info, ...) { \
    if (filterTag(tag)) { \
        printf("%s ", tag); \
        printf(info, ##__VA_ARGS__); \
        printf("\n"); \
    }\
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
