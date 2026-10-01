cmake_minimum_required(VERSION 3.14)
project(bluetooth_ohmmeter_firmware C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTING "Build unit tests" ON)

# Source files
set(APP_SOURCES
    src/app/app_state.c
)

set(SERVICE_SOURCES
    src/services/measurement_svc.c
    src/services/ble_svc.c
    src/services/led_svc.c
    src/services/diag_svc.c
)

set(HAL_SOURCES
    src/hal/hal_gpio.c
    src/hal/hal_spi.c
    src/hal/hal_uart.c
    src/hal/hal_tick.c
)

# Library for testing (excludes main.c and HAL for mocking)
add_library(firmware_lib STATIC
    ${APP_SOURCES}
    ${SERVICE_SOURCES}
)

target_include_directories(firmware_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
