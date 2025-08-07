cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(FitnessTracker LANGUAGES C)

option(
    ENABLE_PROGRAM_TARGETS
    "Enable program/debug targets (requires OpenOCD and GDB)"
    ON
)
if(ENABLE_PROGRAM_TARGETS)
    include(cmake/program.cmake)
else()
    # Create a no-op macro
    macro(add_program_target)
    endmacro()
endif()

add_subdirectory(libs/OrbitOLED EXCLUDE_FROM_ALL)
add_subdirectory(libs/tivaware EXCLUDE_FROM_ALL)
add_subdirectory(libs/freertos EXCLUDE_FROM_ALL)
add_subdirectory(src)

option(BUILD_DEMOS "Build demo applications" ON)
if(BUILD_DEMOS)
    add_subdirectory(demos)
endif()
