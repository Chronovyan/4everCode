# Quick Start

This guide will help you get up and running with Chronovyan in just a few minutes.

## Basic Usage

### Creating Time Points

```cpp
#include <chronovyan/chronovyan.hpp>
#include <iostream>

int main() {
    // Get current time
    auto now = chronovyan::system_clock::now();
    std::cout << "Current time: " << now << std::endl;
    
    // Create a specific date
    auto date = chronovyan::year_month_day{
        chronovyan::year{2023},
        chronovyan::month{6},
        chronovyan::day{7}
    };
    std::cout << "Specific date: " << date << std::endl;
    
    // Create a time duration
    auto duration = 2h + 30min;
    std::cout << "Duration: " << duration << std::endl;
    
    return 0;
}
```

### Time Zone Support

```cpp
#include <chronovyan/chronovyan.hpp>

int main() {
    // Get current time in UTC
    auto utc_time = chronovyan::utc_clock::now();
    
    // Convert to local time zone
    auto local_zone = chronovyan::current_zone();
    auto local_time = chronovyan::zoned_time{local_zone, utc_time};
    
    // Format the time
    std::cout << "Local time: " << local_time << std::endl;
    
    return 0;
}
```

## Next Steps

- [API Reference](../api/chrono.md)
- [Time Zone Guide](../guides/timezones.md)
- [Performance Tips](../guides/performance.md)
