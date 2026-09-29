"""BLE connection manager using Bleak."""

import asyncio
import logging
from typing import Callable, Optional
from dataclasses import dataclass

from bleak import BleakClient, BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData

from .uuids import UUIDs

logger = logging.getLogger(__name__)


@dataclass
class DiscoveredDevice:
    """Represents a discovered BLE device."""

    device: BLEDevice
    name: str
    address: str

    def __str__(self) -> str:
        return f"{self.name} ({self.address})"


class BLEManager:
    """Manages BLE scanning, connection, and communication."""

    def __init__(
        self,
        on_device_discovered: Optional[Callable[[DiscoveredDevice], None]] = None,
        on_status: Optional[Callable[[str], None]] = None,
        on_data: Optional[Callable[[bytes], None]] = None,
        on_connected: Optional[Callable[[bool], None]] = None,
    ):
        self._on_device_discovered = on_device_discovered
        self._on_status = on_status
        self._on_data = on_data
        self._on_connected = on_connected

        self._scanner: Optional[BleakScanner] = None
        self._client: Optional[BleakClient] = None
        self._scanning = False
        self._connected = False
        self._discovered_devices: dict[str, DiscoveredDevice] = {}
        self._tx_char_uuid: Optional[str] = None

    def _log_status(self, message: str) -> None:
        """Log status message and notify callback."""
        logger.info(message)
        if self._on_status:
            self._on_status(message)

    def _detection_callback(
        self, device: BLEDevice, advertisement_data: AdvertisementData
    ) -> None:
        """Handle device detection during scan."""
        if device.address in self._discovered_devices:
            return

        name = device.name or advertisement_data.local_name or "Unknown"
        discovered = DiscoveredDevice(device=device, name=name, address=device.address)
        self._discovered_devices[device.address] = discovered

        self._log_status(f"Discovered: {name} [{device.address}]")

        if self._on_device_discovered:
            self._on_device_discovered(discovered)

    async def start_scan(self) -> None:
        """Start BLE scanning."""
        if self._scanning:
            await self.stop_scan()

        self._discovered_devices.clear()
        self._log_status("Starting BLE scan...")

        try:
            self._scanner = BleakScanner(detection_callback=self._detection_callback)
            await self._scanner.start()
            self._scanning = True
            self._log_status("BLE scan started")
        except Exception as e:
            self._log_status(f"Scan error: {e}")
            self._scanning = False

    async def stop_scan(self) -> None:
        """Stop BLE scanning."""
        if self._scanner and self._scanning:
            try:
                await self._scanner.stop()
                self._log_status("BLE scan stopped")
            except Exception as e:
                self._log_status(f"Error stopping scan: {e}")
            finally:
                self._scanning = False
                self._scanner = None

    def get_discovered_devices(self) -> list[DiscoveredDevice]:
        """Return list of discovered devices."""
        return list(self._discovered_devices.values())

    def clear_discovered_devices(self) -> None:
        """Clear the discovered devices cache."""
        self._discovered_devices.clear()

    @property
    def is_scanning(self) -> bool:
        """Return True if currently scanning."""
        return self._scanning

    @property
    def is_connected(self) -> bool:
        """Return True if connected to a device."""
        return self._connected

    def _disconnected_callback(self, client: BleakClient) -> None:
        """Handle unexpected disconnection."""
        self._log_status(f"Disconnected from {client.address}")
        self._connected = False
        if self._on_connected:
            self._on_connected(False)

    async def connect(self, device: DiscoveredDevice) -> bool:
        """Connect to a BLE device and set up notifications."""
        if self._connected:
            await self.disconnect()

        self._log_status(f"Connecting to {device.name} [{device.address}]...")

        try:
            self._client = BleakClient(
                device.device, disconnected_callback=self._disconnected_callback
            )
            await asyncio.wait_for(self._client.connect(), timeout=10.0)

            if not self._client.is_connected:
                self._log_status("Connection failed")
                return False

            self._connected = True
            self._log_status(f"Connected to {device.name}")

            await self._enumerate_services()

            if not await self._setup_notifications():
                self._log_status("Failed to set up notifications - missing service")
                await self.disconnect()
                return False

            if self._on_connected:
                self._on_connected(True)

            return True

        except asyncio.TimeoutError:
            self._log_status("Connection timeout")
            return False
        except Exception as e:
            self._log_status(f"Connection error: {e}")
            return False

    async def _enumerate_services(self) -> None:
        """Enumerate and log all services and characteristics."""
        if not self._client:
            return

        self._log_status("Enumerating services...")

        for service in self._client.services:
            self._log_status(f"Service: {service.uuid}")
            for char in service.characteristics:
                props = ", ".join(char.properties)
                self._log_status(f"  Char: {char.uuid} [{props}]")

    async def _setup_notifications(self) -> bool:
        """Set up notifications for the TX characteristic."""
        if not self._client:
            return False

        tx_char = None
        for service in self._client.services:
            if service.uuid.lower() == UUIDs.SERVICE.lower():
                self._log_status(f"Found Transparent UART service: {service.uuid}")
                for char in service.characteristics:
                    if char.uuid.lower() == UUIDs.TX_CHAR.lower():
                        tx_char = char
                        self._tx_char_uuid = char.uuid
                        break
                break

        if not tx_char:
            self._log_status(
                f"Transparent UART service or TX characteristic not found. "
                f"Expected service: {UUIDs.SERVICE}, TX: {UUIDs.TX_CHAR}"
            )
            return False

        if "notify" in tx_char.properties or "indicate" in tx_char.properties:
            try:
                await self._client.start_notify(
                    tx_char.uuid, self._notification_handler
                )
                self._log_status(f"Subscribed to TX notifications: {tx_char.uuid}")
                return True
            except Exception as e:
                self._log_status(f"Failed to subscribe: {e}")
                return False
        else:
            self._log_status(f"TX characteristic doesn't support notify/indicate")
            return False

    def _notification_handler(
        self, characteristic: int, data: bytearray  # noqa: ARG002
    ) -> None:
        """Handle incoming BLE notifications."""
        self._log_status(f"RX [{len(data)} bytes]: {data!r}")

        if self._on_data:
            self._on_data(bytes(data))

    async def disconnect(self) -> None:
        """Disconnect from the current device."""
        if self._client:
            try:
                if self._tx_char_uuid:
                    try:
                        await self._client.stop_notify(self._tx_char_uuid)
                    except Exception:
                        pass

                if self._client.is_connected:
                    await self._client.disconnect()
                    self._log_status("Disconnected")
            except Exception as e:
                self._log_status(f"Disconnect error: {e}")
            finally:
                self._client = None
                self._tx_char_uuid = None
                self._connected = False

                if self._on_connected:
                    self._on_connected(False)

    async def cleanup(self) -> None:
        """Clean up all resources."""
        await self.stop_scan()
        await self.disconnect()
