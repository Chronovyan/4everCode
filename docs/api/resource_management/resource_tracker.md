# Resource Tracker API

## Overview
The Resource Tracker is a core component of Chronovyan's resource management system, providing real-time monitoring and tracking of resource usage across the application. It helps identify bottlenecks, detect leaks, and optimize resource utilization.

## Core Features

- **Real-time Monitoring**: Track resource usage with nanosecond precision
- **Leak Detection**: Identify and report resource leaks
- **Historical Analysis**: Analyze resource usage patterns over time
- **Threshold Alerts**: Get notified when resource usage exceeds defined thresholds
- **Multi-dimensional Metrics**: Track CPU, memory, I/O, and custom resources

## API Reference

### Classes

#### ResourceTracker
Main class for tracking resource usage.

```cpp
class ResourceTracker {
public:
    // Singleton access
    static ResourceTracker& instance();
    
    // Start tracking a resource
    ResourceHandle track(
        const std::string& name,
        const std::string& type = "custom"
    );
    
    // Record a resource usage sample
    void record_usage(
        const ResourceHandle& handle,
        double value,
        TimePoint timestamp = Clock::now()
    );
    
    // Get current usage statistics
    ResourceStats get_stats(const std::string& name) const;
    
    // Set alert threshold
    void set_threshold(
        const std::string& name,
        double threshold,
        std::function<void(const Alert&)> callback
    );
    
    // Generate report
    std::string generate_report(ReportFormat format = ReportFormat::TEXT) const;
};
```

### Types

#### ResourceHandle
```cpp
class ResourceHandle {
    std::string name;
    std::string type;
    // ...
};

struct ResourceStats {
    double min;
    double max;
    double average;
    double current;
    TimePoint last_updated;
    // ...
};

struct Alert {
    std::string resource_name;
    double threshold;
    double current_value;
    TimePoint timestamp;
    std::string message;
};

enum class ReportFormat {
    TEXT,
    JSON,
    CSV
};
```

## Usage Examples

### Basic Resource Tracking

```cpp
// Get the tracker instance
auto& tracker = ResourceTracker::instance();

// Track a resource
auto handle = tracker.track("database_connections", "connection");

// Record usage
tracker.record_usage(handle, 5);  // 5 active connections

// Later...
tracker.record_usage(handle, 3);  // 3 active connections
```

### Setting Up Alerts

```cpp
// Set a threshold alert
tracker.set_threshold(
    "memory_usage",
    90.0,  // 90% threshold
    [](const Alert& alert) {
        std::cerr << "WARNING: Memory usage at " 
                  << alert.current_value << "%" << std::endl;
    }
);
```

### Generating Reports

```cpp
// Generate a text report
std::cout << tracker.generate_report(ReportFormat::TEXT) << std::endl;

// Or JSON for programmatic processing
std::string json_report = tracker.generate_report(ReportFormat::JSON);
```

## Advanced Usage

### Custom Resource Types

```cpp
// Define a custom resource type
struct GpuMemory {
    size_t used_mb;
    size_t total_mb;
    float utilization() const {
        return static_cast<float>(used_mb) / total_mb * 100.0f;
    }
};

// Track GPU memory usage
auto gpu_handle = tracker.track("gpu_mem", "gpu");
GpuMemory gpu_mem = get_gpu_memory_usage();
tracker.record_usage(gpu_handle, gpu_mem.utilization());
```

### Thread-Safe Tracking

```cpp
// The tracker is thread-safe, so you can record from multiple threads
void worker_thread(ResourceTracker& tracker) {
    auto handle = tracker.track("worker_threads", "thread");
    while (running) {
        tracker.record_usage(handle, 1.0);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    tracker.record_usage(handle, 0.0);
}
```

## Performance Considerations

- **Overhead**: Minimal overhead for tracking (typically < 1% CPU)
- **Memory Usage**: Approximately 100 bytes per tracked metric
- **Thread Safety**: All public methods are thread-safe
- **Sampling Rate**: Default sampling rate is 1 second, adjustable via configuration

## Integration

### With Timeline System

```cpp
// Create a timeline with resource tracking
Timeline timeline;
ResourceTracker& tracker = ResourceTracker::instance();

timeline.add_event("process_data", [&]() {
    auto handle = tracker.track("data_processing", "cpu");
    tracker.record_usage(handle, 100.0);  // 100% CPU usage
    
    // Process data...
    
    tracker.record_usage(handle, 0.0);  // Done
});
```

### With Resource Pool

```cpp
ResourcePool pool({"cpu", "gpu", "memory"});
ResourceTracker& tracker = ResourceTracker::instance();

// Track pool usage
pool.set_usage_callback([&](const std::string& resource, double usage) {
    static std::unordered_map<std::string, ResourceHandle> handles;
    if (handles.find(resource) == handles.end()) {
        handles[resource] = tracker.track("pool_" + resource, resource);
    }
    tracker.record_usage(handles[resource], usage);
});
```

## Best Practices

1. **Use Meaningful Names**: Choose descriptive names for tracked resources
2. **Monitor Key Metrics**: Focus on critical resources first
3. **Set Realistic Thresholds**: Avoid alert fatigue
4. **Regularly Review Reports**: Look for usage patterns and optimize
5. **Clean Up**: Stop tracking resources when they're no longer needed

## Related Documentation

- [Resource Management Guide](../guides/resource_management.md)
- [Timeline Trader API](./timeline_trader.md)
- [Performance Tuning](../guides/performance.md)
