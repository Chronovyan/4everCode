---
title: Time Module
description: Date and time handling
weight: 70
---

# Time Module

## Overview

The `std.time` module provides comprehensive date and time handling capabilities, including time zones, durations, and formatting.

## Basic Types

### `DateTime`
Represents a specific point in time with nanosecond precision.

### `Date`
Represents a calendar date (year, month, day).

### `Time`
Represents a time of day (hours, minutes, seconds, nanoseconds).

### `Duration`
Represents a length of time with nanosecond precision.

## Getting Current Time

```chronovyan
IMPORT std.time as time;

// Current date and time
DECLARE now = time.now();
PRINT(now);  // e.g., 2023-04-01T12:34:56.789Z

// Current date
DECLARE today = time.today();
PRINT(today);  // e.g., 2023-04-01

// Current time
DECLARE current_time = time.time_now();
PRINT(current_time);  // e.g., 12:34:56.789
```

## Creating Date and Time

```chronovyan
// From components
DECLARE dt1 = time.DateTime(
    year: 2023,
    month: 4,
    day: 1,
    hour: 12,
    minute: 34,
    second: 56
);

// From string
DECLARE dt2 = time.parse_datetime("2023-04-01T12:34:56Z");

// From Unix timestamp (seconds since epoch)
DECLARE dt3 = time.from_timestamp(1680359696);
```

## Date/Time Components

```chronovyan
DECLARE dt = time.now();

PRINT(dt.year());      // 2023
PRINT(dt.month());     // 4 (April)
PRINT(dt.day());       // 1
PRINT(dt.hour());      // 12
PRINT(dt.minute());    // 34
PRINT(dt.second());    // 56
PRINT(dt.weekday());   // 6 (Saturday)
PRINT(dt.yearday());   // 91 (day of year)
```

## Time Zones

```chronovyan
// Get UTC timezone
DECLARE utc = time.utc();

// Get local timezone
DECLARE local = time.local();

// Get specific timezone
DECLARE ny = time.timezone("America/New_York");

// Convert between timezones
DECLARE dt_utc = time.now();
DECLARE dt_ny = dt_utc.with_timezone(ny);
```

## Duration and Arithmetic

```chronovyan
// Create durations
DECLARE d1 = time.duration(hours: 1, minutes: 30);
DECLARE d2 = time.duration(seconds: 90);

// Duration arithmetic
PRINT(d1 + d2);  // 2 hours, 1 minute, 30 seconds
PRINT(d1 > d2);  // true

// Add/subtract from DateTime
DECLARE future = now + time.duration(days: 7);
DECLARE past = now - time.duration(weeks: 2);

// Difference between DateTimes
DECLARE diff = future - past;  // Returns Duration
```

## Formatting and Parsing

### Format Specifiers

| Specifier | Description | Example |
|-----------|-------------|---------|
| `%Y` | Year with century | 2023 |
| `%y` | Year without century (00-99) | 23 |
| `%m` | Month (01-12) | 04 |
| `%d` | Day of month (01-31) | 01 |
| `%H` | Hour (00-23) | 14 |
| `%M` | Minute (00-59) | 34 |
| `%S` | Second (00-60) | 56 |
| `%Z` | Timezone name | UTC, EST |
| `%z` | Timezone offset | +0000, -0500 |

### Formatting Examples

```chronovyan
DECLARE dt = time.now();

// Default format
PRINT(dt.format());  // "2023-04-01T12:34:56.789Z"

// Custom format
PRINT(dt.format("%Y-%m-%d %H:%M:%S"));  // "2023-04-01 12:34:56"
PRINT(dt.format("%A, %B %d, %Y"));     // "Saturday, April 01, 2023"
```

### Parsing Examples

```chronovyan
// Parse with known format
DECLARE dt1 = time.parse_datetime("2023-04-01 12:34:56", "%Y-%m-%d %H:%M:%S");

// Try to guess format
DECLARE dt2 = time.parse_datetime_loose("01/04/2023 12:34 PM");
```

## Time Measurement

### High-Resolution Timing

```chronovyan
// Start a timer
DECLARE start = time.instant_now();

// Do some work
FOR i IN 1..1_000_000 {
    // Some computation
}

// Get elapsed time
DECLARE elapsed = time.instant_now() - start;
PRINT("Took ", elapsed.as_millis(), " ms");
```

### Benchmarking

```chronovyan
// Time a function call
DECLARE (result, duration) = time.measure(\() -> INT {
    RETURN expensive_computation();
});

PRINT("Result: ", result);
PRINT("Duration: ", duration);
```

## Example: Countdown Timer

```chronovyan
IMPORT std.time as time;
IMPORT std.io as io;

FUNC VOID countdown(minutes: INT) {
    DECLARE end = time.now() + time.duration(minutes: minutes);
    
    WHILE (TRUE) {
        DECLARE remaining = end - time.now();
        IF (remaining <= time.duration(seconds: 0)) {
            BREAK;
        }
        
        // Format as MM:SS
        DECLARE mins = remaining.whole_minutes();
        DECLARE secs = (remaining - time.duration(minutes: mins)).whole_seconds();
        io.print_f("\r{:02}:{:02}", mins, secs);
        
        time.sleep(time.duration(millis: 100));
    }
    
    io.println("\nTime's up!");
}
```

## Time Zones and DST

### Handling DST Transitions

```chronovyan
// Get timezone with DST support
DECLARE tz = time.timezone("America/New_York");

// During DST transition
DECLARE dt1 = time.DateTime(
    year: 2023,
    month: 3,
    day: 12,
    hour: 1,
    minute: 30,
    timezone: tz
);

// This time might not exist or be ambiguous
MATCH tz.resolve(dt1) {
    OK(instant) => {
        // Handle valid time
    },
    ERR(Ambiguous(early, late)) => {
        // Handle ambiguous time (DST fallback)
    },
    ERR(Nonexistent) => {
        // Handle non-existent time (DST spring forward)
    }
}
```

## Performance Considerations

### Time Measurement

```chronovyan
// High-precision timing
LET start = time.Instant::now();
// ... operation to time ...
LET duration = start.elapsed();
println!("Operation took: {:?}", duration);

// For microbenchmarks, use the `test` module's benchmarking tools
```

### Time Zone Handling

```chronovyan
// Convert between time zones
LET local = time.Local::now();
LET utc = local.to_utc();
LET new_york = time.Tz::America__New_York.from_utc(utc);

// Handle DST transitions
IF new_york.is_dst() {
    println!("Daylight Saving Time is in effect");
}
```

## Common Pitfalls

1. **Clock Drift**
   ```chronovyan
   // Bad: Subject to system clock changes
   LET now = time.SystemTime::now();
   
   // Good: Monotonic clock for measuring durations
   LET start = time.Instant::now();
   ```

2. **Time Zone Ambiguity**
   ```chronovyan
   // Handle ambiguous times during DST transitions
   MATCH time.Local.from_local_time("2023-11-05 01:30:00") {
       time.LocalResult::Single(t) => { /* unique time */ },
       time.LocalResult::Ambiguous(earlier, later) => { /* handle ambiguity */ },
       time.LocalResult::None => { /* time doesn't exist */ },
   }
   ```

## Related

- [Temporal](./temporal.md) - For advanced time manipulation and time travel
- [Concurrency](./concurrency.md) - For timeouts and scheduling
- [IO](./io.md) - For file timestamps and timeouts
- [Testing](./testing.md) - For time-based testing utilities

## Best Practices

1. **Use appropriate time sources**:
   - `Instant` for measuring durations
   - `SystemTime` for wall-clock time
   - `Temporal` for application-level time

2. **Handle timeouts** in all I/O operations

3. **Be explicit about time zones**

4. **Use `Duration` for relative time**

5. **Test time-dependent code** with controlled clocks

## See Also

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Chronovyan Language Specification](reference/language/specification.md) - For built-in date/time literals concepts
- [Concurrency](./concurrency.md) - For timeouts and scheduling
