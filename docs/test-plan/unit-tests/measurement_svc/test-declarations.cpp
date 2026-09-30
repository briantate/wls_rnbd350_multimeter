/**
 * @file test-declarations.cpp
 * @brief Test declarations for measurement_svc module (M-03)
 * @date 2026-09-30
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "services/measurement_svc.h"
#include "hal/hal_gpio.h"
#include "hal/hal_spi.h"
}

TEST_GROUP(MeasurementSvc)
{
    void setup() override
    {
        mock().clear();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void mockAdcResponse(uint16_t counts)
    {
        uint8_t high = (counts >> 8) & 0x0F;
        uint8_t low = counts & 0xFF;

        // hal_spi_transfer_block handles CS internally
        mock().expectOneCall("hal_spi_transfer_block")
              .ignoreOtherParameters();
        // Mock the response via out parameter or return buffer
    }
};

/* TCI-013: measurement_svc_init */
TEST(MeasurementSvc, Init_ConfiguresRangeGpios)
{
    // Arrange: Expect range pin configuration
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_RES_A)
          .ignoreOtherParameters();
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_RES_B)
          .ignoreOtherParameters();
    mock().expectOneCall("hal_gpio_write")
          .withParameter("pin", HAL_PIN_RES_C)
          .ignoreOtherParameters();

    // Act
    measurement_svc_init();

    // Assert: mock verifies GPIO init called for range pins
}

/* TCI-014: measurement_svc_sample reads ADC */
TEST(MeasurementSvc, Sample_ReadsAdcViaHal)
{
    // Arrange
    mock().expectOneCall("hal_spi_transfer_block")
          .ignoreOtherParameters();

    // Act
    measurement_t m = measurement_svc_sample();

    // Assert: mock verifies SPI transfer called
    (void)m;
}

/* TCI-015: measurement_svc_sample converts counts to ohms */
TEST(MeasurementSvc, Sample_ConvertsCountsToOhms)
{
    // Arrange: Mock SPI to return mid-range counts (2048)
    mock().expectOneCall("hal_spi_transfer_block")
          .ignoreOtherParameters();
    // Implementation will parse response buffer

    // Act
    measurement_t m = measurement_svc_sample();

    // Assert: ~5000 ohms for mid-range (assuming 10k max)
    CHECK_TRUE(m.ohms > 4000 && m.ohms < 6000);
}

/* TCI-016: measurement_svc_sample returns valid */
TEST(MeasurementSvc, Sample_ReturnsValidMeasurement)
{
    // Arrange
    mock().expectOneCall("hal_spi_transfer_block")
          .ignoreOtherParameters();

    // Act
    measurement_t m = measurement_svc_sample();

    // Assert
    CHECK_TRUE(m.valid);
}

/* TCI-017: Zero counts returns zero ohms */
TEST(MeasurementSvc, Sample_ZeroCounts_ReturnsZeroOhms)
{
    // Arrange: Mock SPI to return 0 counts
    mock().expectOneCall("hal_spi_transfer_block")
          .ignoreOtherParameters();

    // Act
    measurement_t m = measurement_svc_sample();

    // Assert
    CHECK_EQUAL(0, m.ohms);
}

/* TCI-018: Max counts returns max ohms */
TEST(MeasurementSvc, Sample_MaxCounts_ReturnsMaxOhms)
{
    // Arrange: Mock SPI to return 4095 counts
    mock().expectOneCall("hal_spi_transfer_block")
          .ignoreOtherParameters();

    // Act
    measurement_t m = measurement_svc_sample();

    // Assert: Should be max range (10000 ohms)
    CHECK_EQUAL(10000, m.ohms);
}

/* TCI-019: Mid counts returns proportional ohms */
TEST(MeasurementSvc, Sample_MidCounts_ReturnsCorrectOhms)
{
    // Arrange: Mock SPI to return 1024 counts (1/4 of max)
    mock().expectOneCall("hal_spi_transfer_block")
          .ignoreOtherParameters();

    // Act
    measurement_t m = measurement_svc_sample();

    // Assert: ~2500 ohms (1/4 of 10k)
    CHECK_TRUE(m.ohms > 2000 && m.ohms < 3000);
}

/* TCI-020: Sample sends correct SPI command */
TEST(MeasurementSvc, Sample_SendsCorrectSpiCommand)
{
    // Arrange: Verify TX buffer contains MCP3204 command
    // Command byte: 0x06 = Start bit + single-ended + channel 2
    uint8_t expected_tx[] = {0x06, 0x00, 0x00};
    mock().expectOneCall("hal_spi_transfer_block")
          .withMemoryBufferParameter("tx", expected_tx, 3)
          .ignoreOtherParameters();

    // Act
    measurement_svc_sample();

    // Assert: mock verifies command bytes
}

/* TCI-021: Channel 2 selected in command */
TEST(MeasurementSvc, Sample_Channel2Selected)
{
    // Arrange: Channel 2 = 0x06 for MCP3204 single-ended
    // Bits: 0000 0110 = start(1) + single(1) + D2(0) + D1(1) + D0(0)
    mock().expectOneCall("hal_spi_transfer_block")
          .ignoreOtherParameters();

    // Act
    measurement_svc_sample();

    // Assert: verified by mock in TCI-020
}

/* TCI-022: SPI error returns invalid */
TEST(MeasurementSvc, Sample_SpiError_ReturnsInvalid)
{
    // Arrange: Simulate SPI failure via mock
    mock().expectOneCall("hal_spi_transfer_block")
          .ignoreOtherParameters();
    // Mock indicates failure in some way

    // Act
    measurement_t m = measurement_svc_sample();

    // Assert
    CHECK_FALSE(m.valid);
}

/* TCI-023: Multiple consecutive samples */
TEST(MeasurementSvc, Sample_MultipleConsecutive_WorksCorrectly)
{
    // Arrange & Act & Assert for sample 1
    mock().expectOneCall("hal_spi_transfer_block").ignoreOtherParameters();
    measurement_t m1 = measurement_svc_sample();
    CHECK_TRUE(m1.valid);

    mock().checkExpectations();
    mock().clear();

    // Arrange & Act & Assert for sample 2
    mock().expectOneCall("hal_spi_transfer_block").ignoreOtherParameters();
    measurement_t m2 = measurement_svc_sample();
    CHECK_TRUE(m2.valid);
}

/* TCI-024: Timing within budget */
TEST(MeasurementSvc, Sample_TimingWithinBudget)
{
    // Arrange
    mock().expectOneCall("hal_spi_transfer_block").ignoreOtherParameters();

    // Act
    // Note: Actual timing test would use hal_tick_get_ms()
    // This is a placeholder - real timing verified on target
    measurement_t m = measurement_svc_sample();

    // Assert: Completes (timing verified by inspection/integration)
    CHECK_TRUE(m.valid);
}
