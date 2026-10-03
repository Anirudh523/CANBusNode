idf_component_register(SRCS "main.c" "can_tx.c" "can_rx.c" "supervisor.c"
                       PRIV_REQUIRES spi_flash driver
                       INCLUDE_DIRS "")
