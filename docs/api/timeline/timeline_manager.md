# Timeline Manager API

## Overview
The Timeline Manager is a central component in Chronovyan that coordinates multiple timelines, manages their lifecycle, and provides synchronization primitives. It enables complex temporal workflows and ensures consistent time management across the application.

## Core Features

- **Timeline Management**: Create, start, pause, and stop timelines
- **Temporal Synchronization**: Coordinate events across multiple timelines
- **Resource Allocation**: Manage shared resources between timelines
- **Time Scaling**: Adjust the speed of time for individual timelines
- **Event Propagation**: Control how events flow between timelines

## API Reference

### Classes

#### TimelineManager
Main class for managing timelines.

```cpp
class TimelineManager {
public:
    // Singleton access
    static TimelineManager& instance();
    
    // Timeline management
    TimelineHandle create_timeline(const std::string& name = "");
    bool remove_timeline(const TimelineHandle& handle);
    
    // Timeline control
    void start(const TimelineHandle& handle);
    void pause(const TimelineHandle& handle);
    void stop(const TimelineHandle& handle);
    void join(const TimelineHandle& handle);
    
    // Time manipulation
    void set_time_scale(const TimelineHandle& handle, double scale);
    void seek(const TimelineHandle& handle, TimePoint position);
    
    // Event handling
    EventHandle register_event_handler(
        const TimelineHandle& handle,
        EventType type,
        EventHandler handler
    );
    void unregister_event_handler(const EventHandle& handle);
    
    // Query
    std::vector<TimelineInfo> list_timelines() const;
    TimelineStatus get_status(const TimelineHandle& handle) const;
};
```

### Types

#### TimelineHandle
```cpp
class TimelineHandle {
    std::string id;
    // ...
};

struct TimelineInfo {
    TimelineHandle handle;
    std::string name;
    TimelineStatus status;
    TimePoint current_time;
    double time_scale;
};

enum class TimelineStatus {
    STOPPED,
    RUNNING,
    PAUSED,
    ERROR
};
```

## Usage Examples

### Basic Timeline Management

```cpp
// Get the timeline manager instance
auto& manager = TimelineManager::instance();

// Create a new timeline
auto timeline = manager.create_timeline("background_processing");

// Add some events
manager.register_event_handler(timeline, EventType::UPDATE, [](const Event& e) {
    // Handle update event
});

// Start the timeline
manager.start(timeline);

// Later...
manager.pause(timeline);
manager.seek(timeline, seconds(10));  // Jump to 10 seconds
manager.start(timeline);

// Clean up
manager.stop(timeline);
manager.join(timeline);
manager.remove_timeline(timeline);
```

### Multiple Timelines

```cpp
// Create multiple timelines
auto physics_timeline = manager.create_timeline("physics");
auto render_timeline = manager.create_timeline("rendering");

// Run physics at fixed timestep
manager.register_event_handler(physics_timeline, EventType::FIXED_UPDATE, [](const Event& e) {
    // Fixed timestep physics update
});

// Run rendering as fast as possible
manager.register_event_handler(render_timeline, EventType::UPDATE, [](const Event& e) {
    // Render frame
});

// Start both timelines
manager.start(physics_timeline);
manager.start(render_timeline);
```

## Advanced Features

### Timeline Synchronization

```cpp
// Create a master timeline and a slave timeline
auto master = manager.create_timeline("master");
auto slave = manager.create_timeline("slave");

// Synchronize slave to master
manager.register_event_handler(master, EventType::UPDATE, [&](const Event& e) {
    manager.seek(slave, e.time);
});
```

### Time Scaling

```cpp
// Create a slow-motion timeline
auto slow_mo = manager.create_timeline("slow_motion");
manager.set_time_scale(slow_mo, 0.5);  // Run at half speed

// Create a fast-forward timeline
auto fast_forward = manager.create_timeline("fast_forward");
manager.set_time_scale(fast_forward, 2.0);  // Run at double speed
```

### Event Filtering

```cpp
// Create a filtered timeline
auto filtered = manager.create_timeline("filtered");

// Only process events that pass the filter
manager.register_event_handler(filtered, EventType::ALL, [](const Event& e) {
    return e.priority > 50;  // Only high-priority events
}, EventHandlerOptions{
    .filter = true
});
```

## Performance Considerations

- **Thread Safety**: All public methods are thread-safe
- **Event Processing**: Event handlers should be fast to avoid blocking
- **Memory Usage**: Each timeline has a small memory overhead
- **Time Precision**: High-precision timing is available but may have platform-specific limitations

## Best Practices

1. **Use Descriptive Names**: Name timelines meaningfully
2. **Clean Up**: Always stop and remove timelines when done
3. **Handle Errors**: Check return values and handle errors appropriately
4. **Monitor Performance**: Keep an eye on CPU usage with many active timelines
5. **Use Appropriate Time Scales**: Be mindful of the performance impact of time scaling

## Related Documentation

- [Timeline API](../timeline/)
- [Event System](../events/)
- [Resource Management](../resource_management/)
- [Performance Optimization](../../guides/performance.md)
