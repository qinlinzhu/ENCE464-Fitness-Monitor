find_program(OPENOCD openocd REQUIRED)
message(STATUS "Found OpenOCD: ${OPENOCD}")

find_program(
    GDB
    NAMES gdb-multiarch arm-none-eabi-gdb
    REQUIRED
)
message(STATUS "Found GDB: ${GDB}")

function(_add_program_target EXECUTABLE_TARGET TARGET_NAME GDB_SCRIPT COMMENT)
    message(
        VERBOSE
        "Adding target '${TARGET_NAME}' for ${EXECUTABLE_TARGET}..."
    )
    set(EXE_PATH $<TARGET_FILE:${EXECUTABLE_TARGET}>)
    add_custom_target(
        ${TARGET_NAME}
        DEPENDS ${EXE_PATH}
        COMMAND
            ${GDB} ${ARGN}
            -x "${SCRIPTS_DIR}/openocd.gdb"
            -x "${SCRIPTS_DIR}/${GDB_SCRIPT}.gdb"
            "${EXE_PATH}"
        COMMENT ${COMMENT}
    )
endfunction()

function(add_program_target EXECUTABLE_TARGET)
    cmake_parse_arguments(
        PARSE_ARGV 1 ARGS "" "PROGRAM_TARGET;DEBUG_TARGET" ""
    )
    if(NOT TARGET ${EXECUTABLE_TARGET})
        message(SEND_ERROR "No target name '${EXECUTABLE_TARGET}'")
    endif()

    if(ARGS_PROGRAM_TARGET)
        set(program_target ${ARGS_PROGRAM_TARGET})
    else()
        set(program_target program-${EXECUTABLE_TARGET})
    endif()
    _add_program_target(
        ${EXECUTABLE_TARGET}
        ${program_target}
        program
        "Programming target with ${EXECUTABLE_TARGET}..."
        -batch
    )

    if(ARGS_DEBUG_TARGET)
        set(debug_target ${ARGS_DEBUG_TARGET})
    else()
        set(debug_target debug-${EXECUTABLE_TARGET})
    endif()
    _add_program_target(
        ${EXECUTABLE_TARGET}
        ${debug_target}
        debug
        "Opening debugger with target ${EXECUTABLE_TARGET}..."
    )

    message(
        NOTICE
        "
===============================================================================
  Added targets for programming and debugging '${EXECUTABLE_TARGET}':
    make ${program_target}  # program the microcontroller
    make ${debug_target}    # open GDB and connect to the micro for debugging
==============================================================================="
    )
endfunction()
