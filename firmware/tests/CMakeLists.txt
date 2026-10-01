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
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CPPUTEST_INCLUDE_DIRS}
)

target_link_libraries(hal_mocks
    ${CPPUTEST_LIBRARIES}
    ${CPPUTEST_EXT_LIBRARIES}
)

# Collect all test source files (add new test files here)
set(TEST_SOURCES
    app_state/test_app_state.cpp
    measurement_svc/test_measurement_svc.cpp
    ble_svc/test_ble_svc.cpp
)

# Unified test executable
add_executable(test_all
    AllTests.cpp
    ${TEST_SOURCES}
)

target_include_directories(test_all PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CPPUTEST_INCLUDE_DIRS}
)

target_link_libraries(test_all
    firmware_lib
    hal_mocks
    ${CPPUTEST_LIBRARIES}
    ${CPPUTEST_EXT_LIBRARIES}
)

# Register with CTest
add_test(NAME all_tests COMMAND test_all)

# Unified target to build and run all unit tests with verbose output
# Usage: make unit_tests          - run all tests
#        make unit_tests GROUP=AppState - run only AppState tests
add_custom_target(unit_tests
    COMMAND test_all -v $ENV{TEST_GROUP_FLAG}
    DEPENDS test_all
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running unit tests"
)
