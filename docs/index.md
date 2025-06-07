---
title: Home
---

# ⏳ Chronovyan

**A Modern C++ Library for Dates, Times, and Time Zones**

Chronovyan is a comprehensive C++20 library that provides powerful tools for working with dates, times, and time zones. Built with performance, safety, and ease of use in mind, Chronovyan is the perfect choice for applications that require precise time handling.

## 🌟 Key Features

<div class="grid cards" markdown>

- :material-clock-fast: **High Performance**
  - Zero-cost abstractions
  - Header-only design
  - Minimal runtime overhead

- :material-earth: **Time Zone Support**
  - Full IANA time zone database
  - Automatic DST handling
  - Efficient time zone lookups

- :material-code-braces: **Modern C++20**
  - Type-safe API
  - constexpr support
  - Concepts and ranges

- :material-shield-check: **Robust**
  - Thread-safe by design
  - Extensive test coverage
  - Memory safe operations

</div>

## 🚀 Quick Start

```cpp
#include <chronovyan/chronovyan.hpp>
#include <iostream>

int main() {
    using namespace chronovyan;
    
    // Get current time
    auto now = system_clock::now();
    std::cout << "Current time: " << now << "\n";
    
    // Format with time zone
    auto ny_zone = locate_zone("America/New_York");
    auto ny_time = zoned_time{ny_zone, now};
    std::cout << "New York: " << format("{:%F %T %Z}", ny_time) << "\n";
    
    // Date arithmetic
    auto tomorrow = floor<days>(now) + days{1};
    std::cout << "Tomorrow: " << tomorrow << "\n";
    
    return 0;
}
```

## 📖 Documentation

<div class="grid cards" markdown>

- [**Getting Started**](getting-started/installation/)
  - Installation guide
  - First steps
  - Basic examples

- [**Guides**](guides/timezones/)
  - Time zone handling
  - Best practices
  - Performance tips

- [**API Reference**](api/reference/)
  - Detailed class docs
  - Function references
  - Type definitions

- [**Development**](development/building/)
  - Building from source
  - Running tests
  - Contributing

</div>

## 💡 Why Choose Chronovyan?

- **Modern C++20** - Leverages the latest language features
- **Header-Only** - Easy integration into any project
- **Cross-Platform** - Works consistently across all major platforms
- **Well-Documented** - Comprehensive guides and API references
- **Active Development** - Regular updates and community support

## 🌐 Getting Help

- [GitHub Issues](https://github.com/Chronovyan/Chronovyan/issues) - Report bugs or request features
- [Discussions](https://github.com/Chronovyan/Chronovyan/discussions) - Ask questions and share ideas
- [Examples](getting-started/examples/) - Browse example code

## 📄 License

Chronovyan is distributed under the [MIT License](https://opensource.org/licenses/MIT).

---

<div class="mdx-parallax" data-mdx-component="parallax">
  <div class="mdx-parallax__image" style="--image: url('assets/images/hero.jpg')"></div>
  <div class="mdx-parallax__content">
    <h3>Ready to get started?</h3>
    <p>Check out our <a href="getting-started/installation/">installation guide</a> to begin using Chronovyan in your project.</p>
  </div>
</div>

## Explore

- [Language Reference](language/syntax.md) - Syntax, types, and core language features
- [Temporal Programming](temporal-programming/concepts.md) - Master time-based programming
- [Standard Library](api/standard-library.md) - Built-in functions and modules

## Resources

- [API Reference](api/standard-library.md) - Comprehensive API documentation
- [Examples](getting-started/examples.md) - Practical code samples
- [Community](community/index.md) - Get help and contribute

## Development

- [Building from Source](development/building.md) - Compile Chronovyan yourself
- [Contributing](development/contributing.md) - How to contribute to Chronovyan
- [Code of Conduct](community/code-of-conduct.md) - Our community standards

## Need Help?

- [FAQ](community/faq.md) - Common questions and answers
- [Troubleshooting](community/troubleshooting.md) - Solutions to common issues
- [GitHub Issues](https://github.com/chronovyan/chronovyan/issues) - Report bugs or request features

## Join the Community

- [GitHub](https://github.com/chronovyan/chronovyan)
- [Discord](https://discord.gg/chronovyan)
- [Twitter](https://twitter.com/chronovyan)

---

*Chronovyan is an open-source project. [Contribute](development/contributing.md) to make it better!*
