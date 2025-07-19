idf_component_register(SRCS "src/bmp280.c" "src/fccu_i2c.c" "src/i2c_config.c" "src/ads1015.c"
                    INCLUDE_DIRS "inc"
                    REQUIRES driver)