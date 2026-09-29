#!/usr/bin/env python3
"""BLE Ohmmeter application entry point."""

import asyncio
import logging
import sys
import threading
from pathlib import Path

from src.gui.main_window import MainWindow

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
)
logger = logging.getLogger(__name__)


def run_event_loop(loop: asyncio.AbstractEventLoop) -> None:
    """Run the asyncio event loop in a separate thread."""
    asyncio.set_event_loop(loop)
    loop.run_forever()


def main() -> int:
    """Application entry point."""
    logger.info("Starting BLE Ohmmeter application")

    data_dir = Path(__file__).parent

    loop = asyncio.new_event_loop()

    loop_thread = threading.Thread(target=run_event_loop, args=(loop,), daemon=True)
    loop_thread.start()

    app = MainWindow(data_dir=data_dir)
    app.set_event_loop(loop)

    try:
        app.mainloop()
    except KeyboardInterrupt:
        logger.info("Application interrupted")
    finally:
        loop.call_soon_threadsafe(loop.stop)
        loop_thread.join(timeout=2.0)
        logger.info("Application closed")

    return 0


if __name__ == "__main__":
    sys.exit(main())
