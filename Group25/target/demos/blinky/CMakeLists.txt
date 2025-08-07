add_tiva_executable(blinky blinky.c)
add_program_target(blinky)
target_link_libraries(blinky PRIVATE tivaware freertos::heap-4)
