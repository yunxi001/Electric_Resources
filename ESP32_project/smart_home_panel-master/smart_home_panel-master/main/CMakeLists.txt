idf_component_register(
                    SRC_DIRS 
                        "."
                        "app"
                        "ui"
                        "ui/components"
                        "ui/fonts"
                        "ui/images"
                        "ui/screens"

                    INCLUDE_DIRS 
                        "."
                        "app"
                        "ui"
                    )

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
