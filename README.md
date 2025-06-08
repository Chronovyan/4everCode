<div align="center">
  <h1>⏳ Chronovyan</h1>
  <h3>A Temporal Programming Library for Python</h3>
  
  [![PyPI](https://img.shields.io/pypi/v/chronovyan)](https://pypi.org/project/chronovyan/)
  [![Documentation Status](https://img.shields.io/badge/docs-latest-brightgreen.svg)](https://chronovyan.github.io/)
  [![Jekyll site CI](https://github.com/Chronovyan/Chronovyan.github.io/actions/workflows/jekyll.yml/badge.svg)](https://github.com/Chronovyan/Chronovyan.github.io/actions/workflows/jekyll.yml)
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
  [![Python Version](https://img.shields.io/pypi/pyversions/chronovyan)](https://pypi.org/project/chronovyan/)
  [![Tests](https://github.com/Chronovyan/Chronovyan/actions/workflows/tests.yml/badge.svg)](https://github.com/Chronovyan/Chronovyan/actions)
  
  *"Time is the canvas, and we are the weavers of its threads."* - Ancient Weaver Proverb
</div>

## 🌟 About Chronovyan

Chronovyan is a Python library that provides a powerful framework for working with time as a first-class construct. It enables developers to create, manage, and synchronize events across different points in time with an intuitive and Pythonic API.

### Key Features

- **Temporal Programming Model**: Work with time as a primary construct
- **Event Management**: Schedule and manage events with precision
- **Time Manipulation**: Control the flow of time in your applications
- **Thread-Safe**: Built with concurrency in mind
- **Extensible**: Create custom event types and timelines
- **Zero Dependencies**: Pure Python with no external dependencies
- **Type Annotations**: Full support for static type checking

## 📚 Documentation

Comprehensive documentation is available at [https://chronovyan.github.io/](https://chronovyan.github.io/).

### Quick Links

- [Getting Started](https://chronovyan.github.io/getting-started/installation/)
- [API Reference](https://chronovyan.github.io/reference/)
- [Examples](https://github.com/Chronovyan/Chronovyan/tree/main/examples)

## 🚀 Quick Start

### Prerequisites

- Python 3.8 or higher
- pip (Python package manager)

### Installation

```bash
# Install from PyPI
pip install chronovyan

# Or install in development mode from source
git clone https://github.com/Chronovyan/Chronovyan.git
cd Chronovyan
pip install -e .
```

### Basic Usage

```python
import time
from chronovyan import Timeline, Event

def on_event(event):
    print(f"{event.name} at {event.timestamp:.2f} seconds")

# Create a timeline
timeline = Timeline()

# Add some events
timeline.add_event(Event("Hello", on_trigger=on_event))
timeline.add_event(Event("World", delay=1.0, on_trigger=on_event))

# Run the timeline
timeline.run()
```

### Advanced Example: Task Dependencies

```python
from chronovyan import Timeline, Event

def create_task(name, duration, dependencies=None):
    def task_handler(event):
        print(f"🚀 Starting {name} (takes {duration}s)")
        
        def on_complete():
            print(f"✅ Completed {name}")
            
            # Trigger dependent tasks
            for dep in dependencies or []:
                if not dep.is_triggered:
                    dep.trigger()
        
        # Schedule completion
        timeline.add_event(Event(f"Complete {name}", on_trigger=on_complete), delay=duration)
    
    return Event(f"Start {name}", on_trigger=task_handler)

# Create tasks with dependencies
timeline = Timeline()
task_a = create_task("Task A", 1.0)
task_b = create_task("Task B", 2.0, [task_a])
task_c = create_task("Task C", 1.5, [task_a])
task_d = create_task("Task D", 1.0, [task_b, task_c])

# Start with Task A
timeline.add_event(task_a)
timeline.run()
```

## 🔍 Examples

Check out the [examples](examples/) directory for more usage examples:

1. [Basic Usage](examples/basic_usage.py) - Introduction to core concepts
2. [Event Synchronization](examples/event_synchronization.py) - Managing task dependencies

## 🧪 Testing

To run the test suite:

```bash
# Install test dependencies
pip install -r requirements-dev.txt

# Run tests
pytest tests/
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details on how to get started.

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📞 Support

For help and support:
- Check the [documentation](https://chronovyan.github.io/)
- Open an [issue](https://github.com/Chronovyan/Chronovyan/issues)
- Join our [Discord community](https://discord.gg/chronovyan)

## 🙏 Acknowledgments

- All contributors who have helped shape Chronovyan
- The open-source community for inspiration and support