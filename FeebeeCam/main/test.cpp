#include <esp_err.h>
#include <bee-fish.h>

esp_err_t beeFishTest() {
    if  (BeeFishTest::test())
        return ESP_OK;
    else
        return ESP_ERR_INVALID_RESPONSE;
}