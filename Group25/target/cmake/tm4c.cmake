set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(LINUX)
    set(CMAKE_SYSROOT /usr/lib/arm-none-eabi CACHE PATH "Path to sysroot")
endif()

find_program(CMAKE_C_COMPILER arm-none-eabi-gcc REQUIRED)
find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc REQUIRED)
find_program(CMAKE_CXX_COMPILER arm-none-eabi-g++ REQUIRED)
find_program(CMAKE_AS arm-none-eabi-as REQUIRED)
find_program(CMAKE_AR arm-none-eabi-ar REQUIRED)
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_OBJDUMP arm-none-eabi-objdump REQUIRED)
find_program(CMAKE_SIZE arm-none-eabi-size REQUIRED)

set(
    CMAKE_C_FLAGS
        "-mcpu=cortex-m4 \
        -mthumb \
        -specs=nano.specs \
        -specs=nosys.specs \
        -mfloat-abi=hard \
        -mfpu=fpv4-sp-d16"
    CACHE INTERNAL "C compiler flags"
)

set(SCRIPTS_DIR ${PROJECT_SOURCE_DIR}/scripts)
set(
    CMAKE_EXE_LINKER_FLAGS
    "-T\"${SCRIPTS_DIR}/link.ld\""
    CACHE INTERNAL "Linker flags"
)

function(add_tiva_executable TARGET_NAME)
    add_executable(
        ${TARGET_NAME}
        ${PROJECT_SOURCE_DIR}/startup.c
        ${PROJECT_SOURCE_DIR}/syscall-stubs.c
        ${ARGN}
    )
endfunction()
