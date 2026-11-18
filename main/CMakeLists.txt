
idf_component_register(SRCS "main.c"
                      INCLUDE_DIRS "." "../includes"
                      PRIV_REQUIRES spi_flash esp_driver_gpio esp_driver_ledc esp_driver_i2c esp_wifi esp_netif nvs_flash esp_timer
                    )
