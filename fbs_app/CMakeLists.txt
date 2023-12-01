set(FILES main.c main_exercise.c)

add_executable(fbs_app ${FILES})

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config
    INTERFACE
        ./
)
