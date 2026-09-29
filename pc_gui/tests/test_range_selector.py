"""Tests for the range selector."""

import pytest

from src.logic.range_selector import RangeSelector, MeterRange


class TestRangeSelector:
    """Tests for RangeSelector."""

    def test_initial_range_selection_1k(self):
        """Test initial range selection for values <= 1k."""
        selector = RangeSelector()

        result = selector.select_range(500)

        assert result.range == MeterRange.RANGE_1K
        assert result.normalized_value == pytest.approx(0.5)
        assert not result.over_range

    def test_initial_range_selection_10k(self):
        """Test initial range selection for values <= 10k."""
        selector = RangeSelector()

        result = selector.select_range(5000)

        assert result.range == MeterRange.RANGE_10K
        assert result.normalized_value == pytest.approx(0.5)

    def test_initial_range_selection_100k(self):
        """Test initial range selection for values <= 100k."""
        selector = RangeSelector()

        result = selector.select_range(50000)

        assert result.range == MeterRange.RANGE_100K
        assert result.normalized_value == pytest.approx(0.5)

    def test_initial_range_selection_1m(self):
        """Test initial range selection for values <= 1M."""
        selector = RangeSelector()

        result = selector.select_range(500000)

        assert result.range == MeterRange.RANGE_1M
        assert result.normalized_value == pytest.approx(0.5)

    def test_over_range(self):
        """Test over-range detection."""
        selector = RangeSelector()

        result = selector.select_range(1500000)

        assert result.over_range
        assert result.normalized_value == 1.0
        assert result.range == MeterRange.RANGE_1M

    def test_exact_boundary_1k(self):
        """Test exact 1k boundary stays on 1k range."""
        selector = RangeSelector()

        result = selector.select_range(1000)

        assert result.range == MeterRange.RANGE_1K
        assert result.normalized_value == pytest.approx(1.0)

    def test_exact_boundary_10k(self):
        """Test exact 10k boundary stays on 10k range."""
        selector = RangeSelector()

        result = selector.select_range(10000)

        assert result.range == MeterRange.RANGE_10K
        assert result.normalized_value == pytest.approx(1.0)

    def test_exact_boundary_100k(self):
        """Test exact 100k boundary stays on 100k range."""
        selector = RangeSelector()

        result = selector.select_range(100000)

        assert result.range == MeterRange.RANGE_100K
        assert result.normalized_value == pytest.approx(1.0)

    def test_exact_boundary_1m(self):
        """Test exact 1M boundary stays on 1M range."""
        selector = RangeSelector()

        result = selector.select_range(1000000)

        assert result.range == MeterRange.RANGE_1M
        assert result.normalized_value == pytest.approx(1.0)
        assert not result.over_range

    def test_hysteresis_prevents_oscillation(self):
        """Test that hysteresis prevents range oscillation at boundaries."""
        selector = RangeSelector()

        selector.select_range(900)
        assert selector._current_range == MeterRange.RANGE_1K

        result = selector.select_range(1050)
        assert result.range == MeterRange.RANGE_10K

        result = selector.select_range(950)
        assert result.range == MeterRange.RANGE_1K

        result = selector.select_range(1050)
        assert result.range == MeterRange.RANGE_10K

    def test_hysteresis_up_threshold(self):
        """Test upward range change threshold (95%)."""
        selector = RangeSelector()

        selector.select_range(500)
        assert selector._current_range == MeterRange.RANGE_1K

        result = selector.select_range(940)
        assert result.range == MeterRange.RANGE_1K

        result = selector.select_range(960)
        assert result.range == MeterRange.RANGE_10K

    def test_hysteresis_down_threshold(self):
        """Test downward range change threshold."""
        selector = RangeSelector()

        selector.select_range(5000)
        assert selector._current_range == MeterRange.RANGE_10K

        result = selector.select_range(1000)
        assert result.range == MeterRange.RANGE_1K

    def test_display_format_ohms(self):
        """Test display formatting for ohms."""
        selector = RangeSelector()

        result = selector.select_range(500)
        assert "Ω" in result.display_value
        assert "500" in result.display_value

    def test_display_format_kilohms(self):
        """Test display formatting for kilohms."""
        selector = RangeSelector()

        result = selector.select_range(5000)
        assert "kΩ" in result.display_value
        assert "5" in result.display_value

    def test_display_format_megohms(self):
        """Test display formatting for megohms."""
        selector = RangeSelector()

        result = selector.select_range(1000000)
        assert "MΩ" in result.display_value
        assert "1" in result.display_value

    def test_reset(self):
        """Test reset clears current range."""
        selector = RangeSelector()

        selector.select_range(50000)
        assert selector._current_range == MeterRange.RANGE_100K

        selector.reset()
        assert selector._current_range is None

        result = selector.select_range(500)
        assert result.range == MeterRange.RANGE_1K

    def test_zero_value(self):
        """Test handling of zero value."""
        selector = RangeSelector()

        result = selector.select_range(0)

        assert result.range == MeterRange.RANGE_1K
        assert result.normalized_value == 0.0
        assert not result.over_range

    def test_range_labels(self):
        """Test that range labels are correct."""
        assert MeterRange.RANGE_1K.label == "1 kΩ"
        assert MeterRange.RANGE_10K.label == "10 kΩ"
        assert MeterRange.RANGE_100K.label == "100 kΩ"
        assert MeterRange.RANGE_1M.label == "1 MΩ"

    def test_range_max_values(self):
        """Test that range max values are correct."""
        assert MeterRange.RANGE_1K.max_value == 1000
        assert MeterRange.RANGE_10K.max_value == 10000
        assert MeterRange.RANGE_100K.max_value == 100000
        assert MeterRange.RANGE_1M.max_value == 1000000

    def test_normalized_value_clamped(self):
        """Test that normalized value is clamped to [0, 1]."""
        selector = RangeSelector()

        result = selector.select_range(1200)
        selector._current_range = MeterRange.RANGE_1K
        result = selector.select_range(1200)

        assert result.normalized_value <= 1.0

    def test_display_format_integer_values(self):
        """Test display formatting for integer values."""
        selector = RangeSelector()

        result = selector.select_range(1000)
        assert result.display_value == "1 kΩ"

        result = selector.select_range(10000)
        assert result.display_value == "10 kΩ"

    def test_display_format_decimal_values(self):
        """Test display formatting for decimal values."""
        selector = RangeSelector()

        result = selector.select_range(1234.5)
        assert "1.234" in result.display_value or "1.235" in result.display_value
