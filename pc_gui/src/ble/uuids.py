"""UUID configuration for Microchip Transparent UART service."""


class UUIDs:
    """Configurable UUIDs for the Microchip Transparent UART BLE service."""

    # Microchip Transparent UART Service UUID
    # TODO: Replace with actual device-specific UUID if different
    SERVICE: str = "49535343-fe7d-4ae5-8fa9-9fafd205e455"

    # TX Characteristic UUID (device transmits, we receive/notify)
    # TODO: Replace with actual device-specific UUID if different
    TX_CHAR: str = "49535343-1e4d-4bd9-ba61-23c647249616"

    # RX Characteristic UUID (we write, device receives)
    # TODO: Replace with actual device-specific UUID if different
    RX_CHAR: str = "49535343-8841-43f4-a8d4-ecbe34729bb3"
