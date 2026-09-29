"""Main application window."""

import asyncio
import logging
from pathlib import Path
from typing import Optional

import customtkinter as ctk

from .gauge_widget import GaugeWidget
from .controls_panel import ControlsPanel
from ..ble.manager import BLEManager, DiscoveredDevice
from ..parser.json_stream import JSONStreamParser, MeterReading
from ..logic.range_selector import RangeSelector
from ..simulation.controller import SimulationController

logger = logging.getLogger(__name__)


class MainWindow(ctk.CTk):
    """Main application window with two-column layout."""

    def __init__(self, data_dir: Path):
        super().__init__()

        self._data_dir = data_dir

        self.title("BLE Ohmmeter")
        self.geometry("1200x700")
        self.minsize(900, 500)

        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")

        self._ble_manager: Optional[BLEManager] = None
        self._parser = JSONStreamParser(
            on_reading=self._on_meter_reading,
            on_error=self._on_parse_error,
        )
        self._range_selector = RangeSelector()

        sample_file = data_dir / "sample_resistance.jsonl"
        self._simulation = SimulationController(
            data_file=sample_file,
            on_data=self._on_simulation_data,
            on_status=self._on_status,
            on_complete=self._on_simulation_complete,
        )

        self._loop: Optional[asyncio.AbstractEventLoop] = None

        self._build_ui()
        self._init_ble()

        self.protocol("WM_DELETE_WINDOW", self._on_close)

    def _build_ui(self) -> None:
        """Build the main UI layout."""
        self.grid_columnconfigure(0, weight=0, minsize=280)
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

        self._controls = ControlsPanel(
            self,
            on_scan=self._handle_scan,
            on_connect=self._handle_connect,
            on_disconnect=self._handle_disconnect,
            on_simulate=self._handle_simulate,
            on_stop_simulate=self._handle_stop_simulate,
            on_clear_status=self._handle_clear_status,
            width=280,
        )
        self._controls.grid(row=0, column=0, sticky="nsew", padx=(10, 5), pady=10)

        self._gauge = GaugeWidget(self)
        self._gauge.grid(row=0, column=1, sticky="nsew", padx=(5, 10), pady=10)

    def _init_ble(self) -> None:
        """Initialize the BLE manager."""
        self._ble_manager = BLEManager(
            on_device_discovered=self._on_device_discovered,
            on_status=self._on_status,
            on_data=self._on_ble_data,
            on_connected=self._on_connection_changed,
        )

    def set_event_loop(self, loop: asyncio.AbstractEventLoop) -> None:
        """Set the asyncio event loop for BLE operations."""
        self._loop = loop

    def _run_async(self, coro) -> None:
        """Run an async coroutine in the event loop."""
        if self._loop:
            asyncio.run_coroutine_threadsafe(coro, self._loop)

    def _on_status(self, message: str) -> None:
        """Handle status messages from BLE or simulation."""
        self.after(0, lambda: self._controls.append_status(message))

    def _on_device_discovered(self, device: DiscoveredDevice) -> None:
        """Handle device discovery."""
        self.after(0, lambda: self._controls.add_device(device))

    def _on_connection_changed(self, connected: bool) -> None:
        """Handle connection state change."""
        self.after(0, lambda: self._controls.set_connected(connected))
        if not connected:
            self._parser.reset()
            self._range_selector.reset()
            self.after(0, self._gauge.reset)

    def _on_ble_data(self, data: bytes) -> None:
        """Handle incoming BLE data."""
        self._parser.feed(data)

    def _on_simulation_data(self, data: bytes) -> None:
        """Handle incoming simulation data."""
        self._on_status(f"SIM RX: {data!r}")
        self._parser.feed(data)

    def _on_meter_reading(self, reading: MeterReading) -> None:
        """Handle a validated meter reading."""
        result = self._range_selector.select_range(reading.ohms)

        self.after(
            0,
            lambda: self._gauge.set_value(
                normalized=result.normalized_value,
                digital=result.display_value,
                range_label=result.range_label,
                over_range=result.over_range,
            ),
        )

    def _on_parse_error(self, error: str) -> None:
        """Handle JSON parse errors."""
        self._on_status(f"Parse error: {error}")

    def _on_simulation_complete(self) -> None:
        """Handle simulation completion."""
        self.after(0, lambda: self._controls.set_simulating(False))

    def _handle_scan(self) -> None:
        """Handle scan button."""
        if not self._ble_manager:
            return

        if self._ble_manager.is_scanning:
            self._run_async(self._ble_manager.stop_scan())
            self._controls.set_scanning(False)
        else:
            self._controls.clear_devices()
            self._ble_manager.clear_discovered_devices()
            self._run_async(self._ble_manager.start_scan())
            self._controls.set_scanning(True)

    def _handle_connect(self, device: DiscoveredDevice) -> None:
        """Handle connect button."""
        if self._ble_manager:
            self._run_async(self._ble_manager.connect(device))

    def _handle_disconnect(self) -> None:
        """Handle disconnect button."""
        if self._ble_manager:
            self._run_async(self._ble_manager.disconnect())

    def _handle_simulate(self) -> None:
        """Handle simulate button."""
        if self._ble_manager and self._ble_manager.is_connected:
            self._on_status("Cannot simulate while connected to device")
            return

        if self._simulation.start(self):
            self._controls.set_simulating(True)

    def _handle_stop_simulate(self) -> None:
        """Handle stop simulation button."""
        self._simulation.stop()
        self._controls.set_simulating(False)

    def _handle_clear_status(self) -> None:
        """Handle clear status button."""
        self._controls.clear_status()

    def _on_close(self) -> None:
        """Handle window close."""
        logger.info("Closing application...")

        self._simulation.stop()

        if self._ble_manager:
            self._run_async(self._ble_manager.cleanup())

        self.after(200, self.destroy)
