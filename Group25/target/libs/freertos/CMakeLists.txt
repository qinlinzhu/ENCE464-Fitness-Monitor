# CMake support for the FreeRTOS kernel in the ENCE464 fitness tracker project
# Harry Mander 2024
# FreeRTOS is copyright of Amazon.com or affiliates

set(ARCH_DIR portable/GCC/ARM_CM4F)
set(FREERTOS_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_library(freertos-core OBJECT
    croutine.c
    event_groups.c
    list.c
    queue.c
    stream_buffer.c
    tasks.c
    timers.c
    ${ARCH_DIR}/port.c
)
target_include_directories(
    freertos-core
    PUBLIC include ${FREERTOS_CONFIG_DIR} ${ARCH_DIR}
)
target_compile_definitions(freertos-core PUBLIC USE_FREERTOS=1)

foreach(impl_num RANGE 1 5)
    set(lib freertos-heap-${impl_num})
    add_library(${lib} STATIC portable/MemMang/heap_${impl_num}.c)
    target_link_libraries(${lib} PUBLIC freertos-core)
    add_library(freertos::heap-${impl_num} ALIAS ${lib})
endforeach()
