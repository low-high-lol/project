idf_component_register(SRCS "display_driver.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_driver_gpio esp_driver_spi freertos)