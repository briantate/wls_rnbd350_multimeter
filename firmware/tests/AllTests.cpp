/**
 * @file AllTests.cpp
 * @brief Unified test runner for all unit tests
 *
 * This file provides a single entry point for running all unit tests.
 * Individual test files register their tests via TEST() macros.
 *
 * Usage:
 *   ./test_all           - Run all tests
 *   ./test_all -v        - Run all tests with verbose output
 *   ./test_all -g AppState     - Run only AppState tests
 *   ./test_all -g MeasurementSvc - Run only MeasurementSvc tests
 */

#include "CppUTest/CommandLineTestRunner.h"

int main(int argc, char** argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
