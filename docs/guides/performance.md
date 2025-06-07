---
title: Performance Optimization
description: Best practices and techniques for optimizing Chronovyan and) runtime")
  performance
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Performance Optimization Guide

This guide provides best practices and techniques for optimizing the performance of applications using Chronovyan. Following these guidelines will help you write efficient and scalable time-handling code.
```mermaid
    graph TD
        A[Performance Considerations] --> B[Clock Selection]
        A --> C[Time Point Arithmetic]
        A --> D[Time Zone Optimization]
        A --> E[String Operations]
        A --> F[Memory Management]
        B --> G[system_clock, steady_clock, high_resolution_clock]
        C --> H[Duration Types, Batch Operations]
        D --> I[Cache Lookups, Batch Conversions]
        E --> J[Pre-allocation, String Views]
        F --> K[Avoid Copies, Smart Pointers]
```text

## Table of Contents

 [Clock Selection](#))
)
)) )[)T)i)m)e) )P)o)i)n)t) )A)r)i)t)h)m)e)t)i)c)])()#))
)
)) )[)T)i)m)e) )Z)o)n)e) )O)p)t)i)m)i)z)a)t)i)o)n)])()#))
)
)) )[)S)t)r)i)n)g) )O)p)e)r)a)t)i)o)n)s)])()#))
)
)) )[)D)u)r)a)t)i)o)n) )T)y)p)e)s)])()#))
)
)) )[)M)e)m)o)r)y) )M)a)n)a)g)e)m)e)n)t)])()#))
)
)) )[)T)h)r)e)a)d) )S)a)f)e)t)y)])()#))
)
)) [Compile-Time) Optimization](#))
)
) [Benchmarking](#))
)
) [Real-World) Examples](#)r)e)a)l)-)w)o)r)l)d)-)e)x)a)m)p)l)e)s)

## Performance Cheat Sheet

| Operation | Fast ✅ | Slow ❌ | --- |--------| --- | Clock Access | `steady_clock::now()` |`system_clock::now()` | Time Zone | Cached lookup | Repeated lookups | String Ops | `format_to` with buffer | Multiple allocations | Duration | Fixed-size types | Floating-point | Memory | Stack allocation | Unnecessary copies |

##

``mermaid
    pie
        title Clock Characteristics
        "Monotonic" : 2
        "Adjustable" : 1
        "High Precision" : 1
```text

Choose the most appropriate clock for your use case:

| Clock Type | Characteristics | Best For | Performance | --- |-----------------| --- |-------------| `system_clock` | Wall clock time, can be adjusted | Timestamps, logging | ⚡ Fast | `steady_clock` | Monotonic, never adjusted | Measuring intervals, timeouts | ⚡⚡ Very Fast | `high_resolution_clock` | Shortest tick period available | Precise measurements | ⚡⚡⚡ Fastest |

###

``mermaid
    flowchart LR
        A[Need Timestamp?] -->|Yes| B[system_clock]
        A -->|No| C[Measuring Time?]
        C -->|Yes| D[steady_clock]
        C -->|No| E[Need Max Precision?]
        E -->|Yes| F[high_resolution_clock]
        E -->|No| G[steady_clock]
```text

###

``cpp
    // For timestamps (can be affected by system time changes)
    auto timestamp = chronovyan::system_clock::now();

    // For measuring intervals (monotonic)
    auto start = chronovyan::steady_clock::now();
    // ... operation ...
    auto end = chronovyan::steady_clock::now();
    auto duration = end - start;

    // For high-precision timing
    auto precise_start = chronovyan::high_resolution_clock::now();
    // ... precision-sensitive operation ...
    auto precise_end = chronovyan::high_resolution_clock::now();
```text

###

``mermaid
    bar
        title Clock Access Latency (lower is better)
        system_clock: 15
        steady_clock: 12
        high_resolution_clock: 10
```text

## Time Point Arithmetic

###

``cpp
    // Good: Using duration literals
    using namespace std::chrono_literals;
    auto timeout = 500ms;

    // Better: Compile-time constants when possible
    constexpr auto TIMEOUT = 500ms;
    constexpr auto CACHE_TTL = 24h;
```text

### Batch Operations

For operations on multiple time points, process them in batches to improve cache locality:
```cpp
    struct Event {
        chronovyan::system_clock::time_point when;
        // ... other data ...
    };

    // Process events in batches
    void process_events(const std::vector<Event>& events) {
        constexpr size_t BATCH_SIZE = 64;
        for (size_t i = 0; i < events.size(); i += BATCH_SIZE) {
            auto batch_end = std::min(i + BATCH_SIZE, events.size());
            // Process batch...
        }
    }
```text

##

``mermaid
    pie
        title Time Zone Operation Costs
        "Lookup" : 40
        "Conversion" : 35
        "Cached Lookup" : 5
        "Cached Conversion" : 20
```text

Time zone operations can be performance bottlenecks. Here's how to optimize them:

### 1. Cache Time Zone Lookups

Time zone lookups are expensive operations. Always cache them when possible.

####

``mermaid
    flowchart LR
        A[Loop Start] --> B[Lookup Time Zone]
        B --> C[Use Time Zone]
        C --> D[Next Iteration]
        D --> B
```
```cpp
    for (const auto& city : cities) {
        auto tz = chronovyan::locate_zone(city.time_zone);  // Expensive!
        // ...
    }
```text

####

``mermaid
    flowchart LR
        A[Create Cache] --> B[Loop Start]
        B --> C{In Cache?}
        C -->|No| D[Lookup & Cache]
        C -->|Yes| E[Use Cached]
        D --> E
        E --> F[Next Iteration]
        F --> C
```
```cpp
    std::unordered_map<std::string, const chronovyan::time_zone*> tz_cache;
    for (const auto& city : cities) {
        auto it = tz_cache.find(city.time_zone);
        if (it == tz_cache.end()) {
            it = tz_cache.emplace(city.time_zone,
                                 chronovyan::locate_zone(city.time_zone)).first;
        }
        const auto* tz = it->second;
        // Use tz...
    }
```text

### 2. Batch Time Zone Conversions

When converting multiple time points, process them in batches to improve cache locality.

####

``mermaid
    gantt
        title Time Zone Conversion Strategies
        dateFormat  HH:mm:ss.SSS
        axisFormat %S.%Lms

        section Single Conversion
        Lookup TZ       :a1, 0s, 1s
        Convert Time 1  :a2, after a1, 2s
        Convert Time 2  :a3, after a2, 2s

        section Batch Conversion
        Single Lookup   :b1, 0s, 1s
        Convert All     :b2, after b1, 3s
```text

####

``cpp
    void convert_times(
        const std::vector<system_clock::time_point>& utc_times,
        const time_zone* tz,
        std::vector<zoned_time<system_clock::duration>>& results) {

        results.clear();
        results.reserve(utc_times.size());

        // Single time zone lookup, multiple conversions
        for (const auto& utc : utc_times) {
            results.emplace_back(tz, utc);
        }
    }
```text

### 3. Time Zone Conversion Performance Tips

| Operation | Cost | Optimization | --- |------| --- | Time Zone Lookup | High | Cache results | First Conversion | High | Warm up cache early | Subsequent Conversions | Medium | Batch process | DST Transitions | Variable | Pre-calculate transitions |

###

``mermaid
    classDiagram
        class time_zone {
            <<singleton>>
            +static const time_zone* locate_zone(string_view)
            +static const time_zone* current_zone()
        }

        class zoned_time {
            -const time_zone* zone_
            -sys_time time_
            +zoned_time()
            +zoned_time(const time_zone*, const sys_time&)
        }

        time_zone <|-- zoned_time
```text`time_zone` lookups are thread-safe after program startup
- `zoned_time` objects are immutable and thread-safe for concurrent reads
- For write operations, use synchronization primitives

##

``mermaid
    pie
        title String Operation Costs
        "Allocation" : 45
        "Formatting" : 35
        "Parsing" : 20
```text

String operations can be a significant bottleneck in time-related code. Here's how to optimize them:

### 1. Avoid Formatting in Tight Loops

####

``mermaid
    flowchart LR
        A[Loop Start] --> B[Allocate String]
        B --> C[Format to String]
        C --> D[Process String]
        D --> E[Deallocate String]
        E --> F[Next Iteration]
        F --> B
```
```cpp
    for (const auto& tp : time_points) {
        // New allocation on each iteration!
        std::string formatted = chronovyan::format("%Y-%m-%d %H:%M:%S", tp);
        process(formatted);
    }
```text

####

``mermaid
    flowchart LR
        A[Allocate Buffer] --> B[Loop Start]
        B --> C[Clear Buffer]
        C --> D[Format to Buffer]
        D --> E[Process Buffer]
        E --> F[Next Iteration]
        F --> C
```
```cpp
    std::string buffer;
    // Pre-allocate to avoid reallocations
    buffer.reserve(32);  // Sufficient for most date-time formats

    for (const auto& tp : time_points) {
        buffer.clear();
        // Reuse the same buffer
        chronovyan::format_to(std::back_inserter(buffer), "%Y-%m-%d %H:%M:%S", tp);
        process(buffer);
    }
```text

### 2. Use String Views for Parsing

Avoid unnecessary string copies by using `std::string_view`:```mermaid
    classDiagram
        class string_view {
            +size_t size()
            +const char* data()
            +string_view substr()
        }

        class string {
            +string(const char*)
            +const char*c_str()
        }

        string_view <|-- string
```text

####

``cpp
    std::optional<system_clock::time_point> parse_timestamp(std::string_view str) {
        system_clock::time_point tp;
        std::istringstream ss{std::string(str)};  // One allocation
        ss >> parse("%Y-%m-%d %H:%M:%S", tp);
        if (ss.fail()) return std::nullopt;
        return tp;
    }
```text

### 3. Performance Comparison

| Operation | Time (ms) | Relative | --- |-----------| --- | Naive Formatting | 100 | 1.0x | Reused Buffer | 35 | 2.9x | String View Parsing | 25 | 4.0x | Cached Format String | 15 | 6.7x |

### 4. Format String Optimization

Pre-compile format strings when possible:
```cpp
    // Compile-time format string
    constexpr auto fmt_string = "%Y-%m-%d %H:%M:%S";

    // At usage:
    chronovyan::format_to(back_inserter(buffer),
                         fmt_string,
                         time_point);
```text

###

``mermaid
    gantt
        title Memory Allocation Strategies
        dateFormat  HH:mm:ss.SSS
        axisFormat %S.%Lms

        section Multiple Allocations
        Alloc 1 :a1, 0s, 1s
        Alloc 2 :a2, after a1, 1s
        Alloc 3 :a3, after a2, 1s

        section Single Allocation
        Large Alloc :b1, 0s, 1s
        Reuse       :b2, after b1, 2s
```text**Avoid**multiple small allocations in performance-critical code
-**Pre-allocate**buffers when the maximum size is known
-**Reserve**capacity for containers when the size is predictable

##

``mermaid
    pie
        title Duration Type Usage
        "seconds" : 40
        "milliseconds" : 30
        "microseconds" : 15
        "nanoseconds" : 10
        "minutes" : 3
        "hours" : 2
```text

###

``mermaid
    flowchart TD
        A[Need Sub-millisecond?] -->|Yes| B[Use nanoseconds/microseconds]
        A -->|No| C[Human-readable?]
        C -->|Yes| D[Use seconds/minutes/hours]
        C -->|No| E[Need exact ms?]
        E -->|Yes| F[Use milliseconds]
        E -->|No| G[Use seconds]
```text

### Duration Type Comparison

| Type | Precision | Range | Memory | Performance | Best For | --- |-----------| --- |--------| --- |----------| `nanoseconds` | 1ns | ±292 years | 8 bytes | ⚡⚡⚡ | High-precision timing, benchmarks | `microseconds` | 1µs | ±292,000 years | 8 bytes | ⚡⚡⚡ | General timing, I/O operations | `milliseconds` | 1ms | ±292M years | 8 bytes | ⚡⚡ | UI updates, networking timeouts | `seconds` | 1s | ±292B years | 8 bytes | ⚡ | General purpose, timestamps | `minutes` | 1min | ±5.5e18 years | 8 bytes | ⚡ | Human intervals, scheduling | `hours` | 1h | ±3.3e17 years | 8 bytes | ⚡ | Long durations, calendar events |

###

``mermaid
    classDiagram
        class duration {
            +rep_ count_
            +period period_
            +rep count()
            +static duration zero()
            +duration operator+()
            +duration operator-()
        }

        duration <|-- seconds
        duration <|-- milliseconds
        duration <|-- microseconds
        duration <|-- nanoseconds
        duration <|-- minutes
        duration <|-- hours
```text

###

``cpp
    // High-precision timing
    auto start = chronovyan::high_resolution_clock::now();
    // ... operation ...
    auto end = chronovyan::high_resolution_clock::now();
    auto ns = chronovyan::duration_cast<chronovyan::nanoseconds>(end - start);

    // Network timeout (human-readable)
    constexpr auto timeout = 500ms;  // More readable than 0.5s or 500'000µs

    // Calendar event
    auto meeting_duration = 1h + 30min;  // Self-documenting

    // Frame timing (60 FPS)
    constexpr auto frame_time = 16.666ms;  // 1/60 second
```text

###

``mermaid
    gantt
        title Duration Conversion Overhead
        dateFormat  HH:mm:ss.SSS
        axisFormat %S.%Lms

        section No Conversion
        Operation :a1, 0s, 1s

        section With Conversion
        Convert :b1, 0s, 1s
        Operation :b2, after b1, 1s
```text**Prefer**using the natural duration type for your operations
-**Avoid**unnecessary `duration_cast` operations
-**Use**floating-point durations for division/multiplication when exact precision isn't critical
-**Consider**integer durations for exact arithmetic and comparisons

### Common Pitfalls

1.**Mixing Duration Types**```cpp
    // ❌ Error: Can't add different duration types
    auto total = 1s + 500ms;  // Error

    // ✅ Correct: Explicit conversion
    auto total = 1s + chrono::duration_cast<seconds>(500ms);
```text

    2.**Loss of Precision**```cpp
    // ❌ Loses sub-second precision
    seconds s = 1500ms;  // s.count() == 1

    // ✅ Preserves precision
    auto ms = 1500ms;    // 1.5 seconds exactly
```text

    3.**Floating-point Durations**
```cpp
       // Good for calculations requiring division
       using fsec = duration<double>;

    std::sort(time_series.begin(), time_series.end());

    // For lookups by time
    std::map<time_point<system_clock>, Event> events;
    auto it = events.lower_bound(some_time);
```text

### 6. Memory Pooling

For high-frequency time point allocations:
```cpp
    class TimePointPool {
        std::vector<std::unique_ptr<time_point<system_clock>>> pool_;

    public:
        time_point<system_clock>* acquire() {
            if (pool_.empty()) {
                return new time_point<system_clock>();
            }
            auto ptr = std::move(pool_.back());
            pool_.pop_back();
            return ptr.release();
        }

        void release(time_point<system_clock>* tp) {
            pool_.push_back(std::unique_ptr<time_point<system_clock>>(tp));
        }
    };

    ##

``mermaid
    pie
        title Thread Safety Concerns
        "Race Conditions" : 45
        "Deadlocks" : 25
        "False Sharing" : 20
        "Cache Invalidation" : 10
```text

### 1. Thread-Local Caching

####

``mermaid
    flowchart LR
        A[Thread 1] -->|Lock| B[Global Cache]
        C[Thread 2] -->|Wait| B
        D[Thread 3] -->|Wait| B
```
```cpp
    // Global cache with mutex protection
    std::mutex cache_mutex;
    std::unordered_map<std::string, const time_zone*> global_cache;

    const time_zone* get_time_zone_slow(const std::string& name) {
        std::lock_guard<std::mutex> lock(cache_mutex);
        auto it = global_cache.find(name);
        if (it == global_cache.end()) {
            it = global_cache.emplace(name, locate_zone(name)).first;
        }
        return it->second;
    }
```text

####

``mermaid
    flowchart LR
        A[Thread 1] --> B[Thread 1 Cache]
        C[Thread 2] --> D[Thread 2 Cache]
        E[Thread 3] --> F[Thread 3 Cache]
```
```cpp
    // Each thread gets its own cache
    const time_zone* get_cached_time_zone(const std::string& name) {
        thread_local std::unordered_map<std::string, const time_zone*> cache;

        auto it = cache.find(name);
        if (it == cache.end()) {
            it = cache.emplace(name, locate_zone(name)).first;
        }
        return it->second;
    }
```text

###

``mermaid
    classDiagram
        class time_point {
            +duration time_since_epoch()
            +time_point& operator+=(const duration&)
            +static time_point min()
            +static time_point max()
        }

        class zoned_time {
            -const time_zone* zone_
            -sys_time time_
            +zoned_time()
            +zoned_time(const time_zone*, const sys_time&)
            +sys_time get_sys_time()
            +local_time get_local_time()
        }

        time_point <|-- zoned_time
```text`time_point` and`zoned_time` are thread-safe for concurrent reads
- No synchronization needed for const operations
- For modifications, external synchronization is required

### 3. Thread Safety Patterns

####

``mermaid
    sequenceDiagram
        participant Reader1
        participant Reader2
        participant Writer
        participant Data

        Reader1->>Data: Read (no lock)
        Reader2->>Data: Read (no lock)
        Writer->>Data: Create new copy
        Writer->>Data: Atomic swap
        Reader1->>Data: Read new version
```text

####

``cpp
    class TimeZoneCache {
        std::shared_ptr<const std::unordered_map<std::string, const time_zone*>> cache_;

    public:
        TimeZoneCache() : cache_(std::make_shared<std::unordered_map<std::string, const time_zone*>>()) {}

        void update() {
            auto new_cache = std::make_shared<std::unordered_map<std::string, const time_zone*>>(*cache_);
            // Update new_cache...
            std::atomic_store(&cache_, new_cache);
        }

        const time_zone* get(const std::string& name) const {
            auto cache = std::atomic_load(&cache_);
            auto it = cache->find(name);
            return it != cache->end() ? it->second : nullptr;
        }
    };
```text

###

``mermaid
    gantt
        title False Sharing Impact
        dateFormat  HH:mm:ss.SSS
        axisFormat %S.%Lms

        section With False Sharing
        Thread 1 :a1, 0s, 2s
        Thread 2 :a2, 0s, 2s

        section Without False Sharing
        Thread 1 :b1, 3s, 1s
        Thread 2 :b2, 3s, 1s
```text

####

``cpp
    struct alignas(64) PaddedCounter {
        std::atomic<int> counter;
        char padding[64 - sizeof(std::atomic<int>)];  // Pad to cache line size
    };

    // Array of counters, each on its own cache line
    PaddedCounter counters[std::thread::hardware_concurrency()];
```text

###

``mermaid
    flowchart TD
        A[Get Time Zone] --> B{In Cache?}
        B -->|Yes| C[Return Cached]
        B -->|No| D[Create New]
        D --> E[Add to Cache]
        E --> C
```text

####

``cpp
    class TimeZoneFactory {
        mutable std::mutex mutex_;
        std::unordered_map<std::string, std::weak_ptr<const time_zone>> cache_;

    public:
        std::shared_ptr<const time_zone> get_time_zone(const std::string& name) {
            std::lock_guard<std::mutex> lock(mutex_);

            // Check if we have a valid entry in cache
            if (auto it = cache_.find(name); it != cache_.end()) {
                if (auto sp = it->second.lock()) {
                    return sp;  // Return existing shared_ptr
                }
                // Entry exists but expired, remove it
                cache_.erase(it);
            }

            // Create new entry
            auto tz = std::shared_ptr<const time_zone>(
                locate_zone(name),
                [this, name](/)c)o)n)s)t) time_zone*tz) {
                    std::lock_guard<std::mutex> lock(mutex_);
                    cache_.erase(name);
                    delete tz;
                }
            );

            cache_[name] = tz;
            return tz;
        }
    };
```text

##

``mermaid
    pie
        title Compile-Time Benefits
        "Faster Runtime" : 40
        "Smaller Binary" : 30
        "Better Optimizations" : 30
```text

### 1. Compile-Time Calculations

####

``mermaid
    flowchart LR
        A[Program Start] --> B[Calculate at Runtime]
        B --> C[Use Result]
        C --> D[Program End]
```
```cpp
    // Calculated every time the function is called
    auto get_week_later() {
        return system_clock::now() + 7* 24h;
    }
```text

####

``mermaid
    flowchart LR
        A[Compile Time] --> B[Calculate Value]
        C[Runtime] --> D[Use Precomputed Value]
```
```cpp
    // Compile-time duration calculations
    constexpr auto one_day = 24h;
    constexpr auto one_week = 7 *one_day;
    constexpr auto one_year = 365* one_day;

    // Compile-time time point calculations
    constexpr auto epoch = system_clock::time_point{};
    constexpr auto year_2000 = epoch + 946'684'800s;  // 2000-01-01 00:00:00 UTC

    // Known at compile time
    template<typename Clock>
    constexpr auto get_week_later() {
        return Clock::now() + one_week;
    }
```text

### 2. Type Aliases and Constants

####

``mermaid
    classDiagram
        class duration {
            <<templated>>
            +rep count()
            +static duration zero()
        }

        class time_point {
            <<templated>>
            +duration time_since_epoch()
        }

        duration <|-- seconds
        duration <|-- milliseconds
        time_point <|-- sys_time
        time_point <|-- local_time

        note for time_point "Commonly used aliases:\n- sys_time: System clock time\n- local_time: Local time"
```text

####

``cpp
    // Duration types
    using days = duration<int, std::ratio<86400>>;
    using weeks = duration<int, std::ratio<604800>>;
    using months = duration<int, std::ratio<2629746>>;  // 30.44 days
    using years = duration<int, std::ratio<31556952>>;  // 365.24 days

    // Time point types
    using sys_time = time_point<system_clock>;
    using steady_time = time_point<steady_clock>;
    using highres_time = time_point<high_resolution_clock>;
```text

### 3. Compile-Time String Processing

####

``mermaid
    flowchart TD
        A[Compile Time] --> B[Validate Format String]
        B -->|Valid| C[Compile Success]
        B -->|Invalid| D[Compile Error]
```
```cpp
    template<typename... Args>
    constexpr void validate_format_string(const char*fmt) {
        // Compile-time format string validation
        // Will fail to compile if format string is invalid
        chronovyan::parse(fmt, Args{}...);
    }

    // Usage:
    // validate_format_string<year, month, day>("%Y-%m-%d");  // OK
    // validate_format_string<year, month, day>("%Y-%m");    // Compile error
```text

### 4. Compile-Time Date Calculations

####

``mermaid
    flowchart LR
        A[Year] --> B{Is Leap Year?}
        B -->|Yes| C[366 Days]
        B -->|No| D[365 Days]
```
```cpp
    constexpr bool is_leap_year(int y) {
        return (y % 4 == 0 && y % 100 != 0) | (y % 400 == 0);
    }

    constexpr int days_in_month(int y, unsigned m) {
        if (m == 2) return is_leap_year(y) ? 29 : 28;
        constexpr int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return days[m - 1];
    }

    // Compile-time validation
    static_assert(days_in_month(2020, 2) == 29, "2020 was a leap year");
    static_assert(days_in_month(2021, 2) == 28, "2021 is not a leap year");
```text

###

``mermaid
    gantt
        title Runtime vs Compile-Time Calculation
        dateFormat  HH:mm:ss.SSS
        axisFormat %S.%Lms

        section Runtime
        Calculate :a1, 0s, 5s

        section Compile-Time
        Use Value :b1, 0s, 1s
```text**Zero runtime overhead**for compile-time calculations
-**Better optimization**opportunities for the compiler
-**Smaller binary size**due to constant folding
-**Early error detection** at compile time

###

``cpp
    // Type-safe duration units
    template<typename Rep, typename Period>
    class duration {
        Rep rep_;
    public:
        constexpr explicit duration(const Rep& r) : rep_(r) {}
        constexpr Rep count() const { return rep_; }
        // ...
    };

    // User-defined literals
    constexpr auto operator""_days(unsigned long long d) {
        return duration<int, std::ratio<86400>>(d);
    }

    // Usage:
    constexpr auto two_weeks = 14_days;  // Type-safe, checked at compile time
```text

###

``cpp
    // Compile-time format string processing
    template<typename... Args>
    constexpr auto make_format_string() {
        constexpr const char*fmt = "%Y-%m-%d %H:%M:%S";
        // Validate format string at compile time
        static_assert(validate_format_string<Args...>(fmt), "Invalid format string");
        return fmt;
    }

    // Usage:
    auto fmt = make_format_string<year, month, day, hours, minutes, seconds>();
```text

##

``mermaid
    pie
        title Benchmarking Focus Areas
        "CPU Time" : 40
        "Memory Usage" : 30
        "Cache Behavior" : 20
        "Thread Contention" : 10
```text

### 1. Benchmarking with Google Benchmark

####

``mermaid
    flowchart TD
        A[Setup] --> B[Start Timer]
        B --> C[Run Test Code]
        C --> D[Stop Timer]
        D --> E[Report Results]
```text

####

``cpp
    #include <benchmark/benchmark.h>

    // Benchmark time formatting
    static void BM_TimeFormatting(benchmark::State& state) {
        // Setup (not measured)
        auto now = chronovyan::system_clock::now();
        std::string buffer;
        buffer.reserve(32);

        // Benchmark loop
        for (auto _ : state) {
            buffer.clear();
            // Format time to string
            chronovyan::format_to(
                std::back_inserter(buffer),
                "%Y-%m-%d %H:%M:%S",
                now
            );
            // Prevent compiler optimizations
            benchmark::DoNotOptimize(buffer);
        }

        // Optional: Set custom metrics
        state.SetBytesProcessed(
            int64_t(state.iterations())* buffer.size()
        );
    }

    // Register benchmark
    BENCHMARK(BM_TimeFormatting);
```text

### 2. Benchmarking Different Approaches

####

``mermaid
    gantt
        title Benchmark Comparison
        dateFormat  HH:mm:ss.SSS
        axisFormat %S.%Lms

        section Implementation A
        Run 1 :a1, 0s, 2s
        Run 2 :a2, after a1, 2s

        section Implementation B
        Run 1 :b1, 0s, 1s
        Run 2 :b2, after b1, 1s
```
```cpp
    // Compare different formatting approaches
    static void BM_Format_vs_Manual(benchmark::State& state) {
        auto now = chronovyan::system_clock::now();
        std::string buffer;
        buffer.reserve(32);

        for (auto _ : state) {
            // Method 1: Using format_to
            buffer.clear();
            chronovyan::format_to(
                std::back_inserter(buffer),
                "%Y-%m-%d %H:%M:%S",
                now
            );
            benchmark::DoNotOptimize(buffer);
        }
    }

    static void BM_Format_Manual(benchmark::State& state) {
        auto now = chronovyan::system_clock::now();
        std::string buffer;
        buffer.reserve(32);

        for (auto _ : state) {
            // Method 2: Manual formatting
            buffer.clear();
            auto dp = chronovyan::floor<chronovyan::days>(now);
            chronovyan::year_month_weekday ymd{chronovyan::sys_days{dp}};
            // ... manual formatting ...
            benchmark::DoNotOptimize(buffer);
        }
    }

    BENCHMARK(BM_Format_vs_Manual);
    BENCHMARK(BM_Format_Manual);
```text

### 3. Memory Benchmarking

####

``mermaid
    classDiagram
        class MemoryTracker {
            +size_t current_usage()
            +size_t peak_usage()
            +void reset_peak()
        }

        class BenchmarkState {
            +size_t bytes_processed()
            +size_t items_processed()
            +void SetBytesProcessed(int64_t)
        }

        MemoryTracker <|-- BenchmarkState
```text

####

``cpp
    #include <benchmark/benchmark.h>

    static void BM_TimeParsing_Memory(benchmark::State& state) {
        const char*time_str = "2023-01-15 14:30:00";

        // Track memory allocations
        auto start_mem = GetCurrentRSS();  // Platform-specific memory function

        for (auto _ : state) {
            chronovyan::sys_seconds tp;
            std::istringstream ss(time_str);
            ss >> chronovyan::parse("%Y-%m-%d %H:%M:%S", tp);
            benchmark::DoNotOptimize(tp);
        }

        auto end_mem = GetCurrentRSS();
        state.counters["Memory"] = end_mem - start_mem;
    }

    BENCHMARK(BM_TimeParsing_Memory);
```text

### 4. Cache Behavior Analysis

####

``mermaid
    gantt
        title Cache Miss Impact
        dateFormat  HH:mm:ss.SSS
        axisFormat %S.%Lms

        section L1 Cache
        Access :a1, 0s, 1s

        section L2 Cache
        Access :a2, after a1, 2s

        section RAM
        Access :a3, after a2, 10s
```text

####

``cpp
    static void BM_CacheBehavior(benchmark::State& state) {
        // Create a large vector of time points
        std::vector<chronovyan::system_clock::time_point> times(state.range(0));
        auto now = chronovyan::system_clock::now();

        // Initialize with sequential times
        for (size_t i = 0; i < times.size(); ++i) {
            times[i] = now + chronovyan::seconds(i);
        }

        // Shuffle for random access pattern
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(times.begin(), times.end(), g);

        // Benchmark random access
        size_t sum = 0;
        for (auto _ : state) {
            for (const auto& t : times) {
                sum += t.time_since_epoch().count();
            }
        }

        state.counters["TimePerItem"] =
            benchmark::Counter(
                state.iterations()* times.size(),
                benchmark::Counter::kIsRate
            );
    }

    // Test with different container sizes
    BENCHMARK(BM_CacheBehavior)->RangeMultiplier(2)->Range(1<<10, 1<<20);
```text

### 5. Thread Scaling Benchmarks

####

``mermaid
    flowchart TD
        A[Single Thread] -->|2x Cores| B[2x Speedup?]
        B -->|No| C[Bottleneck Found]
        B -->|Yes| D[Continue Scaling]
```text

####

``cpp
    static void BM_ThreadScaling(benchmark::State& state) {
        const size_t num_times = 1'000'000;
        std::vector<chronovyan::system_clock::time_point> times(num_times);
        auto now = chronovyan::system_clock::now();

        // Initialize test data
        for (size_t i = 0; i < num_times; ++i) {
            times[i] = now + chronovyan::milliseconds(i % 1000);
        }

        // Process times in parallel
        auto process_times = [&]() {
            for (auto _ : state) {
                for (const auto& t : times) {
                    auto value = t.time_since_epoch().count();
                    benchmark::DoNotOptimize(value);
                }
            }
        };

        // Execute with different thread counts
        if (state.thread_index() == 0) {
            // Single-threaded baseline
            process_times();
        } else {
            // Multi-threaded
            std::vector<std::thread> threads;
            for (int i = 0; i < state.threads() - 1; ++i) {
                threads.emplace_back(process_times);
            }
            process_times();  // Use main thread too
            for (auto& t : threads) t.join();
        }
    }

    // Test with 1-8 threads
    BENCHMARK(BM_ThreadScaling)->ThreadRange(1, 8);
```text

### 6. Benchmark Analysis and Reporting

#### Performance Metrics

| Metric | Description | Importance | --- |-------------| --- | Time/Op | Time per operation | Primary performance indicator | Ops/sec | Operations per second | Throughput measurement | CPU % | CPU utilization | Efficiency indicator | Mem/Op | Memory per operation | Memory efficiency | Cache-misses | Cache miss rate | Memory access pattern |

####

``bash
    #

   ./benchmark --benchmark_format=console \
                --benchmark_out=results.json \
                --benchmark_out_format=json

    #

   python3 -m pip install pandas matplotlib
    python3 -c "
    import pandas as pd
    import matplotlib.pyplot as plt

    #

   df = pd.read_json('results.json', lines=True)
    df = df[df['name'].str.contains('BM_')]

    #

   plt.figure(figsize=(12, 6))
    for name, group in df.groupby('name'):
        plt.plot(
            group['threads'],
            group['real_time'],
            'o-',
            label=name
        )

    plt.xlabel('Threads')
    plt.ylabel('Time (ns)')
    plt.title('Thread Scaling Performance')
    plt.legend()
    plt.grid(True)
    plt.savefig('benchmark_results.png')
    "
```text

### 7. Benchmarking Best Practices

1. **Warm-up Runs**```cpp
    // First few iterations might be slower due to cache warming
    BENCHMARK(BM_Test)->MinWarmUpTime(1.0);
```text

    2.**Iteration Count**```cpp
    // Run enough iterations for stable measurements
    BENCHMARK(BM_Test)->MinTime(5.0);  // Run for at least 5 seconds
```text

    3.**Parameterized Tests**```cpp
    static void BM_Parameterized(benchmark::State& state) {
        auto size = state.range(0);
        // Test with different container sizes
    }
    BENCHMARK(BM_Parameterized)->RangeMultiplier(2)->Range(8, 8<<10);
```text

    4.**Memory Access Patterns**```mermaid
    flowchart LR
        A[Sequential] -->|Best| B[Cache Friendly]
        C[Random] -->|Worst| D[Cache Thrashing]
        E[Strided] -->|Depends| F[Stride Length]
```text

    5.**Avoiding Optimization**```cpp
    // Prevent dead code elimination
    benchmark::DoNotOptimize(value);

    // Force memory writes
    benchmark::ClobberMemory();
```text

    6.**Measuring Real-world Scenarios**```cpp
    // Simulate real-world access patterns
    std::vector<TimePoint> times = generate_realistic_workload();
    BENCHMARK_CAPTURE(BM_RealWorld, workload1, std::move(times));
```text
    }
    BENCHMARK(BM_TimeFormatting);

    BENCHMARK_MAIN();
```text

###

``cpp
    static void BM_TimeZoneConversion(benchmark::State& state) {
        auto tz = chronovyan::locate_zone("America/New_York");
        auto now = chronovyan::system_clock::now();

        for (auto _ : state) {
            auto zt = chronovyan::zoned_time{tz, now};
            benchmark::DoNotOptimize(zt);
        }

        state.SetItemsProcessed(state.iterations());
    }
    BENCHMARK(BM_TimeZoneConversion);
```text

## Real-World Examples

###

``cpp
    class Logger {
        std::ofstream log_file;
        std::string buffer;

    public:
        void log(std::string_view message) {
            buffer.clear();
            auto now = system_clock::now();

            // Format timestamp
            format_to(std::back_inserter(buffer),
                     "[%Y-%m-%d %H:%M:%S] ",
                     zoned_time{current_zone(), now});

            // Append message
            buffer.append(message);
            buffer.push_back('\n');

            // Write to file
            log_file.write(buffer.data(), buffer.size());
        }
    };
```text

###

``cpp
    struct TimeSeries {
        struct Point {
            system_clock::time_point time;
            double value;
        };

        std::vector<Point> points;

        // Find all points in a time range \1CONST\2/core/Core Concepts - The Foundation of Temporal Programming.md#const\3c)o)n)s)t) Point&) p,) system_clock::time_point) t) {
                    return p.time < t;
                });
        }

        // Resample to a fixed interval
        TimeSeries resample(seconds interval) const {
            if (points.empty()) return {};

            TimeSeries result;
            auto current = points.front().time;
            const auto end_time = points.back().time;

            while (current <= end_time) {
                auto next = current + interval;
                auto [first, last] = find_in_range(current, next);

                if (first != last) {
                    // Simple average of values in the interval
                    double sum = 0;
                    size_t count = 0;
                    for (; first != last; ++first) {
                        sum += first->value;
                        ++count;
                    }
                    result.points.push_back({current, sum / count});
                }

                current = next;
            }

            return result;
        }
    };
```text

## Conclusion

Optimizing time-related code requires careful consideration of both algorithmic efficiency and practical constraints. By following these guidelines and measuring performance in your specific use case, you can achieve optimal performance with Chronovyan.

Remember to always profile your application to identify actual bottlenecks before optimizing. In many cases, the biggest performance gains come from higher-level architectural decisions rather than micro-optimizations of time operations.
    }
}
BENCHMARK(BM_TimeFormatting);

BENCHMARK_MAIN();
```text

## Memory Usage

Be aware of the memory footprint:

 `time_point<system_clock>`: Typically 8 bytes (64-bit)
- `time_point<system_clock, nanoseconds>`: 16 bytes (for higher precision)
- `zoned_time`: Larger due to time zone data

##

 **Not thread-safe**: Time zone database operations (use locks if modifying)

## Profiling Tips

1. Profile your application to identify time-related bottlenecks
2. Look for:
    Excessive time zone conversions
    String formatting in tight loops
    Unnecessary time point copies
    Suboptimal duration types
