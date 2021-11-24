idf_component_register(SRCS "main.cpp" "test.cpp" "timer_cam_config.cpp" "app_httpd.cpp"
                    REQUIRES esp_https_server "esp32-camera" nvs_flash error "bee-fish" battery bme280 certificates bm8563 i2c light network setup led
                    INCLUDE_DIRS ".")