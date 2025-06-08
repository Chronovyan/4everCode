# First Steps with Chronovyan

Welcome to Chronovyan! This guide will help you take your first steps with the language.

## Your First Chronovyan Program

Let's create a simple Chronovyan program to get started:

```python
import chronovyan as cv

# Create a new timeline
timeline = cv.Timeline()

# Add an event to the timeline
timeline.add_event("Hello, Chronovyan!")

# Run the timeline
timeline.run()
```

## Core Concepts

### 1. Timelines

A `Timeline` is a sequence of events that occur over time. You can add events to a timeline and then execute them in order.

```python
timeline = cv.Timeline()

# Add events with delays
timeline.add_event("First event", delay=1.0)  # 1 second delay
timeline.add_event("Second event")           # No delay

# Run the timeline
timeline.run()
```

### 2. Events

Events are the building blocks of Chronovyan programs. They can be simple strings or more complex objects.

```python
# Simple event
event1 = cv.Event("A simple event")

# Event with data
event2 = cv.Event("Event with data", data={"key": "value"})

# Add events to timeline
timeline.add_event(event1)
timeline.add_event(event2)
```

### 3. Callbacks

You can attach callbacks to events to perform actions when they occur.

```python
def on_event(event):
    print(f"Event occurred: {event.name}")
    print(f"Event data: {event.data}")

# Create an event with a callback
event = cv.Event("Event with callback", on_trigger=on_event)
timeline.add_event(event)
```

## Next Steps

Now that you've taken your first steps with Chronovyan, you can explore:

- [Examples](../examples/index.md) - Check out our examples section for more advanced features and techniques
- [API Reference](../reference/index.md) - Detailed documentation of all classes and methods
- [Core Concepts](../concepts/index.md) - Deep dive into Chronovyan's architecture

## Need Help?

If you have any questions or run into issues, feel free to [open an issue](https://github.com/Chronovyan/Chronovyan.github.io/issues) on GitHub.
