export

# Define what we are building
TARGET = controller

# Build input directories
FIRMWARE_DIR := firmware
APP_DIR := $(FIRMWARE_DIR)/src
HAL_DIR := $(FIRMWARE_DIR)/hal
LINKER_DIR := $(FIRMWARE_DIR)
TEST_DIR := firmware/tests
COVERAGE_DIR := $(TEST_DIR)/coverage

# Build output directores
BUILD_DIR := $(FIRMWARE_DIR)/build
EXE_DIR := bin
ANALYSIS_DIR := analytics

# List of the application headers and sources. The APP code should be
# hardware independent so that it can be used on-target, in simulation,
# and in unit tests. Includes sources in subdirectories (e.g., src/temp_ctrl/).
APP_HEADERS := $(shell find $(APP_DIR) -type f -name '*.h')
APP_SOURCES := $(shell find $(APP_DIR) -type f -name '*.cpp')
APP_C_SOURCES := $(shell find $(APP_DIR) -type f -name '*.c')
APP_INCLUDES := $(patsubst %,-I%,$(sort $(dir $(APP_HEADERS))))

# List of firmware root sources (main.c, drivers, etc.)
FIRMWARE_HEADERS := $(wildcard $(FIRMWARE_DIR)/*.h)
FIRMWARE_C_SOURCES := $(wildcard $(FIRMWARE_DIR)/*.c)
FIRMWARE_ASSEMBLY := $(wildcard $(FIRMWARE_DIR)/*.[sS])
FIRMWARE_INCLUDES := $(patsubst %,-I%,$(sort $(dir $(FIRMWARE_HEADERS))))

# List of HAL headers and sources.
HAL_HEADERS := $(wildcard $(HAL_DIR)/*.h)
HAL_C_SOURCES := $(wildcard $(HAL_DIR)/*.c)
HAL_INCLUDES := $(patsubst %,-I%,$(sort $(dir $(HAL_HEADERS))))

# List of the microcontroller headers and sources. The MCU code should be
# hardware dependent and should be able to be used on-target.
# NOTE: The following looks messy because of how STM32CubeMx generates code
#MCU_HEADERS   := $(shell find $(MCU_DIR) -type f -name '*.h*')#$(wildcard $(MCU_DIR)/*.h*)
#MCU_SOURCES   := $(wildcard $(MCU_DIR)/*.cpp)
#MCU_C_SOURCES := $(wildcard $(MCU_DIR)/Core/Src/*.c)
#MCU_C_SOURCES += $(filter-out %template.c, $(wildcard $(MCU_DIR)/Drivers/STM32L4xx_HAL_Driver/Src/*.c))
#MCU_C_SOURCES += $(wildcard $(MCU_DIR)/Middlewares/Third_Party/FreeRTOS/Source/*.c)
#MCU_C_SOURCES += $(wildcard $(MCU_DIR)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/*.c)
#MCU_C_SOURCES += $(wildcard $(MCU_DIR)/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c)
#MCU_C_SOURCES += $(wildcard $(MCU_DIR)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/*.c)
#MCU_ASSEMBLY  := $(wildcard $(MCU_DIR)/*.[sS])
#MCU_INCLUDES := $(patsubst %,-I%,$(sort $(dir $(MCU_HEADERS))))

# List of the test headers and sources. The TEST code should be
# hardware independent and should be able to be used in the test harness
TEST_HEADERS := $(wildcard $(TEST_DIR)/*.h*)
TEST_SOURCES := $(wildcard $(TEST_DIR)/*.c)
TEST_SOURCES += $(wildcard $(TEST_DIR)/*.cpp)
TEST_INCLUDES := $(patsubst %,-I%,$(sort $(dir $(TEST_HEADERS))))

# Gcov and Gcovr flags
GCOVR_EXCLUDE_DIR = '($(TEST_DIR)|$(LIB_DIR))'
GCOVR_EXCLUDE_FLAG := --exclude $(GCOVR_EXCLUDE_DIR)
GCOVR_FLAGS := $(GCOVR_EXCLUDE_FLAG) --txt --html-details --html=$(COVERAGE_DIR)/coverage.html

#######################################
# build targets
#######################################

.PHONY: all clean mccabe_analysis

all: release debug

clean:
	-rm -fR $(BUILD_DIR)
	-rm -fR $(EXE_DIR)
	-rm -fR $(COVERAGE_DIR)
	-rm -fR $(TEST_DIR)/junit
	-rm -fR $(TEST_DIR)/lib
	-rm -fR $(TEST_DIR)/obj

release debug:
	$(MAKE) -j -f $(FIRMWARE_DIR)/$@.mk

mccabe_analysis:
	@pmccabe -vt $(APP_C_SOURCES) $(FIRMWARE_C_SOURCES) $(HAL_C_SOURCES)

#######################################
# Unit Tests (CMake-based)
#######################################
# Usage:
#   make unit_tests                   - run all tests
#   make unit_tests AppState          - run only AppState tests
#   make unit_tests MeasurementSvc    - run only MeasurementSvc tests

# Extract group filter from command line (word after unit_tests)
TEST_GROUP := $(word 2,$(MAKECMDGOALS))

# Prevent make from treating the group name as a target
ifneq ($(TEST_GROUP),)
$(TEST_GROUP):
	@:
endif

.PHONY: unit_tests
unit_tests:
	@mkdir -p $(FIRMWARE_DIR)/build
	@cd $(FIRMWARE_DIR)/build && cmake -DBUILD_TESTING=ON .. > /dev/null 2>&1
	@cd $(FIRMWARE_DIR)/build && cmake --build . --target test_all > /dev/null
ifneq ($(TEST_GROUP),)
	@cd $(FIRMWARE_DIR)/build/tests && ./test_all -v -g $(TEST_GROUP)
else
	@cd $(FIRMWARE_DIR)/build/tests && ./test_all -v
endif

#######################################
# Unit Tests (Make-based) - legacy
#######################################
# .PHONY: unit_tests
# unit_tests:
# 	$(MAKE) -j CC=gcc -f $(TEST_DIR)/cpputest.mk
# 	mkdir -p $(COVERAGE_DIR)
# #	mkdir -p $(TEST_DIR)/junit
# #	mv *.xml $(TEST_DIR)/junit
# 	@gcovr $(GCOVR_FLAGS)

.PHONY: docker_image
docker_image:
# --platform=linux/amd64 pins the build to amd64 because the ARM GNU
# toolchain installed inside the image is the x86_64-hosted variant.
# On Apple Silicon hosts this runs under Rosetta or QEMU emulation.
# --pull refreshes the ubuntu:22.04 base image so apt-get update sees
# current GPG signing keys (stale base images cause "invalid signature"
# errors during apt-get update).
	docker build --platform=linux/amd64 --pull -t name/embedded-dev -f docker/Dockerfile .

# Note, if you are using windows, you can use WSL2 or
# change pwd to
.PHONY: docker_run
docker_run:
# if you are using windosws powershell, use the following to start Docker:
#	cmd /c "docker run --platform=linux/amd64 --rm -it --privileged -v "%CD%:/home/app" name/embedded-dev:latest
# otherwise, MacOS and Linux can use the following:
	docker run -p 3456:3456 --platform=linux/amd64 --rm -it --privileged -v "$(CURDIR):/home/app" name/embedded-dev:latest bash

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***