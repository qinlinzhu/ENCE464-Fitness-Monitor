add_tiva_executable(pwm-oled pwm-oled.c buttons.c)
add_program_target(pwm-oled)
target_link_libraries(pwm-oled PRIVATE tivaware freertos::heap-4 OrbitOLED)
