# ble_multimeter

Bluetooth Low Energy wireless resistance measurement firmware.

## Structure

| Path                              | Purpose                                                                                                                             |
|-----------------------------------|-------------------------------------------------------------------------------------------------------------------------------------|
| _build                            | The [CMake build tree](https://cmake.org/cmake/help/latest/manual/cmake.1.html#introduction-to-cmake-buildsystems), can be deleted. |
| cmake                             | Generated [CMake](https://cmake.org/) files. May be deleted if user.cmake has not been added                                        |
| .vscode                           | See [VSCode](https://code.visualstudio.com/docs/getstarted/settings)                                                                |
| .vscode\settings.json             | Workspace specific settings                                                                                                         |
| .vscode\ble_multimeter.mplab.json | The MPLAB project file, should not be deleted                                                                                       |
| out                               | Final build artifacts                                                                                                               |
| src/                              | Production source code (modules organized by layer)                                                                                 |
| tests/                            | Unit tests (CppUTest)                                                                                                               |

---

## Unit Tests

Unit tests use [CppUTest](https://cpputest.github.io/) and run inside the project's Docker container.

### Running Tests

```bash
# Run all unit tests
make unit_tests

# Run tests for a specific module
make unit_tests <ModuleName>
```

### Available Test Modules

| Module | Description | Tests |
|--------|-------------|-------|
| `AppState` | Application state machine (BLE connection state) | 12 |
| `MeasurementSvc` | Resistance measurement acquisition and ADC conversion | 12 |
| `BleSvc` | BLE service (RNBD350 UART parsing, JSON transmission) | 22 |

### Examples

```bash
# Run all tests (46 total)
make unit_tests
# Output: OK (46 tests, 46 ran, 785 checks, 0 ignored, 0 filtered out, 5 ms)

# Run only AppState tests
make unit_tests AppState
# Output: OK (46 tests, 12 ran, 17 checks, 0 ignored, 34 filtered out, 0 ms)

# Run only MeasurementSvc tests
make unit_tests MeasurementSvc
# Output: OK (46 tests, 12 ran, 40 checks, 0 ignored, 34 filtered out, 0 ms)

# Run only BleSvc tests
make unit_tests BleSvc
# Output: OK (46 tests, 22 ran, 728 checks, 0 ignored, 24 filtered out, 3 ms)
```

### Test Output Format

```
TEST(GroupName, TestName) - <time> ms
...
OK (<total> tests, <ran> ran, <checks> checks, <ignored> ignored, <filtered> filtered out, <time> ms)
```

### Adding New Test Modules

1. Create test file: `tests/<module>/test_<module>.cpp`
2. Add to `tests/CMakeLists.txt`:
   ```cmake
   set(TEST_SOURCES
       ...
       <module>/test_<module>.cpp
   )
   ```
3. Run `make unit_tests` to rebuild and run
