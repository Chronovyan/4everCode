<div class="mdx-hero" style="text-align: center; margin: 2em 0 4em 0;">
    <h1>⏳ Chronovyan</h1>
    <p class="lead">A temporal programming language for weaving narratives across time</p>
    <p>
        <a href="getting-started/installation/" class="md-button md-button--primary">
            Get Started
        </a>
        <a href="examples/" class="md-button">
            View Examples
        </a>
    </p>
</div>

## 🚀 Features

<div class="grid cards" style="display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 1.5rem; margin: 2em 0;">

- **Temporal Programming**
  - Work with time as a first-class construct
  - Schedule and manage events with precision
  - Control the flow of time in your applications

- **Powerful API**
  - Intuitive and Pythonic interface
  - Thread-safe design
  - Extensible architecture

- **Comprehensive Documentation**
  - Getting started guides
  - API reference
  - Practical examples

</div>

## 📖 Quick Start

1. **Install Chronovyan**
   ```bash
   pip install chronovyan
   ```

2. **Create your first timeline**
   ```python
   from chronovyan import Timeline, Event
   
   def log_event(data):
       print(f"[{timeline.current_time:.2f}s] {data}")
   
   # Create and run a timeline
   timeline = Timeline()
   timeline.add_event(Event(1.0, log_event, "Hello, Chronovyan!"))
   timeline.run()
   ```

3. **Explore the documentation**
   - [Installation Guide](getting-started/installation.md)
   - [First Steps](getting-started/first-steps.md)
   - [Examples](examples.md)

## 🎯 Use Cases

- **Game Development**: Manage game loops, animations, and timed events
- **Simulations**: Model complex temporal systems
- **Data Processing**: Schedule and coordinate data pipelines
- **Interactive Applications**: Create responsive UIs with timed interactions

## 🌟 Why Chronovyan?

- **Intuitive API**: Designed to be easy to use and understand
- **Flexible**: Suitable for a wide range of applications
- **Well-documented**: Comprehensive guides and API reference
- **Active Development**: Regular updates and improvements

## 📚 Documentation

Explore the documentation to learn more about Chronovyan:

- [Getting Started](getting-started/installation.md) - Set up Chronovyan and take your first steps
- [Examples](examples.md) - Practical examples and use cases
- [API Reference](api.md) - Comprehensive API documentation
- [Contributing](CONTRIBUTING.md) - How to contribute to Chronovyan

## 🤝 Contributing

We welcome contributions from the community! Whether you're fixing bugs, adding new features, or improving documentation, your help is appreciated.

[Learn how to contribute →](CONTRIBUTING.md)

## 📄 License

Chronovyan is licensed under the [MIT License](LICENSE).
