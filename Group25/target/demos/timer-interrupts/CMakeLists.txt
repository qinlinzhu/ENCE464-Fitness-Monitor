add_tiva_executable(timer-interrupts timer-interrupts.c)
add_program_target(timer-interrupts)
target_link_libraries(timer-interrupts PRIVATE tivaware freertos::heap-4)
