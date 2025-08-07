add_tiva_executable(fitness-tracker
    accl_manager.c
    ADC_read.c
    button_manager.c
    display_manager.c
    i2c_driver.c
    step_counter_main.c
    temp_track.c
    circBuf/circBufT.c
    circBuf/circBufF.c
    circBuf/circBufV.c
    hal/adc_hal_tiva.c
    hal/accl_hal_tiva.c
    hal/temp_hal_tiva.c
    hal/buttons_hal_tiva.c
    hal/switches_hal_tiva.c
    
)


target_include_directories(fitness-tracker PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/circBuf
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
          
)
target_include_directories(fitness-tracker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hal)
target_include_directories(fitness-tracker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/circBuf)
target_compile_options(fitness-tracker PRIVATE -Wall)
target_link_libraries(fitness-tracker PRIVATE tivaware OrbitOLED freertos::heap-4 m)
add_program_target(fitness-tracker PROGRAM_TARGET program DEBUG_TARGET debug)
 