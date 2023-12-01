set(FILES main.c main_blinky.c)

add_executable(example ${FILES})

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config
    INTERFACE
        ./
)
