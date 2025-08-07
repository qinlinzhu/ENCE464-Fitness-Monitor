# CMake support for TivaWare in the ENCE464 fitness tracker project
# Harry Mander 2024
# TivaWare is copyright of Texas Instruments Incorporated

set(TIVAWARE_MCU TM4C123GH6PM CACHE STRING "MCU part number")

add_library(tivaware OBJECT utils/ustdlib.c)
target_link_libraries(
    tivaware
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/driverlib/gcc/libdriver.a
)
target_include_directories(tivaware PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(tivaware PUBLIC "PART_${TIVAWARE_MCU}")
