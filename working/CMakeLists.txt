idf_component_register(SRCS main.c game.c ball.c brick.c platform.c
                       INCLUDE_DIRS .
                       PRIV_REQUIRES esp_timer config lcd cursor pin sound)
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
