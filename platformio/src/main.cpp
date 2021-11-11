#include "bee.fish.h"

const char* TAG = "feebeecam:main.cpp";

#include "camera.h"

Camera camera;

extern "C" void app_main() {
    esp_err_t err = ESP_OK;

    err = camera.initialize();

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Main Init Failed 0x%x", err);
        while (1)
            ;
    }

    ESP_LOGV(TAG, "Main initialized");

    while (1)
        ;

}