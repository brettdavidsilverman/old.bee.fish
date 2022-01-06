idf_component_register(SRCS "main.cpp" "test.cpp" "app_httpd.cpp"
                    REQUIRES esp_https_server "esp32-camera" nvs_flash error "bee-fish" battery bme280 bm8563 i2c light network led arduino "web-request" website "feebee-cam-config"
                    INCLUDE_DIRS ".")


component_compile_options(-std=c++17)
