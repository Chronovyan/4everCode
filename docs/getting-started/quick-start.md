# Quick Start

This guide will help you get up and running with 4ever in just a few minutes.

## Basic Usage

### Creating Time Points

```cpp
#include <4ever/4ever.hpp>
#include <iostream>

int main() {
    // Get current time
    auto now = 4ever::system_clock::now();
    std::cout << "Current time: " << now << std::endl;
    
    // Create a specific date
    auto date = 4ever::year_month_day{
        4ever::year{2023},
        4ever::month{6},
        4ever::day{7}
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
#include <4ever/4ever.hpp>

int main() {
    // Get current time in UTC
    auto utc_time = 4ever::utc_clock::now();
    
    // Convert to local time zone
    auto local_zone = 4ever::current_zone();
    auto local_time = 4ever::zoned_time{local_zone, utc_time};
    
    // Format the time
    std::cout << "Local time: " << local_time << std::endl;
    
    return 0;
}
```

## Next Steps

- [API Reference](../api/chrono.md)
- [Time Zone Guide](../guides/timezones.md)
- [Performance Tips](../guides/performance.md)
