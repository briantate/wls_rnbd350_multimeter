"""JSON stream parser for meter readings."""

import json
import logging
import math
from dataclasses import dataclass
from typing import Callable, Optional

logger = logging.getLogger(__name__)


@dataclass
class MeterReading:
    """Represents a validated meter reading."""

    ohms: float


class JSONStreamParser:
    """Parses JSON messages from a byte stream, handling fragmentation."""

    def __init__(
        self,
        on_reading: Optional[Callable[[MeterReading], None]] = None,
        on_error: Optional[Callable[[str], None]] = None,
    ):
        self._buffer = ""
        self._on_reading = on_reading
        self._on_error = on_error

    def feed(self, data: bytes) -> list[MeterReading]:
        """Feed raw bytes into the parser and return any complete readings."""
        try:
            text = data.decode("utf-8")
        except UnicodeDecodeError as e:
            self._report_error(f"UTF-8 decode error: {e}")
            return []

        text = text.replace("\r", "")
        self._buffer += text

        readings = []

        while "\n" in self._buffer:
            line, self._buffer = self._buffer.split("\n", 1)
            line = line.strip()

            if not line:
                continue

            reading = self._parse_line(line)
            if reading:
                readings.append(reading)
                if self._on_reading:
                    self._on_reading(reading)

        return readings

    def _parse_line(self, line: str) -> Optional[MeterReading]:
        """Parse a single JSON line and return a MeterReading if valid."""
        try:
            data = json.loads(line)
        except json.JSONDecodeError as e:
            self._report_error(f"JSON parse error: {e} in '{line}'")
            return None

        if not isinstance(data, dict):
            self._report_error(f"Expected JSON object, got: {type(data).__name__}")
            return None

        meter = data.get("Meter")
        if not isinstance(meter, dict):
            self._report_error(f"Missing or invalid 'Meter' object in: {line}")
            return None

        ohm_value = meter.get("ohm")
        if ohm_value is None:
            self._report_error(f"Missing 'ohm' value in: {line}")
            return None

        try:
            ohms = float(ohm_value)
        except (TypeError, ValueError) as e:
            self._report_error(f"Invalid 'ohm' value '{ohm_value}': {e}")
            return None

        if not math.isfinite(ohms):
            self._report_error(f"Non-finite 'ohm' value: {ohms}")
            return None

        if ohms < 0:
            self._report_error(f"Negative 'ohm' value: {ohms}")
            return None

        return MeterReading(ohms=ohms)

    def _report_error(self, message: str) -> None:
        """Log and report an error."""
        logger.warning(message)
        if self._on_error:
            self._on_error(message)

    def reset(self) -> None:
        """Clear the internal buffer."""
        self._buffer = ""
