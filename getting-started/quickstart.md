---
title: Quick Start
description: Get up and running with 4ever in minutes
---

# Quick Start Guide

This guide will help you get started with 4ever by walking through common use cases and examples.

## Basic Usage

### Including the Library

```cpp
#include <4ever/4ever.hpp>

// Optional: Bring the namespace into scope
using namespace 4ever;
```

### Getting the Current Time

```cpp
auto now = system_clock::now();
std::cout << "Current time: " << now << "\n";
```

### Working with Durations

```cpp
// Create durations
auto one_hour = 1h;
auto thirty_minutes = 30min;

// Perform arithmetic
auto total = one_hour + thirty_minutes;  // 90 minutes

// Convert between units
auto hours = duration_cast<hours>(total);  // 1 hour
```

### Time Points and Arithmetic

```cpp
// Get current time
auto now = system_clock::now();

// Add/subtract durations
auto in_one_hour = now + 1h;
auto yesterday = now - 24h;

// Difference between time points
auto diff = in_one_hour - now;  // 1 hour
```

## Time Zone Support

### Working with Time Zones

```cpp
// Get a time zone
try {
    auto ny_zone = locate_zone("America/New_York");
    
    // Convert system time to New York time
    auto ny_time = zoned_time{ny_zone, system_clock::now()};
    std::cout << "New York time: " << ny_time << "\n";
    
    // Get local time in that zone
    auto local_ny = ny_zone->to_local(system_clock::now());
    
} catch (const std::runtime_error& e) {
    std::cerr << "Time zone error: " << e.what() << "\n";
}
```

### Handling Daylight Saving Time

```cpp
auto zone = locate_zone("America/New_York");

// A time that might be in DST transition
auto tp = sys_days{2023y/March/12} + 2h + 30min;  // DST starts at 2 AM

// Convert to zoned time
auto zt = zoned_time{zone, tp};
std::cout << zt << "\n";  // Will show correct DST status
```

## Formatting and Parsing

### Formatting Dates and Times

```cpp
using namespace std::chrono;

// Current time
auto now = system_clock::now();

// Format using format string
std::cout << format("Today is {:%A, %B %d, %Y}\n", now);
std::cout << format("The time is {:%H:%M:%S}\n", now);

// Using locale settings
std::cout << format(std::locale("en_US.UTF-8"), 
                   "%c\n", 
                   now);
```

### Parsing Dates and Times

```cpp
try {
    // Parse a time string
    std::istringstream ss{"2023-12-25 15:30:00"};
    sys_seconds tp;
    ss >> parse("%Y-%m-%d %H:%M:%S", tp);
    
    if (!ss.fail()) {
        std::cout << "Parsed time: " << tp << "\n";
    }
} catch (const std::exception& e) {
    std::cerr << "Parse error: " << e.what() << "\n";
}
```

## Practical Examples

### Measuring Code Execution Time

```cpp
auto start = system_clock::now();

// Your code here
std::this_thread::sleep_for(100ms);

auto end = system_clock::now();
auto duration = duration_cast<milliseconds>(end - start);

std::cout << "Execution took " << duration.count() << "ms\n";
```

### Scheduling Tasks

```cpp
// Schedule a task to run at a specific time
auto schedule_task(auto&& task, sys_time<milliseconds> when) {
    auto now = system_clock::now();
    if (when > now) {
        std::this_thread::sleep_for(when - now);
    }
    task();
}

// Usage
schedule_task(
    []{ std::cout << "Task executed at " << system_clock::now() << "\n"; },
    system_clock::now() + 5s  // Run after 5 seconds
);
```

## Next Steps

- Explore more [examples](../examples/) for advanced use cases
- Read the [API Reference](../api/reference/) for detailed documentation
- Check out the [Best Practices](../guides/best-practices/) guide
- Join our [community](../community/) for help and discussions

## Need Help?

If you have any questions or run into issues:
1. Check the [FAQ](../faq/)
2. Search the [GitHub Issues](https://github.com/4ever/4ever/issues)
3. [Ask a question](https://github.com/4ever/4ever/discussions) in our discussions
