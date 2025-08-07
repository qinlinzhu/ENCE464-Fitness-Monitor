add_tiva_executable(blinky-no-rtos blinky-no-rtos.c)
add_program_target(blinky-no-rtos)
target_link_libraries(blinky-no-rtos PRIVATE tivaware)
