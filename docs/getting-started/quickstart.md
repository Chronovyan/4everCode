# Quick Start Guide

This guide will help you get up and running with Chronovyan as quickly as possible.

## Installation

```bash
pip install chronovyan
```

## Basic Usage

### 1. Import Chronovyan

```python
import chronovyan as cv
```

### 2. Create a Timeline

```python
timeline = cv.Timeline()
```

### 3. Add Events

```python
# Add a simple event
timeline.add_event("Hello, World!")

# Add an event with a delay
timeline.add_event("This happens after 1 second", delay=1.0)
```

### 4. Run the Timeline

```python
timeline.run()
```

## Example: Simple Animation

Here's a simple example that creates a timeline with multiple events:

```python
import chronovyan as cv

def print_message(event):
    print(f"At {event.timestamp:.2f}s: {event.name}")

# Create a new timeline
timeline = cv.Timeline()

# Add some events
timeline.add_event(cv.Event("Start", on_trigger=print_message))
timeline.add_event(cv.Event("Processing...", delay=0.5, on_trigger=print_message))
timeline.add_event(cv.Event("Almost done", delay=1.0, on_trigger=print_message))
timeline.add_event(cv.Event("Complete!", delay=1.5, on_trigger=print_message))

# Run the timeline
print("Starting timeline...")
timeline.run()
print("Timeline complete!")
```

## Next Steps

- Learn more in the [First Steps](first-steps.md) guide
- Explore the [API Reference](../reference/index.md)
- Check out the [examples](../examples/index.md) for more complex use cases
