---
title: Working with Time Zones
description: Comprehensive guide to handling time zones in Chronovyan
---

# Working with Time Zones

Time zones are a critical aspect of any date and time library. Chronovyan provides comprehensive support for working with time zones, including handling daylight saving time (DST) transitions and time zone conversions.

## Table of Contents

- [Basic Time Zone Operations](#basic-time-zone-operations)
- [Daylight Saving Time](#daylight-saving-time)
- [Time Zone Database](#time-zone-database)
- [Common Operations](#common-operations)
- [Best Practices](#best-practices)
- [Common Pitfalls](#common-pitfalls)
- [Advanced Topics](#advanced-topics)

## Basic Time Zone Operations

### Getting the System Time Zone

```cpp
#include <chronovyan/chronovyan.hpp>
#include <iostream>

int main() {
    auto tz = chronovyan::current_zone();
    std::cout << "Current time zone: " << tz->name() << "\n";
    return 0;
}
```

### Converting Between Time Zones

```cpp
#include <chronovyan/chronovyan.hpp>
#include <iostream>

int main() {
    using namespace std::chrono;
    using namespace chronovyan;
    
    // Get current time
    auto now = system_clock::now();
    
    // Convert to New York time
    auto ny_tz = locate_zone("America/New_York");
    auto ny_time = zoned_time{ny_tz, now};
    
    // Convert to Tokyo time
    auto tokyo_tz = locate_zone("Asia/Tokyo");
    auto tokyo_time = zoned_time{tokyo_tz, now};
    
    std::cout << "UTC:        " << now << "\n";
    std::cout << "New York:   " << ny_time << "\n";
    std::cout << "Tokyo:      " << tokyo_time << "\n";
    
    return 0;
}
```

## Daylight Saving Time

Chronovyan automatically handles DST transitions. Here's how to work with them:

### Checking for DST

```cpp
auto is_dst(const time_zone* tz, const system_clock::time_point& tp) {
    auto info = tz->get_info(tp);
    return info.save != 0min;
}

// Usage
auto tz = locate_zone("America/New_York");
auto summer = sys_days{July/1/2023} + 12h;
auto winter = sys_days{January/1/2023} + 12h;

std::cout << "Summer is DST: " << is_dst(tz, summer) << "\n";
std::cout << "Winter is DST: " << is_dst(tz, winter) << "\n";
```

### Handling DST Transitions

```cpp
try {
    auto tz = locate_zone("America/New_York");
    
    // Spring forward (typically 2 AM becomes 3 AM)
    auto spring_forward = local_days{March/12/2023} + 2h + 30min;
    auto zt = zoned_time{tz, spring_forward};
    std::cout << "Spring forward: " << zt << "\n";
    
    // Fall back (1:30 AM occurs twice)
    auto fall_back = local_days{November/5/2023} + 1h + 30min;
    zt = zoned_time{tz, fall_back};
    std::cout << "Fall back: " << zt << "\n";
    
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

## Time Zone Database

Chronovyan uses the IANA time zone database. You can list all available time zones:

```cpp
void list_time_zones() {
    std::cout << "Available time zones:\n";
    for (const auto& zone : get_tzdb().zones) {
        std::cout << "- " << zone.name() << "\n";
    }
}
```

### Updating the Time Zone Database

```cpp
try {
    // Reload the time zone database
    reload_tzdb();
    
    // Get the latest database
    auto& db = get_tzdb();
    std::cout << "TZ Database version: " << db.version << "\n";
    
} catch (const std::exception& e) {
    std::cerr << "Failed to update time zone database: " << e.what() << "\n";
}
```

## Common Operations

### Converting Between Time Zones

```cpp
// Convert between arbitrary time zones
void convert_between_zones(
    const std::string& from_zone,
    const std::string& to_zone,
    const system_clock::time_point& tp) {
    
    auto from = locate_zone(from_zone);
    auto to = locate_zone(to_zone);
    
    auto zt = zoned_time{from, tp};
    auto result = zoned_time{to, zt};
    
    std::cout << from_zone << ": " << zt << "\n";
    std::cout << to_zone << ":   " << result << "\n";
}
```

### Formatting with Time Zones

```cpp
void format_with_time_zone() {
    auto now = system_clock::now();
    auto tz = locate_zone("America/New_York");
    auto zt = zoned_time{tz, now};
    
    // Format with time zone abbreviation
    std::cout << format("%F %T %Z", zt) << "\n";
    
    // Format with UTC offset
    std::cout << format("%F %T %z", zt) << "\n";
    
    // Custom format with time zone info
    std::cout << format("It's %A, %B %d, %Y at %I:%M %p %Z", zt) << "\n";
}
```

## Best Practices

1. **Store Times in UTC**
   - Convert to local time zones only when displaying to users
   - Example: `auto utc_time = system_clock::now();`

2. **Use IANA Time Zone Names**
   - Prefer "America/New_York" over "EST" or "EDT"
   - These are standardized and handle DST automatically

3. **Handle Ambiguities**
   - Always handle `nonexistent_local_time` and `ambiguous_local_time` exceptions
   - Provide user-friendly error messages

4. **Be Explicit About Time Points**
   - Clearly document whether a time is in local time or UTC
   - Consider using type aliases: `using utc_time = system_clock::time_point;`

5. **Update the Time Zone Database**
   - Keep it current for accurate historical and future dates
   - Consider bundling the latest database with your application

## Common Pitfalls

### 1. Ambiguous Times

When clocks are set back, some local times occur twice:

```cpp
try {
    auto tz = locate_zone("America/New_York");
    auto ambiguous_time = local_days{November/5/2023} + 1h + 30min;
    auto zt = zoned_time{tz, ambiguous_time};
} catch (const ambiguous_local_time& e) {
    // Handle ambiguous time (e.g., ask user to disambiguate)
    auto possible_times = e.possible();
    // ...
}
```

### 2. Non-existent Times

When clocks are set forward, some local times don't exist:

```cpp
try {
    auto tz = locate_zone("America/New_York");
    auto non_existent = local_days{March/12/2023} + 2h + 30min;
    auto zt = zoned_time{tz, non_existent};
} catch (const nonexistent_local_time& e) {
    // Handle non-existent time (e.g., adjust to next valid time)
    auto tp = e.what_should_be_here();
    // ...
}
```

### 3. Leap Seconds

Chronovyan supports leap seconds, but most system clocks don't:

```cpp
// Check if leap seconds are supported
constexpr bool has_leap_seconds = 
    system_clock::period::num == 1 && 
    system_clock::period::den == 1'000'000'000;

// Convert to UTC with leap seconds if supported
if constexpr (has_leap_seconds) {
    auto utc = utc_clock::from_sys(system_clock::now());
    // ...
}
```

## Advanced Topics

### Custom Time Zones

Create a custom time zone with fixed offset:

```cpp
auto create_custom_zone(const std::string& name, minutes offset) {
    auto tz = std::make_shared<time_zone>(name);
    tz->add_rule({
        .begin = year::min(),
        .end = year::max(),
        .offset = offset,
        .save = 0min,
        .abbrev = name
    });
    return tz;
}

// Usage
auto custom_zone = create_custom_zone("CUSTOM", 3h + 30min);
```

### Time Zone Transitions

Inspect time zone transitions (e.g., DST changes):

```cpp
void print_transitions(const time_zone* tz, 
                      const system_clock::time_point& start,
                      const system_clock::time_point& end) {
    
    auto transitions = tz->get_transitions(start, end);
    
    std::cout << "Time zone transitions for " << tz->name() << ":\n";
    for (const auto& trans : transitions) {
        std::cout << "- " << trans.when << ": "
                  << trans.offset.count() / 60 << "h "
                  << (trans.save > 0min ? "(DST)" : "(STD)")
                  << "\n";
    }
}
```

### Thread Safety

Time zone operations are thread-safe, but be careful with shared objects:

```cpp
// Thread-safe
auto tz = locate_zone("America/New_York");  // OK to share

// Not thread-safe - create per thread
auto now = system_clock::now();
```

## Example: Meeting Scheduler

```cpp
struct Meeting {
    std::string title;
    std::string location_zone;
    local_days date;
    minutes time_of_day;
    
    system_clock::time_point get_utc_time() const {
        auto tp = date + time_of_day;
        auto tz = locate_zone(location_zone);
        return tz->to_sys(tp);
    }
    
    std::string format_local(const std::string& user_zone) const {
        auto tz = locate_zone(user_zone);
        auto utc = get_utc_time();
        auto local = zoned_time{tz, utc};
        return format("%A, %B %d, %Y at %I:%M %p %Z", local);
    }
};

// Usage
Meeting meeting{
    .title = "Project Kickoff",
    .location_zone = "America/New_York",
    .date = November/15/2023,
    .time_of_day = 14h  // 2:00 PM
};

std::cout << "Meeting time in New York: " 
          << meeting.format_local("America/New_York") << "\n";
std::cout << "Meeting time in London:   "
          << meeting.format_local("Europe/London") << "\n";
```

## Further Reading

- [IANA Time Zone Database](https://www.iana.org/time-zones)
- [Time Zone Best Practices](https://www.creativedeletion.com/2015/01/28/timezone-handling-in-software.html)
- [The Problem with Time & Timezones](https://www.youtube.com/watch?v=-5wpm-gesOY)

## Conclusion

Working with time zones can be complex, but Chronovyan provides the tools you need to handle them correctly. By following the best practices outlined in this guide and being aware of common pitfalls, you can write robust, timezone-aware applications that work correctly around the world.
