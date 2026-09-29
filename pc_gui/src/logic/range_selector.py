"""Auto-range logic for the ohmmeter."""

import logging
from dataclasses import dataclass
from enum import Enum
from typing import Optional

logger = logging.getLogger(__name__)


class MeterRange(Enum):
    """Available meter ranges."""

    RANGE_1K = (1_000, "1 kΩ")
    RANGE_10K = (10_000, "10 kΩ")
    RANGE_100K = (100_000, "100 kΩ")
    RANGE_1M = (1_000_000, "1 MΩ")

    def __init__(self, max_value: int, label: str):
        self._max_value = max_value
        self._label = label

    @property
    def max_value(self) -> int:
        return self._max_value

    @property
    def label(self) -> str:
        return self._label


@dataclass
class RangeResult:
    """Result of range selection."""

    range: MeterRange
    normalized_value: float
    over_range: bool
    display_value: str
    range_label: str


class RangeSelector:
    """Selects the appropriate range with hysteresis."""

    HYSTERESIS_UP = 0.95
    HYSTERESIS_DOWN = 0.10

    RANGES = [
        MeterRange.RANGE_1K,
        MeterRange.RANGE_10K,
        MeterRange.RANGE_100K,
        MeterRange.RANGE_1M,
    ]

    def __init__(self):
        self._current_range: Optional[MeterRange] = None

    def select_range(self, ohms: float) -> RangeResult:
        """Select the appropriate range for the given resistance value."""
        over_range = ohms > MeterRange.RANGE_1M.max_value

        if self._current_range is None:
            new_range = self._find_best_range(ohms)
        else:
            new_range = self._apply_hysteresis(ohms)

        self._current_range = new_range

        if over_range:
            normalized = 1.0
        else:
            normalized = min(1.0, ohms / new_range.max_value)

        display_value = self._format_display(ohms, over_range)

        return RangeResult(
            range=new_range,
            normalized_value=normalized,
            over_range=over_range,
            display_value=display_value,
            range_label=new_range.label,
        )

    def _find_best_range(self, ohms: float) -> MeterRange:
        """Find the smallest range that can display the value."""
        for range_option in self.RANGES:
            if ohms <= range_option.max_value:
                return range_option
        return MeterRange.RANGE_1M

    def _apply_hysteresis(self, ohms: float) -> MeterRange:
        """Apply hysteresis to prevent range oscillation."""
        assert self._current_range is not None

        current_idx = self.RANGES.index(self._current_range)
        current_max = self._current_range.max_value

        if ohms > current_max * self.HYSTERESIS_UP and current_idx < len(self.RANGES) - 1:
            return self.RANGES[current_idx + 1]

        if current_idx > 0:
            lower_range = self.RANGES[current_idx - 1]
            if ohms < lower_range.max_value * self.HYSTERESIS_DOWN:
                return lower_range
            if ohms <= lower_range.max_value:
                return lower_range

        if ohms <= current_max:
            return self._current_range

        for i in range(current_idx + 1, len(self.RANGES)):
            if ohms <= self.RANGES[i].max_value:
                return self.RANGES[i]

        return MeterRange.RANGE_1M

    @staticmethod
    def _format_display(ohms: float, over_range: bool) -> str:
        """Format the resistance value for display."""
        if over_range:
            if ohms >= 1_000_000:
                return f"{ohms / 1_000_000:.3f} MΩ"
            elif ohms >= 1_000:
                return f"{ohms / 1_000:.3f} kΩ"
            else:
                return f"{ohms:.2f} Ω"

        if ohms >= 1_000_000:
            value = ohms / 1_000_000
            if value == int(value):
                return f"{int(value)} MΩ"
            return f"{value:.3f} MΩ"
        elif ohms >= 1_000:
            value = ohms / 1_000
            if value == int(value):
                return f"{int(value)} kΩ"
            return f"{value:.3f} kΩ"
        else:
            if ohms == int(ohms):
                return f"{int(ohms)} Ω"
            return f"{ohms:.2f} Ω"

    def reset(self) -> None:
        """Reset the range selector state."""
        self._current_range = None
