---
title: API Reference
description: Comprehensive API documentation for Chronovyan
---

# API Reference

## Core Classes

### Timeline

```python
class Timeline:
    """A timeline manages and executes events in temporal order.
    
    The Timeline class is the main interface for scheduling and running events.
    It maintains a queue of events and processes them in chronological order.
    """
    
    def __init__(self):
        """Initialize a new Timeline instance."""
        ...
    
    def add_event(self, event: Event) -> None:
        """Add an event to the timeline.
        
        Args:
            event: The Event instance to add to the timeline.
            
        Raises:
            ValueError: If the event is already added to a timeline.
        """
        ...
    
    def run(self, until: Optional[float] = None) -> None:
        """Run the timeline, executing events in order.
        
        Args:
            until: Optional timestamp to run until. If None, runs until no events remain.
        """
        ...
    
    def stop(self) -> None:
        """Stop the timeline execution."""
        ...
    
    @property
    def current_time(self) -> float:
        """Get the current simulation time.
        
        Returns:
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
