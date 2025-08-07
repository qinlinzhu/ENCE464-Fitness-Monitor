add_tiva_executable(pulse-timer pulse-timer.c printf.c)
add_program_target(pulse-timer)
target_link_libraries(pulse-timer PRIVATE tivaware freertos::heap-4)
