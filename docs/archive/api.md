---
title: API Reference
description: Comprehensive API documentation for Chronovyan
---

# API Reference

## Core Classes

### Timeline

```python
class Timeline:
    """A sequence of events that occur over time.
    
    The Timeline class allows you to schedule events to occur at specific times
    or after specific delays, and then run them in the correct order.
    """
    
    def __init__(self):
        """Initialize a new, empty timeline."""
        ...
    
    def add_event(self, event: 'Event', delay: float = 0.0) -> None:
        """Add an event to the timeline.
        
        Args:
            event: The event to add
            delay: Number of seconds to wait before the event occurs
        """
        ...
    
    def run(self, max_time: Optional[float] = None) -> None:
        """Run the timeline, executing events in order.
        
        Args:
            max_time: Maximum time to run the timeline (in seconds).
                     If None, runs until all events are processed.
        """
        ...
    
    def pause(self) -> None:
        """Pause the timeline."""
        ...
    
    def resume(self) -> None:
        """Resume a paused timeline."""
        ...
    
    def stop(self) -> None:
        """Stop the timeline immediately."""
        ...
    
    @property
    def is_running(self) -> bool:
        """Check if the timeline is currently running."""
        ...
    
    @property
    def current_time(self) -> float:
        """Get the current simulation time (in seconds since start)."""
        ...

class Event:
    """An event that can be scheduled on a timeline.
    
    Args:
        name: A name for the event (for debugging and logging)
        data: Optional data to associate with the event
        on_trigger: Optional callback function to call when the event is triggered
        delay: Optional delay in seconds before the event occurs
    """
    
    def __init__(self, name: str, data: Any = None, 
                 on_trigger: Optional[Callable[['Event'], None]] = None,
                 delay: float = 0.0):
        ...
    
    def trigger(self) -> None:
        """Trigger the event, calling the on_trigger callback if set."""
        ...
    
    @property
    def is_triggered(self) -> bool:
        """Check if the event has been triggered."""
        ...
    
    @property
    def timestamp(self) -> Optional[float]:
        """Get the time when the event was triggered, or None if not triggered yet."""
        ...
    
    @property
    def age(self) -> Optional[float]:
        """Get the time since the event was triggered, or None if not triggered yet."""
        ...
            The current simulation time in seconds.
        """
        ...
    
    @property
    def is_running(self) -> bool:
        """Check if the timeline is currently running.
        
        Returns:
            True if the timeline is running, False otherwise.
        """
        ...

### Event

```python
class Event:
    """Represents an event that occurs at a specific time.
    
    Events are the basic building blocks of a timeline. Each event has a timestamp
    and a callback function that gets called when the event is processed.
    """
    
    def __init__(self, timestamp: float, callback: Callable, data: Any = None):
        """Initialize a new Event.
        
        Args:
            timestamp: When the event should occur (in simulation time).
            callback: Function to call when the event is processed.
            data: Optional data to pass to the callback.
        """
        ...
    
    def trigger(self) -> None:
        """Trigger the event's callback."""
        ...
    
    @property
    def timestamp(self) -> float:
        """Get the event's timestamp."""
        ...
    
    @property
    def data(self) -> Any:
        """Get the event's data."""
        ...
```

## CLI Reference

Chronovyan provides a command-line interface for basic timeline operations:

```bash
# Show help
chronovyan --help

# Create and run a simple timeline
chronovyan run --event "0.5:print('Hello after 0.5s')" --event "1.0:print('Hello after 1.0s')"

# Run a timeline from a JSON file
chronovyan run --file timeline.json

# List available commands
chronovyan --help
```

## Example Usage

### Creating a Timeline

```python
from chronovyan import Timeline, Event

def callback(data):
    print(f"Event triggered with data: {data}")

timeline = Timeline()
event1 = Event(1.0, callback, "First event")
event2 = Event(2.0, callback, "Second event")

timeline.add_event(event2)
timeline.add_event(event1)  # Events will be processed in timestamp order

timeline.run()
```

### Using Dependencies Between Events

```python
from chronovyan import Timeline, Event

def setup_event(timeline):
    def callback(data):
        print(f"Processing {data}")
        # Schedule another event
        timeline.add_event(Event(timeline.current_time + 1.0, callback, f"Child of {data}"))
    
    return Event(0.0, callback, "Initial event")

timeline = Timeline()
timeline.add_event(setup_event(timeline))
timeline.run(until=5.0)  # Run for 5 seconds of simulation time
```

## Type Definitions

### Callback Protocol

```python
from typing import Protocol, Any, Optional

class EventCallback(Protocol):
    def __call__(self, data: Any) -> None:
        """Callback function type for event processing.
        
        Args:
            data: The data associated with the event.
        """
        ...
```

## Constants

- `VERSION`: Current version of the Chronovyan library
- `DEFAULT_TIMESTEP`: Default time step used for simulation (0.01 seconds)

## Exceptions

### ChronovyanError

Base class for all Chronovyan-specific exceptions.

### TimelineError

Raised for errors related to timeline operations.

### EventError

Raised for errors related to event operations.
