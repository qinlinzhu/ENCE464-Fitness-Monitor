set(SRC_DIR ../target/src)
set(TIVAWARE_DIR ../target/libs/tivaware)
set(MOCK_DIR mocks) 


cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(FitnessTrackerTests LANGUAGES C)

enable_testing()
add_subdirectory(unity)

# add_unity_test(test_example test_example.c)


add_unity_test(test_circular_buffer test_circular_buffer.c ${SRC_DIR}/circBuf/circBufT.c)
target_include_directories(test_circular_buffer PRIVATE ${SRC_DIR}/circBuf)

add_unity_test(test_adc test_adc.c ${SRC_DIR}/ADC_read.c ${SRC_DIR}/hal/adc_hal_tiva.c)
target_include_directories(test_adc PRIVATE ${SRC_DIR} ${SRC_DIR}/hal ${SRC_DIR}/circBuf ${TIVAWARE_DIR} ${MOCK_DIR})

add_unity_test(test_adc_hal test_adc_hal.c ${SRC_DIR}/hal/adc_hal_tiva.c)
target_include_directories(test_adc_hal PRIVATE ${SRC_DIR} ${SRC_DIR}/hal ${SRC_DIR}/circBuf ${TIVAWARE_DIR} ${MOCK_DIR})

# add_unity_test(test_temp_hal test_temp_hal.c ${SRC_DIR}/hal/temp_hal_tiva.c)
# target_include_directories(test_temp_hal PRIVATE ${SRC_DIR} ${SRC_DIR}/hal ${SRC_DIR}/circBuf ${TIVAWARE_DIR} ${MOCK_DIR})

# add_unity_test(test_accl test_accl.c ${SRC_DIR}/accl_manager.c ${SRC_DIR}/hal/accl_hal_tiva.c)
# target_include_directories(test_accl PRIVATE ${SRC_DIR} ${SRC_DIR}/hal ${SRC_DIR}/circBuf ${TIVAWARE_DIR} ${MOCK_DIR})

# add_unity_test(test_accl_hal test_accl_hal.c ${SRC_DIR}/hal/accl_hal_tiva.c)
# target_include_directories(test_accl_hal PRIVATE ${SRC_DIR} ${SRC_DIR}/hal ${SRC_DIR}/circBuf ${TIVAWARE_DIR} ${MOCK_DIR})

