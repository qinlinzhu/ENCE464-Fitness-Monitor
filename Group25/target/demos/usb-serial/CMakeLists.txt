add_tiva_executable(usb-serial usb-serial.c)
add_program_target(usb-serial)
target_link_libraries(usb-serial PRIVATE tivaware freertos::heap-4)
