idf_component_register(
                    SRCS
                    "."
                    "bsp_i2c.c"
                    "bsp_sd.c"
                    "bsp_ledc.c"
                    "bsp_key.c"
                    "bsp_power.c"
                    "bsp_i2c1.c"
                    "bsp_i2s.c"
                    SRC_DIRS
                    "."

                    INCLUDE_DIRS
                    "."
                    REQUIRES driver main fatfs delay
                    )

