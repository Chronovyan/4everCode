# Chrono Reference

## Clocks

### `system_clock`

System-wide real-time clock.

```cpp
// Get current time
auto now = 4ever::system_clock::now();
```

### `utc_clock`

UTC clock (no leap seconds).

```cpp
auto utc = 4ever::utc_clock::now();
```

## Time Points

### `time_point`

Represents a point in time.

```cpp
using time_point = std::chrono::time_point<Clock, Duration>;
```

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
```

## Calendar Types

### `year_month_day`

Represents a calendar date.

```cpp
auto today = 4ever::year_month_day{
    4ever::year{2023},
    4ever::month{6},
    4ever::day{7}
};
```

## Time Zones

### `time_zone`

Represents a time zone.

```cpp
// Get system time zone
auto local_zone = 4ever::current_zone();

// Get specific time zone
auto ny_zone = 4ever::locate_zone("America/New_York");
```

### `zoned_time`

Represents a time point in a specific time zone.

```cpp
auto now = 4ever::system_clock::now();
auto ny_time = 4ever::zoned_time{"America/New_York", now};
```

## Formatting

### `format`

Format time points and durations.

```cpp
auto now = 4ever::system_clock::now();
std::string formatted = 4ever::format("%Y-%m-%d %H:%M:%S", now);
// Example: "2023-06-07 09:30:15"
```

## Parsing

### `parse`

Parse strings into time points.

```cpp
std::string date_str = "2023-06-07 14:30:00";
auto tp = 4ever::parse("%Y-%m-%d %H:%M:%S", date_str);
```
