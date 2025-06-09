# API Reference

This section provides detailed documentation for the Chronovyan API.

## Core Classes

### Timeline

The main class for managing a sequence of events.

#### Methods

- `add_event(event, delay=0.0)`: Add an event to the timeline
- `run()`: Execute all events in the timeline
- `pause()`: Pause the timeline
- `resume()`: Resume a paused timeline
- `stop()`: Stop the timeline execution

#### Properties

- `is_running`: Whether the timeline is currently running
- `current_time`: The current time in the timeline
- `event_count`: Number of events in the timeline

### Event

Represents an event in the timeline.

#### Constructor

```python
Event(name, data=None, on_trigger=None, delay=0.0)
```

#### Properties

- `name`: The name of the event
- `data`: Optional data associated with the event
- `timestamp`: When the event is scheduled to occur
- `is_triggered`: Whether the event has been triggered

## Utility Functions

### Time Utilities

- `get_current_time()`: Get the current time in the timeline
- `sleep(seconds)`: Sleep for the specified number of seconds

## Examples

### Creating and Running a Timeline

```python
import chronovyan as cv

def on_event(event):
    print(f"Event: {event.name}")

# Create a timeline
timeline = cv.Timeline()

# Add some events
timeline.add_event(cv.Event("Start", on_trigger=on_event))
timeline.add_event(cv.Event("Middle", on_trigger=on_event, delay=1.0))
timeline.add_event(cv.Event("End", on_trigger=on_event, delay=2.0))

# Run the timeline
timeline.run()
```

## Advanced Topics

### Custom Events

You can create custom event types by subclassing `Event`:

```python
class CustomEvent(cv.Event):
    def __init__(self, name, custom_data, **kwargs):
        super().__init__(name, **kwargs)
        self.custom_data = custom_data
    
    def trigger(self):
        print(f"Custom event triggered with data: {self.custom_data}")
        super().trigger()

# Usage
event = CustomEvent("Custom", custom_data=42, delay=1.0)
timeline.add_event(event)
```

## Next Steps

- [Getting Started](../getting-started/installation.md) - Set up Chronovyan
- [Core Concepts](../concepts/index.md) - Learn about Chronovyan's architecture
- [Examples](../examples/index.md) - Practical examples of Chronovyan in action
