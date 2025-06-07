# Core Concepts

This section covers the fundamental concepts behind Chronovyan and how they work together.

## Table of Contents

- [Timelines](#timelines)
- [Events](#events)
- [Callbacks](#callbacks)
- [Synchronization](#synchronization)
- [Time Management](#time-management)

## Timelines

A `Timeline` is the central concept in Chronovyan. It represents a sequence of events that occur over time. You can think of it as a timeline where events are scheduled to happen at specific times.

Key features of Timelines:

- **Ordered Execution**: Events are executed in chronological order
- **Time Management**: Supports both relative and absolute timing
- **Thread Safety**: Can be accessed from multiple threads safely

## Events

Events are the building blocks of Chronovyan programs. They represent something that happens at a specific point in time.

Types of events:

1. **Basic Events**: Simple events with a name and optional data
2. **Timed Events**: Events that occur after a delay or at a specific time
3. **Recurring Events**: Events that repeat at regular intervals

## Callbacks

Callbacks are functions that get called when an event occurs. They allow you to execute custom code in response to events.

Example:

```python
def on_event(event):
    print(f"Event occurred: {event.name}")

event = cv.Event("Test Event", on_trigger=on_event)
```

## Synchronization

Chronovyan provides tools for synchronizing events across different parts of your application:

- **Barriers**: Synchronize multiple timelines
- **Conditions**: Wait for specific conditions to be met
- **Locks**: Protect shared resources

## Time Management

Chronovyan provides flexible time management:

- **Wall Time**: Real-world time
- **Simulation Time**: Virtual time for testing
- **Time Scaling**: Speed up or slow down time

## Next Steps

- [Getting Started](../getting-started/installation.md) - Set up Chronovyan
- [API Reference](../reference/index.md) - Detailed API documentation
- [Examples](../examples/index.md) - Practical examples of Chronovyan in action
