"""
Tests for the Chronovyan timeline implementation.
"""
import time
import pytest
from chronovyan import Timeline, Event

class TestTimeline:
    """Test cases for the Timeline class."""
    
    def test_create_timeline(self):
        """Test creating a new timeline."""
        timeline = Timeline()
        assert timeline is not None
        assert timeline.event_count == 0
        assert not timeline.is_running
    
    def test_add_event(self):
        """Test adding an event to the timeline."""
        timeline = Timeline()
        event = Event("test_event")
        
        timeline.add_event(event)
        assert timeline.event_count == 1
    
    def test_run_empty_timeline(self):
        """Test running an empty timeline."""
        timeline = Timeline()
        timeline.run()  # Should complete immediately
        assert not timeline.is_running
    
    def test_event_triggering(self):
        """Test that events are triggered correctly."""
        triggered = []
        
        def on_event(event):
            triggered.append(event.name)
        
        timeline = Timeline()
        event1 = Event("event1", on_trigger=on_event)
        event2 = Event("event2", on_trigger=on_event)
        
        timeline.add_event(event1)
        timeline.add_event(event2, delay=0.1)
        
        timeline.run()
        
        assert len(triggered) == 2
        assert "event1" in triggered
        assert "event2" in triggered
    
    def test_event_ordering(self):
        """Test that events are executed in the correct order."""
        order = []
        
        def on_event(event):
            order.append(event.name)
        
        timeline = Timeline()
        
        # Add events with different delays
        timeline.add_event(Event("first", on_trigger=on_event), delay=0.2)
        timeline.add_event(Event("second", on_trigger=on_event), delay=0.1)
        timeline.add_event(Event("third", on_trigger=on_event))
        
        timeline.run()
        
        # Check order of execution
        assert order == ["third", "second", "first"]
    
    def test_stop_timeline(self):
        """Test stopping the timeline before completion."""
        def long_running():
            time.sleep(10)  # This would block for a long time
        
        timeline = Timeline()
        event = Event("long", on_trigger=lambda e: long_running())
        timeline.add_event(event)
        
        # Run in a separate thread so we can stop it
        import threading
        thread = threading.Thread(target=timeline.run)
        thread.start()
        
        # Give it a moment to start
        time.sleep(0.1)
        
        # Stop the timeline
        timeline.stop()
        thread.join(timeout=1.0)
        
        assert not timeline.is_running

class TestEvent:
    """Test cases for the Event class."""
    
    def test_create_event(self):
        """Test creating a new event."""
        event = Event("test")
        assert event.name == "test"
        assert not event.is_triggered
        assert event.timestamp is None
    
    def test_trigger_event(self):
        """Test triggering an event."""
        callback_called = False
        
        def callback(event):
            nonlocal callback_called
            callback_called = True
            assert event.name == "test"
        
        event = Event("test", on_trigger=callback)
        event.trigger()
        
        assert callback_called
        assert event.is_triggered
        assert event.timestamp is not None
    
    def test_event_data(self):
        """Test event data storage and retrieval."""
        data = {"key": "value", "count": 42}
        event = Event("test", data=data)
        
        assert event.data == data
        event.data["new_key"] = "new_value"
        assert event.data["new_key"] == "new_value"
    
    def test_event_string_representation(self):
        """Test the string representation of an event."""
        event = Event("test", data={"key": "value"})
        
        # Before triggering
        assert "status='pending'" in str(event)
        
        # After triggering
        event.trigger()
        assert "status='triggered'" in str(event)
