#pragma once
#include <bee-fish.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "wifi-web-server.h"

namespace FeebeeCam {


    using namespace BeeFishBString;

    class Setup {
    public:
        BString _label;
        BString _ssid;
        BString _password;
        BString _secretHash;
        BString _beehiveVersion;
    public:
        
        Setup() {
            
            // Initialize NVS
            esp_err_t err = nvs_flash_init();
            if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
                // NVS partition was truncated and needs to be erased
                // Retry nvs_flash_init
                ESP_ERROR_CHECK(nvs_flash_erase());
                err = nvs_flash_init();
            }
            
            ESP_ERROR_CHECK( err );

            nvs_handle handle;
            
            err = nvs_open("setup", NVS_READWRITE, &handle);
            
            if (err != ESP_OK) {
                printf("Error (%d) opening NVS handle!\n", err);
                return;
            }

            getValue(handle, "label", _label);
            getValue(handle, "ssid", _ssid);
            getValue(handle, "password", _password);
            getValue(handle, "secretHash", _secretHash);
            getValue(handle, "beehiveVersion", _beehiveVersion);

            nvs_close(handle);

            nvs_flash_deinit();

            //Serial.println("Setup overriding values for debug");
            //_ssid = "Android";
            //_password = "feebeegeeb3";
        }

        void getValue(nvs_handle handle, const char* key, BString& value) {

            size_t length = 0;

            esp_err_t err = nvs_get_str(handle, key, NULL, &length);

            if (err == ESP_OK) {
                char* buffer = (char*)malloc(length);
                err = nvs_get_str(handle, key, buffer, &length);
                value = BString(buffer);
                delete buffer;
            }
        }

        bool save() {
            esp_err_t err = nvs_flash_init();
            ESP_ERROR_CHECK( err );
            nvs_handle handle;
            err = nvs_open("setup", NVS_READWRITE, &handle);
            ESP_ERROR_CHECK( err );
            nvs_set_str(handle, "label", _label.c_str());
            nvs_set_str(handle, "ssid", _ssid.c_str());
            nvs_set_str(handle, "password", _password.c_str());
            nvs_set_str(handle, "secretHash", _secretHash.c_str());
            nvs_set_str(handle, "beehiveVersion", _beehiveVersion.c_str());
            nvs_close(handle);
            nvs_flash_deinit();
            return true;
        }
    };

    bool onSetupSettings(BeeFishWeb::WebRequest& request, WiFiClient& client);

    extern Setup _setup;

}