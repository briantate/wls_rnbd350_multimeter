"""BLE controls panel widget."""

import tkinter as tk
from typing import Callable, Optional

import customtkinter as ctk

from ..ble.manager import DiscoveredDevice


class ControlsPanel(ctk.CTkFrame):
    """Left panel containing BLE controls and status."""

    def __init__(
        self,
        master,
        on_scan: Callable[[], None],
        on_connect: Callable[[DiscoveredDevice], None],
        on_disconnect: Callable[[], None],
        on_simulate: Callable[[], None],
        on_stop_simulate: Callable[[], None],
        on_clear_status: Callable[[], None],
        **kwargs,
    ):
        super().__init__(master, **kwargs)

        self._on_scan = on_scan
        self._on_connect = on_connect
        self._on_disconnect = on_disconnect
        self._on_simulate = on_simulate
        self._on_stop_simulate = on_stop_simulate
        self._on_clear_status = on_clear_status

        self._devices: list[DiscoveredDevice] = []
        self._filtered_devices: list[DiscoveredDevice] = []
        self._selected_device: Optional[DiscoveredDevice] = None
        self._scanning = False
        self._connected = False
        self._simulating = False

        self._build_ui()

    def _build_ui(self) -> None:
        """Build the control panel UI."""
        self.configure(fg_color="#1e1e2e")

        title = ctk.CTkLabel(
            self,
            text="BLE Ohmmeter",
            font=ctk.CTkFont(size=24, weight="bold"),
        )
        title.pack(pady=(15, 20))

        self._scan_btn = ctk.CTkButton(
            self,
            text="Scan",
            command=self._handle_scan,
            width=200,
            height=40,
            font=ctk.CTkFont(size=14),
        )
        self._scan_btn.pack(pady=5)

        filter_label = ctk.CTkLabel(
            self,
            text="Device Filter:",
            font=ctk.CTkFont(size=12),
        )
        filter_label.pack(pady=(15, 2), anchor="w", padx=20)

        self._filter_var = tk.StringVar()
        self._filter_var.trace_add("write", self._on_filter_changed)

        self._filter_entry = ctk.CTkEntry(
            self,
            textvariable=self._filter_var,
            placeholder_text="Filter by name or address...",
            width=200,
            height=32,
        )
        self._filter_entry.pack(pady=(0, 10), padx=20)

        devices_label = ctk.CTkLabel(
            self,
            text="Discovered Devices:",
            font=ctk.CTkFont(size=12),
        )
        devices_label.pack(pady=(5, 2), anchor="w", padx=20)

        self._device_listbox = tk.Listbox(
            self,
            bg="#2a2a3e",
            fg="#ffffff",
            selectbackground="#4a4a6e",
            selectforeground="#ffffff",
            font=("Consolas", 10),
            height=8,
            borderwidth=0,
            highlightthickness=1,
            highlightbackground="#3a3a5e",
        )
        self._device_listbox.pack(fill=tk.X, padx=20, pady=5)
        self._device_listbox.bind("<<ListboxSelect>>", self._on_device_selected)

        self._connect_btn = ctk.CTkButton(
            self,
            text="Connect",
            command=self._handle_connect,
            width=200,
            height=40,
            font=ctk.CTkFont(size=14),
            state="disabled",
        )
        self._connect_btn.pack(pady=10)

        self._status_label = ctk.CTkLabel(
            self,
            text="Status: Disconnected",
            font=ctk.CTkFont(size=12),
            text_color="#aaaaaa",
        )
        self._status_label.pack(pady=5)

        separator = ctk.CTkFrame(self, height=2, fg_color="#3a3a5e")
        separator.pack(fill=tk.X, padx=20, pady=15)

        self._simulate_btn = ctk.CTkButton(
            self,
            text="Simulate",
            command=self._handle_simulate,
            width=200,
            height=40,
            font=ctk.CTkFont(size=14),
            fg_color="#2a6a2a",
            hover_color="#3a8a3a",
        )
        self._simulate_btn.pack(pady=5)

        separator2 = ctk.CTkFrame(self, height=2, fg_color="#3a3a5e")
        separator2.pack(fill=tk.X, padx=20, pady=15)

        status_header = ctk.CTkFrame(self, fg_color="transparent")
        status_header.pack(fill=tk.X, padx=20)

        status_title = ctk.CTkLabel(
            status_header,
            text="BLE Status Log:",
            font=ctk.CTkFont(size=12),
        )
        status_title.pack(side=tk.LEFT)

        clear_btn = ctk.CTkButton(
            status_header,
            text="Clear",
            command=self._on_clear_status,
            width=60,
            height=24,
            font=ctk.CTkFont(size=11),
        )
        clear_btn.pack(side=tk.RIGHT)

        status_frame = ctk.CTkFrame(self, fg_color="#0a0a14")
        status_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=(5, 15))

        self._status_text = tk.Text(
            status_frame,
            bg="#0a0a14",
            fg="#00ff00",
            font=("Consolas", 9),
            wrap=tk.WORD,
            state=tk.DISABLED,
            borderwidth=0,
            highlightthickness=0,
        )
        self._status_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        scrollbar = ctk.CTkScrollbar(status_frame, command=self._status_text.yview)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self._status_text.configure(yscrollcommand=scrollbar.set)

    def _handle_scan(self) -> None:
        """Handle scan button click."""
        self._on_scan()

    def _handle_connect(self) -> None:
        """Handle connect/disconnect button click."""
        if self._connected:
            self._on_disconnect()
        elif self._selected_device:
            self._on_connect(self._selected_device)

    def _handle_simulate(self) -> None:
        """Handle simulate button click."""
        if self._simulating:
            self._on_stop_simulate()
        else:
            self._on_simulate()

    def _on_device_selected(self, event: tk.Event) -> None:
        """Handle device selection in listbox."""
        selection = self._device_listbox.curselection()
        if selection:
            index = selection[0]
            if index < len(self._filtered_devices):
                self._selected_device = self._filtered_devices[index]
                self._connect_btn.configure(state="normal")
        else:
            self._selected_device = None
            if not self._connected:
                self._connect_btn.configure(state="disabled")

    def _on_filter_changed(self, *args) -> None:
        """Handle filter text change."""
        self._apply_filter()

    def _apply_filter(self) -> None:
        """Apply the current filter to the device list."""
        filter_text = self._filter_var.get().lower()

        if not filter_text:
            self._filtered_devices = list(self._devices)
        else:
            self._filtered_devices = [
                d
                for d in self._devices
                if filter_text in d.name.lower() or filter_text in d.address.lower()
            ]

        self._update_device_listbox()

    def _update_device_listbox(self) -> None:
        """Update the device listbox with filtered devices."""
        self._device_listbox.delete(0, tk.END)

        for device in self._filtered_devices:
            display_text = f"{device.name} [{device.address}]"
            self._device_listbox.insert(tk.END, display_text)

        if self._selected_device not in self._filtered_devices:
            self._selected_device = None
            if not self._connected:
                self._connect_btn.configure(state="disabled")

    def add_device(self, device: DiscoveredDevice) -> None:
        """Add a discovered device to the list."""
        if any(d.address == device.address for d in self._devices):
            return

        self._devices.append(device)
        self._apply_filter()

    def clear_devices(self) -> None:
        """Clear the device list."""
        self._devices.clear()
        self._filtered_devices.clear()
        self._selected_device = None
        self._device_listbox.delete(0, tk.END)
        if not self._connected:
            self._connect_btn.configure(state="disabled")

    def set_scanning(self, scanning: bool) -> None:
        """Update scanning state."""
        self._scanning = scanning
        self._scan_btn.configure(text="Stop Scan" if scanning else "Scan")

    def set_connected(self, connected: bool) -> None:
        """Update connected state."""
        self._connected = connected
        self._connect_btn.configure(
            text="Disconnect" if connected else "Connect",
            state="normal" if connected or self._selected_device else "disabled",
        )
        status_text = "Connected" if connected else "Disconnected"
        status_color = "#00ff00" if connected else "#aaaaaa"
        self._status_label.configure(text=f"Status: {status_text}", text_color=status_color)

        if connected:
            self._simulate_btn.configure(state="disabled")
        else:
            self._simulate_btn.configure(state="normal")

    def set_simulating(self, simulating: bool) -> None:
        """Update simulation state."""
        self._simulating = simulating
        self._simulate_btn.configure(
            text="Stop Simulation" if simulating else "Simulate",
            fg_color="#6a2a2a" if simulating else "#2a6a2a",
            hover_color="#8a3a3a" if simulating else "#3a8a3a",
        )

        if simulating:
            self._scan_btn.configure(state="disabled")
            self._connect_btn.configure(state="disabled")
        else:
            self._scan_btn.configure(state="normal")
            if self._selected_device or self._connected:
                self._connect_btn.configure(state="normal")

    def append_status(self, message: str) -> None:
        """Append a message to the status log."""
        self._status_text.configure(state=tk.NORMAL)
        self._status_text.insert(tk.END, message + "\n")
        self._status_text.see(tk.END)
        self._status_text.configure(state=tk.DISABLED)

    def clear_status(self) -> None:
        """Clear the status log."""
        self._status_text.configure(state=tk.NORMAL)
        self._status_text.delete(1.0, tk.END)
        self._status_text.configure(state=tk.DISABLED)
