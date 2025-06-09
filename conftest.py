"""Pytest configuration and fixtures for Forever tests."""
import pytest
from unittest.mock import Mock

@pytest.fixture
def mock_event():
    """Create a mock event for testing."""
    return Mock()

@pytest.fixture
def timeline():
    """Create a Timeline instance for testing."""
    from Forever import Timeline
    return Timeline()

@pytest.fixture
def event_factory():
    """Factory to create events with custom parameters."""
    def _event_factory(name="test_event", data=None, on_trigger=None, delay=0.0):
        from Forever import Event
        return Event(name=name, data=data, on_trigger=on_trigger, delay=delay)
    return _event_factory
