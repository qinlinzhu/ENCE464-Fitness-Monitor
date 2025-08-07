# CMake support for OrbitOLED library in the ENCE464 fitness tracker project
# Harry Mander 2024

add_library(
    OrbitOLED
    STATIC
    OrbitOLEDInterface.c
    lib_OrbitOled/ChrFont0.c
    lib_OrbitOled/delay.c
    lib_OrbitOled/FillPat.c
    lib_OrbitOled/OrbitOled.c
    lib_OrbitOled/OrbitOledChar.c
    lib_OrbitOled/OrbitOledGrph.c
)
target_include_directories(OrbitOLED PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(OrbitOLED PUBLIC tivaware)
