idf_component_register(SRCS "main.cpp" "test.cpp" "feebee-cam-config.cpp" "app_httpd.cpp" "ssl-cert.cpp"
                    REQUIRES esp_https_server "esp32-camera" nvs_flash error "bee-fish" battery bme280 bm8563 i2c light network led arduino "web-request" website
                    INCLUDE_DIRS ".")


component_compile_options(-std=c++17)
