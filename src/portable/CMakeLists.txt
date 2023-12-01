add_library(freertos_kernel_port STATIC
    # Posix Simulator port for GCC
    $<$<STREQUAL:${FREERTOS_PORT},GCC_POSIX>:
        Posix/port.c
        Posix/utils/wait_for_event.c>

    # Windows Simulator for Microsoft Visual C Compiler and MinGW GCC
    $<$<STREQUAL:${FREERTOS_PORT},MSVC_MINGW>:
        MSVC-MingW/port.c>
)

target_include_directories(freertos_kernel_port PUBLIC
    # Posix Simulator port for GCC
    $<$<STREQUAL:${FREERTOS_PORT},GCC_POSIX>:
        ${CMAKE_CURRENT_LIST_DIR}/Posix
        ${CMAKE_CURRENT_LIST_DIR}/Posix/utils>
    # Windows Simulator for Microsoft Visual C Compiler and MinGW GCC
    $<$<STREQUAL:${FREERTOS_PORT},MSVC_MINGW>:${CMAKE_CURRENT_LIST_DIR}/MSVC-MingW>
)

if(FREERTOS_PORT STREQUAL GCC_POSIX)
    find_package(Threads REQUIRED)
endif()

target_link_libraries(freertos_kernel_port
    PRIVATE
        freertos_kernel_include
        $<$<STREQUAL:${FREERTOS_PORT},GCC_POSIX>:Threads::Threads>
        $<$<STREQUAL:${FREERTOS_PORT},MSVC_MINGW>:winmm> # Windows library which implements timers
)
