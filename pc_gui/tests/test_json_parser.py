"""Tests for the JSON stream parser."""

import pytest

from src.parser.json_stream import JSONStreamParser, MeterReading


class TestJSONStreamParser:
    """Tests for JSONStreamParser."""

    def test_simple_message(self):
        """Test parsing a simple complete message."""
        readings = []
        parser = JSONStreamParser(on_reading=readings.append)

        result = parser.feed(b'{"Meter":{"ohm":1000}}\n')

        assert len(result) == 1
        assert result[0].ohms == 1000
        assert len(readings) == 1

    def test_message_with_crlf(self):
        """Test parsing message with CRLF line ending."""
        parser = JSONStreamParser()

        result = parser.feed(b'{"Meter":{"ohm":500}}\r\n')

        assert len(result) == 1
        assert result[0].ohms == 500

    def test_fragmented_message(self):
        """Test parsing a message split across multiple feeds."""
        parser = JSONStreamParser()

        result1 = parser.feed(b'{"Meter":')
        assert len(result1) == 0

        result2 = parser.feed(b'{"ohm":2500}}\n')
        assert len(result2) == 1
        assert result2[0].ohms == 2500

    def test_multiple_messages_single_feed(self):
        """Test parsing multiple messages in a single feed."""
        parser = JSONStreamParser()

        data = b'{"Meter":{"ohm":100}}\n{"Meter":{"ohm":200}}\n{"Meter":{"ohm":300}}\n'
        result = parser.feed(data)

        assert len(result) == 3
        assert result[0].ohms == 100
        assert result[1].ohms == 200
        assert result[2].ohms == 300

    def test_combined_and_fragmented(self):
        """Test combination of complete and partial messages."""
        parser = JSONStreamParser()

        result1 = parser.feed(b'{"Meter":{"ohm":100}}\n{"Meter":{"ohm":')
        assert len(result1) == 1
        assert result1[0].ohms == 100

        result2 = parser.feed(b'200}}\n')
        assert len(result2) == 1
        assert result2[0].ohms == 200

    def test_float_value(self):
        """Test parsing float resistance values."""
        parser = JSONStreamParser()

        result = parser.feed(b'{"Meter":{"ohm":1234.567}}\n')

        assert len(result) == 1
        assert result[0].ohms == pytest.approx(1234.567)

    def test_zero_value(self):
        """Test parsing zero resistance."""
        parser = JSONStreamParser()

        result = parser.feed(b'{"Meter":{"ohm":0}}\n')

        assert len(result) == 1
        assert result[0].ohms == 0

    def test_invalid_json(self):
        """Test handling of invalid JSON."""
        errors = []
        parser = JSONStreamParser(on_error=errors.append)

        result = parser.feed(b'not json\n')

        assert len(result) == 0
        assert len(errors) == 1
        assert "JSON parse error" in errors[0]

    def test_missing_meter_key(self):
        """Test handling of missing Meter key."""
        errors = []
        parser = JSONStreamParser(on_error=errors.append)

        result = parser.feed(b'{"ohm":1000}\n')

        assert len(result) == 0
        assert len(errors) == 1

    def test_missing_ohm_key(self):
        """Test handling of missing ohm key."""
        errors = []
        parser = JSONStreamParser(on_error=errors.append)

        result = parser.feed(b'{"Meter":{"resistance":1000}}\n')

        assert len(result) == 0
        assert len(errors) == 1

    def test_negative_value(self):
        """Test rejection of negative resistance values."""
        errors = []
        parser = JSONStreamParser(on_error=errors.append)

        result = parser.feed(b'{"Meter":{"ohm":-100}}\n')

        assert len(result) == 0
        assert len(errors) == 1
        assert "Negative" in errors[0]

    def test_infinite_value(self):
        """Test rejection of infinite values."""
        errors = []
        parser = JSONStreamParser(on_error=errors.append)

        result = parser.feed(b'{"Meter":{"ohm":1e999}}\n')

        assert len(result) == 0
        assert len(errors) == 1

    def test_nan_string_value(self):
        """Test rejection of non-numeric values."""
        errors = []
        parser = JSONStreamParser(on_error=errors.append)

        result = parser.feed(b'{"Meter":{"ohm":"abc"}}\n')

        assert len(result) == 0
        assert len(errors) == 1

    def test_empty_lines_ignored(self):
        """Test that empty lines are ignored."""
        parser = JSONStreamParser()

        result = parser.feed(b'\n\n{"Meter":{"ohm":100}}\n\n')

        assert len(result) == 1
        assert result[0].ohms == 100

    def test_reset_clears_buffer(self):
        """Test that reset clears the internal buffer."""
        parser = JSONStreamParser()

        parser.feed(b'{"Meter":{"ohm":')
        parser.reset()

        result = parser.feed(b'{"Meter":{"ohm":100}}\n')
        assert len(result) == 1
        assert result[0].ohms == 100

    def test_unicode_handling(self):
        """Test proper UTF-8 handling."""
        parser = JSONStreamParser()

        result = parser.feed('{"Meter":{"ohm":100}}\n'.encode("utf-8"))

        assert len(result) == 1
        assert result[0].ohms == 100

    def test_large_value(self):
        """Test parsing large resistance values."""
        parser = JSONStreamParser()

        result = parser.feed(b'{"Meter":{"ohm":5000000}}\n')

        assert len(result) == 1
        assert result[0].ohms == 5000000

    def test_very_small_value(self):
        """Test parsing very small resistance values."""
        parser = JSONStreamParser()

        result = parser.feed(b'{"Meter":{"ohm":0.001}}\n')

        assert len(result) == 1
        assert result[0].ohms == pytest.approx(0.001)
