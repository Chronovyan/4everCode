# Timeline Trader API

## Overview
The Timeline Trader is a sophisticated component in Chronovyan's resource management system that handles the trading and arbitration of temporal resources across different timelines. It ensures fair and efficient allocation of shared resources in time-sensitive applications.

## Core Features

- **Temporal Resource Trading**: Exchange resources between different timelines
- **Deadline-Aware Scheduling**: Prioritize resource allocation based on temporal constraints
- **Conflict Resolution**: Automatic resolution of resource contention
- **Load Balancing**: Distribute resource load across available timelines

## API Reference

### Classes

#### TimelineTrader
Main class for managing timeline-based resource trading.

```cpp
class TimelineTrader {
public:
    // Constructor
    TimelineTrader(ResourcePool& pool);
    
    // Request resources with a deadline
    ResourceLease request_resources(
        const std::string& resource_type,
        int amount,
        TimePoint deadline
    );
    
    // Release resources back to the pool
    void release_resources(ResourceLease& lease);
    
    // Get current resource utilization
    ResourceUtilization get_utilization() const;
    
    // Set trading strategy
    void set_trading_strategy(std::unique_ptr<TradingStrategy> strategy);
};
```

### Types

#### ResourceLease
Represents an active lease of resources.

```cpp
class ResourceLease {
public:
    // Check if lease is valid
    bool is_valid() const;
    
    // Get leased resources
    const ResourceHandle& get_resources() const;
    
    // Get lease expiration time
    TimePoint get_expiration() const;
};
```

### Enums

#### TradingStrategyType
```cpp
enum class TradingStrategyType {
    FAIR_SHARE,     // Equal distribution of resources
    PRIORITY_BASED, // Based on timeline priority
    DEADLINE_AWARE, // Based on temporal constraints
    HYBRID          // Combination of multiple strategies
};
```

## Usage Examples

### Basic Resource Trading

```cpp
// Create a resource pool
ResourcePool pool({
    {"cpu", 16},  // 16 CPU cores
    {"gpu", 2},   // 2 GPUs
    {"mem", 32768} // 32GB memory
});

// Create a timeline trader
TimelineTrader trader(pool);

// Request resources with a deadline
auto lease = trader.request_resources(
    "cpu",  // resource type
    4,      // amount
    now() + std::chrono::seconds(10)  // deadline
);

if (lease.is_valid()) {
    // Use the resources
    process_data(lease.get_resources());
    
    // Release when done
    trader.release_resources(lease);
}
```

### Advanced: Custom Trading Strategy

```cpp
class CustomTradingStrategy : public TradingStrategy {
public:
    ResourceAllocation decide(
        const ResourceRequest& request,
        const ResourceState& state
    ) override {
        // Implement custom trading logic
        if (is_high_priority(request.requester_id)) {
            return allocate_for_high_priority(request, state);
        }
        return allocate_fair_share(request, state);
    }
};

// Set custom strategy
trader.set_trading_strategy(std::make_unique<CustomTradingStrategy>());
```

## Error Handling

All API methods throw `std::runtime_error` for critical errors. Resource requests may also throw `ResourceUnavailableException` when resources cannot be allocated.

```cpp
try {
    auto lease = trader.request_resources("gpu", 4, deadline);
    // Use resources...
} catch (const ResourceUnavailableException& e) {
    // Handle resource allocation failure
    logger.error("Failed to allocate resources: {}", e.what());
} catch (const std::exception& e) {
    // Handle other errors
    logger.error("Error: {}", e.what());
}
```

## Performance Considerations

- **Thread Safety**: All public methods are thread-safe
- **Time Complexity**:
  - `request_resources`: O(log n) for n active leases
  - `release_resources`: O(1) amortized
- **Memory Usage**: O(m + n) where m is the number of resource types and n is the number of active leases

## Related Documentation

- [Resource Management Guide](../guides/resource_management.md)
- [API Reference: Resource Pool](./resource_pool.md)
- [Performance Tuning](../guides/performance.md)
