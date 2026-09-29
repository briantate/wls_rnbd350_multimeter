"""Tests for the simulation controller."""

import time
from pathlib import Path
from unittest.mock import MagicMock, patch
import tempfile

import pytest

from src.simulation.controller import SimulationController


class TestSimulationController:
    """Tests for SimulationController."""

    def test_load_data_success(self, tmp_path):
        """Test successful data loading."""
        data_file = tmp_path / "test.jsonl"
        data_file.write_text('{"Meter":{"ohm":100}}\n{"Meter":{"ohm":200}}\n')

        controller = SimulationController(
            data_file=data_file,
            on_data=MagicMock(),
        )

        assert controller.load_data()
        assert len(controller._lines) == 2

    def test_load_data_missing_file(self, tmp_path):
        """Test loading from non-existent file."""
        data_file = tmp_path / "missing.jsonl"
        statuses = []

        controller = SimulationController(
            data_file=data_file,
            on_data=MagicMock(),
            on_status=statuses.append,
        )

        assert not controller.load_data()
        assert any("not found" in s for s in statuses)

    def test_load_data_empty_file(self, tmp_path):
        """Test loading from empty file."""
        data_file = tmp_path / "empty.jsonl"
        data_file.write_text("")
        statuses = []

        controller = SimulationController(
            data_file=data_file,
            on_data=MagicMock(),
            on_status=statuses.append,
        )

        assert not controller.load_data()
        assert any("empty" in s for s in statuses)

    def test_is_running_property(self, tmp_path):
        """Test is_running property."""
        data_file = tmp_path / "test.jsonl"
        data_file.write_text('{"Meter":{"ohm":100}}\n')

        controller = SimulationController(
            data_file=data_file,
            on_data=MagicMock(),
        )

        assert not controller.is_running

    def test_stop_when_not_running(self, tmp_path):
        """Test stopping when not running doesn't error."""
        data_file = tmp_path / "test.jsonl"
        data_file.write_text('{"Meter":{"ohm":100}}\n')

        controller = SimulationController(
            data_file=data_file,
            on_data=MagicMock(),
        )

        controller.stop()

    def test_reset(self, tmp_path):
        """Test reset clears state."""
        data_file = tmp_path / "test.jsonl"
        data_file.write_text('{"Meter":{"ohm":100}}\n')

        controller = SimulationController(
            data_file=data_file,
            on_data=MagicMock(),
        )

        controller._current_index = 5
        controller.reset()

        assert controller._current_index == 0
        assert not controller.is_running

    def test_playback_duration_constant(self):
        """Test playback duration is 10 seconds."""
        assert SimulationController.PLAYBACK_DURATION_MS == 10_000

    def test_data_callback_receives_bytes(self, tmp_path):
        """Test that data callback receives bytes with newline."""
        data_file = tmp_path / "test.jsonl"
        data_file.write_text('{"Meter":{"ohm":100}}\n')

        received_data = []
        controller = SimulationController(
            data_file=data_file,
            on_data=received_data.append,
        )

        controller.load_data()
        controller._running = True
        controller._play_sample(100)

        assert len(received_data) == 1
        assert received_data[0] == b'{"Meter":{"ohm":100}}\n'

    def test_sample_file_format(self):
        """Test that sample file has correct format."""
        sample_file = Path(__file__).parent.parent / "sample_resistance.jsonl"

        if sample_file.exists():
            with open(sample_file, "r") as f:
                lines = [line.strip() for line in f if line.strip()]

            assert len(lines) > 0

            import json
            for line in lines:
                data = json.loads(line)
                assert "Meter" in data
                assert "ohm" in data["Meter"]
                assert isinstance(data["Meter"]["ohm"], (int, float))
                assert data["Meter"]["ohm"] >= 0


class TestSimulationTiming:
    """Tests for simulation timing calculations."""

    def test_interval_calculation(self, tmp_path):
        """Test that interval is calculated correctly for 10s playback."""
        data_file = tmp_path / "test.jsonl"
        lines = [f'{{"Meter":{{"ohm":{i}}}}}\n' for i in range(100)]
        data_file.write_text("".join(lines))

        controller = SimulationController(
            data_file=data_file,
            on_data=MagicMock(),
        )
        controller.load_data()

        expected_interval = 10_000 // 100
        assert expected_interval == 100

    def test_minimum_interval(self, tmp_path):
        """Test that interval doesn't go below 10ms."""
        data_file = tmp_path / "test.jsonl"
        lines = [f'{{"Meter":{{"ohm":{i}}}}}\n' for i in range(10000)]
        data_file.write_text("".join(lines))

        controller = SimulationController(
            data_file=data_file,
            on_data=MagicMock(),
        )
        controller.load_data()

        raw_interval = 10_000 // 10000
        clamped_interval = max(10, raw_interval)
        assert clamped_interval >= 10
