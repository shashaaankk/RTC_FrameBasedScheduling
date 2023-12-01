if(UNIX)
    message(STATUS " Auto-Detected Unix, setting FREERTOS_PORT=GCC_POSIX")
    set(FREERTOS_PORT GCC_POSIX CACHE STRING "FreeRTOS port name")
elseif(MINGW)
    message(STATUS " Auto-Detected MINGW, setting FREERTOS_PORT=MSVC_MINGW")
    set(FREERTOS_PORT MSVC_MINGW CACHE STRING "FreeRTOS port name")
else ()
	message(STATUS " Neither Unix nor MinGW detected. This might lead to issues")
	message(STATUS " We assume MinGW and hope for the best...")
	set(FREERTOS_PORT MSVC_MINGW CACHE STRING "FreeRTOS port name")
endif()

add_subdirectory(include)
add_subdirectory(portable)

# Heap number or absolute path to custom heap implementation provided by user
set(FREERTOS_HEAP "5" CACHE STRING "FreeRTOS heap model number. 1 .. 5. Or absolute path to custom heap source file")

add_compile_options(
    ### Gnu/Clang C Options
    $<$<COMPILE_LANG_AND_ID:C,GNU>:-fdiagnostics-color=always>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-fcolor-diagnostics>

    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wall>
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wextra>
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wpedantic>
    #$<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Werror> # treat any warning as error. Good practice, yet might lead to issues in some setups. 
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Weverything>
)

add_library(freertos_kernel STATIC
    croutine.c
    event_groups.c
    list.c
    queue.c
    stream_buffer.c
    tasks.c
    timers.c

    # If FREERTOS_HEAP is digit between 1 .. 5 - it is heap number, otherwise - it is path to custom heap source file
    $<IF:$<BOOL:$<FILTER:${FREERTOS_HEAP},EXCLUDE,^[1-5]$>>,${FREERTOS_HEAP},portable/MemMang/heap_${FREERTOS_HEAP}.c>
)

target_link_libraries(freertos_kernel
    PUBLIC
        freertos_kernel_port
        freertos_kernel_include
)
