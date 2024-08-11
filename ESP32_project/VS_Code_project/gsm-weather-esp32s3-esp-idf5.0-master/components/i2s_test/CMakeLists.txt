idf_component_register(
                    SRCS
                    "."
                    #"iis.c"
                    "i2s_test.c"
                    SRC_DIRS
                    "."
                    INCLUDE_DIRS
                    "."
                    REQUIRES main wm8978 wav
                    )

