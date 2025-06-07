---
title: Readme
description: Documentation for README
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chronovyan Python Examples

This directory contains example Python scripts that demonstrate how to use the Chronovyan library for temporal programming.

## Available Examples

1. **[Basic Usage](basic_usage.py)** - A simple introduction to Chronovyan's core concepts:
   - Creating a timeline
   - Adding events with different delays
   - Running the timeline and handling events

2. **[Event Synchronization](event_synchronization.py)** - Demonstrates how to:
   - Create dependent tasks
   - Manage task dependencies
   - Synchronize events based on completion of other events

## Running the Examples

### Prerequisites

Make sure you have Python 3.8 or later installed, then install Chronovyan in development mode:

```bash
# From the project root directory
pip install -e .
```

### Running an Example

To run an example, simply execute it with Python:

```bash
# Run the basic usage example
python examples/basic_usage.py

# Run the event synchronization example
python examples/event_synchronization.py
```

## Example Output

### Basic Usage Example

```
=== Chronovyan Basic Example ===

Adding events to the timeline...

Running timeline with 5 events...
(Press Ctrl+C to stop early)

Event triggered: First Event
  Timestamp: 0.00

Event triggered: Count 1
  Timestamp: 0.50
  Data: {'count': 1}

Event triggered: Count 2
  Timestamp: 1.00
  Data: {'count': 2}

Event triggered: Delayed Event
  Timestamp: 1.50
  Data: {'key': 'value'}

Event triggered: Count 3
  Timestamp: 1.50
  Data: {'count': 3}

Timeline completed in 1.50 seconds

Example completed!
```

### Event Synchronization Example

```
=== Chronovyan Event Synchronization Example ===

Task Dependencies:
- Task A (takes 1.0s) | Depends on: None
- Task B (takes 2.0s) | Depends on: Task A
- Task C (takes 1.5s) | Depends on: Task A
- Task D (takes 1.0s) | Depends on: Task B, Task C
- Task E (takes 0.5s) | Depends on: Task D

=== Task Execution ===
 [0.0s] Starting Task A (takes 1.0s)
 [1.0s] Completed Task A
 [1.0s] Starting Task B (takes 2.0s)
 [1.0s] Starting Task C (takes 1.5s)
 [2.5s] Completed Task C
 [3.0s] Completed Task B
 [3.0s] Starting Task D (takes 1.0s)
 [4.0s] Completed Task D
 [4.0s] Starting Task E (takes 0.5s)
 [4.5s] Completed Task E

All tasks completed in 4.50 seconds

Example completed!
```

## Creating Your Own Examples

To create your own examples, you can use the following template:

```python
from chronovyan import Timeline, Event

def main():
    # Create a new timeline
    timeline = Timeline()
    
    # Define a callback function for events
    def on_event(event):
        print(f"Event triggered: {event.name}")
    
    # Add events to the timeline
    timeline.add_event(Event("My Event", on_trigger=on_event))
    
    # Run the timeline
    timeline.run()

if __name__ == "__main__":
    main()
```

Feel free to explore more advanced features by checking the API documentation and the source code.

## Contributing

If you'd like to contribute new examples, please ensure they:
1. Are well-commented and follow our coding standards
2. Include a descriptive header explaining the purpose
3. Are placed in the appropriate difficulty level directory
4. Follow the numbering convention (XX_example_name.cvy)

## Resources

For more information about the Chronovyan language, see:
- [Chronovyan Language Documentation](https://github.com/Chronovyan/docs)
- [Chronovyan Playground](https://play.chronovyan.io)
- [Chronovyan Discord Community](https://discord.gg/chronovyan) 