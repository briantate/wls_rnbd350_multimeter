"""Simulation controller for playing back sample data."""

import logging
import os
from pathlib import Path
from typing import Callable, Optional

logger = logging.getLogger(__name__)


class SimulationController:
    """Controls playback of sample resistance data."""

    PLAYBACK_DURATION_MS = 10_000

    def __init__(
        self,
        data_file: Path,
        on_data: Callable[[bytes], None],
        on_status: Optional[Callable[[str], None]] = None,
        on_complete: Optional[Callable[[], None]] = None,
    ):
        self._data_file = data_file
        self._on_data = on_data
        self._on_status = on_status
        self._on_complete = on_complete

        self._lines: list[str] = []
        self._running = False
        self._current_index = 0
        self._timer_id: Optional[str] = None
        self._root = None

    def _log_status(self, message: str) -> None:
        """Log status message."""
        logger.info(message)
        if self._on_status:
            self._on_status(message)

    def load_data(self) -> bool:
        """Load sample data from file."""
        if not self._data_file.exists():
            self._log_status(f"Sample file not found: {self._data_file}")
            return False

        try:
            with open(self._data_file, "r", encoding="utf-8") as f:
                self._lines = [line.strip() for line in f if line.strip()]

            if not self._lines:
                self._log_status("Sample file is empty")
                return False

            self._log_status(f"Loaded {len(self._lines)} samples from {self._data_file.name}")
            return True

        except Exception as e:
            self._log_status(f"Error loading sample file: {e}")
            return False

    def start(self, root) -> bool:
        """Start simulation playback."""
        if self._running:
            return False

        if not self._lines and not self.load_data():
            return False

        self._root = root
        self._running = True
        self._current_index = 0

        interval_ms = self.PLAYBACK_DURATION_MS // len(self._lines)
        interval_ms = max(10, interval_ms)

        self._log_status(
            f"Starting simulation: {len(self._lines)} samples over {self.PLAYBACK_DURATION_MS / 1000:.1f}s "
            f"({interval_ms}ms interval)"
        )

        self._schedule_next(interval_ms)
        return True

    def _schedule_next(self, interval_ms: int) -> None:
        """Schedule the next sample playback."""
        if not self._running or self._root is None:
            return

        self._timer_id = self._root.after(interval_ms, lambda: self._play_sample(interval_ms))

    def _play_sample(self, interval_ms: int) -> None:
        """Play the current sample and schedule next."""
        if not self._running:
            return

        if self._current_index >= len(self._lines):
            self._log_status("Simulation complete")
            self._running = False
            if self._on_complete:
                self._on_complete()
            return

        line = self._lines[self._current_index]
        data = (line + "\n").encode("utf-8")

        self._on_data(data)
        self._current_index += 1

        self._schedule_next(interval_ms)

    def stop(self) -> None:
        """Stop simulation playback."""
        self._running = False

        if self._timer_id and self._root:
            try:
                self._root.after_cancel(self._timer_id)
            except Exception:
                pass
            self._timer_id = None

        self._log_status("Simulation stopped")

    @property
    def is_running(self) -> bool:
        """Return True if simulation is running."""
        return self._running

    def reset(self) -> None:
        """Reset simulation state."""
        self.stop()
        self._current_index = 0
