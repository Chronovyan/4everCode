<div class="mdx-hero" style="text-align: center; margin: 2em 0 4em 0;">
    <h1>⏳ Chronovyan</h1>
    <p class="lead">A C++ temporal programming language for time-aware applications</p>
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
  - First-class time types and operations
  - Native support for temporal logic
  - Deterministic execution model

- **Resource Management**
  - Built-in tracking of temporal resources
  - Efficient `Chronon` and `Aethel` management
  - Predictable resource allocation

- **Powerful Type System**
  - Static typing with type inference
  - Temporal type safety
  - Extensible type system

</div>

## 📖 Quick Start

1. **Build Chronovyan**
   ```bash
   # Clone the repository
   git clone https://github.com/Chronovyan/Chronovyan.git
   cd Chronovyan
   
   # Configure and build
   mkdir build && cd build
   cmake ..
   make
   ```

2. **Your First Chronovyan Program**
   ```chronovyan
   // hello_world.cvy
   MODULE hello_world;
   
   IMPORT std.io;
   
   FUNC VOID main() {
       io::println("Hello, Chronovyan!");
       
       // Basic temporal operation
       DECLARE CONF::STATIC start_time = NOW();
       DELAY 1.0s;  // Wait for 1 second
       DECLARE CONF::STATIC end_time = NOW();
       
       io::println("Elapsed: ", (end_time - start_time), " seconds");
   }
   ```

3. **Explore the Documentation**
   - [Installation Guide](getting-started/installation.md)
   - [Language Reference](reference/syntax.md)
   - [Examples](examples.md)

## Chronovyan: Temporal Programming Redefined

Chronovyan is a statically-typed, temporal programming language designed for building robust and efficient time-aware applications. It introduces first-class concepts of time, temporal resources, and timeline management, making it ideal for simulations, event-driven systems, and any application where time is a critical component.

## Core Philosophy

Chronovyan is built around the concept of **Temporal Programming**, where time is a first-class citizen. The language is designed with two fundamental principles:

1. **Order (Conformity)**: For stability and predictability
2. **Flux (Rebellion)**: For dynamic adaptation and temporal manipulation

## Key Features

- **Temporal Types**: Native support for time points, durations, and intervals
- **Resource Management**: Built-in tracking of temporal resources (`Chronon` and `Aethel`)
- **Deterministic Execution**: Predictable behavior for time-dependent operations
- **Concurrency Model**: Safe and efficient handling of temporal concurrency
- **Type Safety**: Strong static typing with type inference

## Getting Started

- [Installation Guide](getting-started/installation.md)
- [Quick Start](getting-started/quickstart.md)
- [Examples](examples.md)

## Core Concepts

- [Temporal Programming](concepts/temporal_programming.md)
- [Resource Management](concepts/resource_management.md)
- [Concurrency Model](concepts/concurrency.md)

## Language Reference

- [Language Specification](reference/language_specification.md)
- [API Reference](reference/api_reference.md)
- [Standard Library](reference/stdlib/index.md)

## Development

- [Contributing](CONTRIBUTING.md)
- [GitHub Repository](https://github.com/Chronovyan/Chronovyan.github.io)
- [Roadmap](ROADMAP.md)

## Community

- [GitHub Repository](https://github.com/yourusername/chronovyan)
- [Issue Tracker](https://github.com/yourusername/chronovyan/issues)
- [Discussions](https://github.com/yourusername/chronovyan/discussions)

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
