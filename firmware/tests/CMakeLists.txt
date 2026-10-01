find_package(PkgConfig)

# CppUTest setup
pkg_check_modules(CPPUTEST cpputest)

if(NOT CPPUTEST_FOUND)
    message(STATUS "CppUTest not found via pkg-config, trying manual find")
    find_path(CPPUTEST_INCLUDE_DIRS CppUTest/TestHarness.h)
    find_library(CPPUTEST_LIBRARIES CppUTest)
    find_library(CPPUTEST_EXT_LIBRARIES CppUTestExt)
endif()

if(CPPUTEST_INCLUDE_DIRS AND CPPUTEST_LIBRARIES)
    message(STATUS "CppUTest found: ${CPPUTEST_INCLUDE_DIRS}")
else()
    message(WARNING "CppUTest not found - tests will not be built")
    return()
endif()

# Mock library
add_library(hal_mocks STATIC
    mocks/hal_gpio_mock.cpp
    mocks/hal_spi_mock.cpp
    mocks/hal_uart_mock.cpp
    mocks/hal_tick_mock.cpp
)

target_include_directories(hal_mocks PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CPPUTEST_INCLUDE_DIRS}
)

target_link_libraries(hal_mocks
    ${CPPUTEST_LIBRARIES}
    ${CPPUTEST_EXT_LIBRARIES}
)

# Test runner
function(add_unit_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CPPUTEST_INCLUDE_DIRS}
    )
    target_link_libraries(${TEST_NAME}
        firmware_lib
        hal_mocks
        ${CPPUTEST_LIBRARIES}
        ${CPPUTEST_EXT_LIBRARIES}
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Tests will be added here as they are developed
# add_unit_test(test_app_state test_app_state.cpp)
