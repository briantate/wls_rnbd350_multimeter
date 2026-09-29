# BLE Ohmmeter GUI

A Python desktop application for displaying resistance measurements from a BLE-connected ohmmeter device. Features a classic analog swing meter with smooth needle animation and auto-ranging display.

## Features

- **BLE Connectivity**: Scan, connect, and receive data from Microchip Transparent UART devices
- **Analog Meter Display**: Semicircular gauge with animated needle and auto-ranging
- **Digital Readout**: Large digital display with automatic Ω/kΩ/MΩ formatting
- **Auto-Ranging**: Automatically selects 1kΩ, 10kΩ, 100kΩ, or 1MΩ range with hysteresis
- **Device Filtering**: Real-time filtering of discovered BLE devices by name or address
- **Simulation Mode**: Test the display without hardware using sample data
- **Status Logging**: Complete BLE event and data logging for debugging

## Requirements

- Python 3.11 or later
- Bluetooth adapter with BLE support
- Windows, macOS, or Linux

## Setup

### 1. Create Virtual Environment

**Windows (PowerShell):**
```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
```

**Windows (Command Prompt):**
```cmd
python -m venv .venv
.venv\Scripts\activate.bat
```

**macOS/Linux:**
```bash
python3 -m venv .venv
source .venv/bin/activate
```

### 2. Install Dependencies

```bash
pip install -r requirements.txt
```

## Running the Application

From the `pc_gui` directory with the virtual environment activated:

```bash
python main.py
```

## Usage

### BLE Connection

1. Click **Scan** to discover nearby BLE devices
2. Use the **Device Filter** field to filter devices by name or Bluetooth address
3. Select a device from the list
4. Click **Connect** to establish a connection
5. The meter will automatically display received resistance values

### Simulation Mode

1. Click **Simulate** to play back the sample data file
2. The simulation runs for 10 seconds, demonstrating all meter ranges
3. Click **Stop Simulation** to end early

### Status Log

- View all BLE events and raw data in the scrollable status log
- Click **Clear** to reset the log

## UUID Configuration

The application uses Microchip Transparent UART service UUIDs. To modify for your device, edit `src/ble/uuids.py`:

```python
class UUIDs:
    SERVICE: str = "49535343-fe7d-4ae5-8fa9-9fafd205e455"
    TX_CHAR: str = "49535343-1e4d-4bd9-ba61-23c647249616"
    RX_CHAR: str = "49535343-8841-43f4-a8d4-ecbe34729bb3"
```

## Data Format

The device should send UTF-8 JSON messages terminated by newline (`\n`):

```json
{"Meter":{"ohm":10000}}
```

- `ohm` value can be integer or float
- Values must be finite and non-negative
- Carriage returns (`\r`) are ignored

## Meter Ranges

| Range | Display | Full Scale |
|-------|---------|------------|
| 1 kΩ  | 0-1000 Ω | 1,000 Ω |
| 10 kΩ | 0-10 kΩ | 10,000 Ω |
| 100 kΩ | 0-100 kΩ | 100,000 Ω |
| 1 MΩ  | 0-1 MΩ | 1,000,000 Ω |

Values above 1 MΩ display "OVER RANGE" with the actual value shown digitally.

## BLE Permissions

### Windows
Bluetooth should work without additional configuration on Windows 10/11.

### macOS
Grant Bluetooth permission when prompted. If issues occur, check System Preferences > Security & Privacy > Privacy > Bluetooth.

### Linux
You may need to run with elevated permissions or configure BlueZ:

```bash
# Option 1: Run with sudo (not recommended for production)
sudo python main.py

# Option 2: Add user to bluetooth group
sudo usermod -aG bluetooth $USER
# Log out and back in

# Option 3: Set capabilities on Python
sudo setcap 'cap_net_raw,cap_net_admin+eip' $(readlink -f $(which python))
```

## Running Tests

```bash
pytest tests/ -v
```

## Troubleshooting

### "Transparent UART service not found"
- Verify the device advertises the expected service UUID
- Check that the device is in the correct mode
- Update UUIDs in `src/ble/uuids.py` if using a different service

### Connection Timeouts
- Ensure the device is powered on and advertising
- Move closer to the device
- Check that no other application is connected to the device

### No Devices Discovered
- Verify Bluetooth is enabled on your computer
- Check BLE permissions (see above)
- Ensure the device is advertising

### Simulation Not Working
- Verify `sample_resistance.jsonl` exists in the `pc_gui` directory
- Check the status log for error messages

## Project Structure

```
pc_gui/
├── main.py                    # Application entry point
├── requirements.txt           # Python dependencies
├── sample_resistance.jsonl    # Sample data for simulation
├── README.md                  # This file
├── src/
│   ├── gui/
│   │   ├── main_window.py     # Main application window
│   │   ├── gauge_widget.py    # Analog meter widget
│   │   └── controls_panel.py  # BLE controls panel
│   ├── ble/
│   │   ├── manager.py         # BLE connection manager
│   │   └── uuids.py           # UUID configuration
│   ├── parser/
│   │   └── json_stream.py     # JSON stream parser
│   ├── logic/
│   │   └── range_selector.py  # Auto-range logic
│   └── simulation/
│       └── controller.py      # Simulation controller
└── tests/
    ├── test_json_parser.py    # Parser tests
    ├── test_range_selector.py # Range logic tests
    └── test_simulation.py     # Simulation tests
```

## License

MIT License - See LICENSE file for details.
