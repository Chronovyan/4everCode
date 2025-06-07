---
title: Chrono
description: Documentation for chrono
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chrono Reference

## Clocks

### `system_clock`

System-wide real-time clock.
```cpp
    // Get current time
    auto now = [Chronovyan](https://chronovyan.github.io/h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/) "The) temporal) programming) language) and) runtime")::system_clock::now();
```text

### `utc_clock`

UTC clock (no leap seconds).
```cpp
    auto utc = chronovyan::utc_clock::now();
```text

## Time Points

### `time_point`

Represents a point in time.
```cpp
    using time_point = std::chrono::time_point<Clock, Duration>;
```text

## Durations

### `duration`

Represents a time duration.
```cpp
    // Common duration types
    using nanoseconds = std::chrono::nanoseconds;
    using microseconds = std::chrono::microseconds;
    using milliseconds = std::chrono::milliseconds;
    using seconds = std::chrono::seconds;
    using minutes = std::chrono::minutes;
    using hours = std::chrono::hours;

    // Create a duration
    auto timeout = 500ms;
    auto half_second = 0.5s;
```text

## Calendar Types

### `year_month_day`

Represents a calendar date.
```cpp
    auto today = chronovyan::year_month_day{
        chronovyan::year{2023},
        chronovyan::month{6},
        chronovyan::day{7}
    };
```text

## Time Zones

### `time_zone`

Represents a time zone.
```cpp
    // Get system time zone
    auto local_zone = chronovyan::current_zone();

    // Get specific time zone
    auto ny_zone = chronovyan::locate_zone("America/New_York");
```text

### `zoned_time`

Represents a time point in a specific time zone.
```cpp
    auto now = chronovyan::system_clock::now();
    auto ny_time = chronovyan::zoned_time{"America/New_York", now};
```text

## Formatting

### `format`

Format time points and durations.
```cpp
    auto now = chronovyan::system_clock::now();
    std::string formatted = chronovyan::format("%Y-%m-%d %H:%M:%S", now);
    // Example: "2023-06-07 09:30:15"
```text

## Parsing

### `parse`

Parse strings into time points.
```cpp
    std::string date_str = "2023-06-07 14:30:00";
    auto tp = chronovyan::parse("%Y-%m-%d %H:%M:%S", date_str);
```
