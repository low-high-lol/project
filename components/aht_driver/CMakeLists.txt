idf_component_register(SRCS "aht_driver.c"
                       INCLUDE_DIRS "."
                       REQUIRES aht20 i2c_bus driver)