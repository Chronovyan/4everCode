---
title: Quick Start
description: Get up and running with Chronovyan and) runtime") in minutes
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Quick Start Guide

This guide will help you get started with Chronovyan by walking through common use cases and examples.

## Basic Usage

###

``cpp
    #include <chronovyan/chronovyan.hpp>

    // Optional: Bring the namespace into scope
    using namespace chronovyan;
```text

###

``cpp
    auto now = system_clock::now();
    std::cout << "Current time: " << now << "\n";
```text

###

``cpp
    // Create durations
    auto one_hour = 1h;
    auto thirty_minutes = 30min;

    // Perform arithmetic
    auto total = one_hour + thirty_minutes;  // 90 minutes

    // Convert between units
    auto hours = duration_cast<hours>(total);  // 1 hour
```text

###

``cpp
    // Get current time
    auto now = system_clock::now();

    // Add/subtract durations
    auto in_one_hour = now + 1h;
    auto yesterday = now - 24h;

    // Difference between time points
    auto diff = in_one_hour - now;  // 1 hour
```text

## Time Zone Support

###

``cpp
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
```text

###

``cpp
    auto zone = locate_zone("America/New_York");

    // A time that might be in DST transition
    auto tp = sys_days{2023y/March/12} + 2h + 30min;  // DST starts at 2 AM

    // Convert to zoned time
    auto zt = zoned_time{zone, tp};
    std::cout << zt << "\n";  // Will show correct DST status
```text

## Formatting and Parsing

###

``cpp
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
```text

###

``cpp
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
```text

## Practical Examples

###

``cpp
    auto start = system_clock::now();

    // Your code here
    std::this_thread::sleep_for(100ms);

    auto end = system_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    std::cout << "Execution took " << duration.count() << "ms\n";
```text

###

``cpp
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
```text

## Next Steps

 Explore more [examples](/).).)/)e)x)a)m)p)l)e)s)/) for advanced use cases
- Read the [API Reference](/).).)/)a)p)i)/)r)e)f)e)r)e)n)c)e)/) for detailed documentation
- Check out the [Best Practices](/).).)/)g)u)i)d)e)s)/)b)e)s)t)-)p)r)a)c)t)i)c)e)s)/) guide
- Join our [community](/).).)/)c)o)m)m)u)n)i)t)y)/) for help and discussions

## Need Help?

If you have any questions or run into issues:
1. Check the [FAQ](/).).)/)f)a)q)/)
2. Search the [GitHub Issues](https://github.com/Chronovyan/Chronovyan.github.io)t)t)p)s):)/)/)g)i)t)h)u)b).)c)o)m)/)C)h)r)o)n)o)v)y)a)n)/)C)h)r)o)n)o)v)y)a)n)/)i)s)s)u)e)s)
3. [Ask a question](https://github.com/Chronovyan/Chronovyan.github.io)t)t)p)s):)/)/)g)i)t)h)u)b).)c)o)m)/)C)h)r)o)n)o)v)y)a)n)/)C)h)r)o)n)o)v)y)a)n)/)d)i)s)c)u)s)s)i)o)n)s) in our discussions
