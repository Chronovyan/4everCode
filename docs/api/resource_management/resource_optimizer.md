# Resource Optimizer API

## Overview
The Resource Optimizer is a sophisticated component in Chronovyan's resource management system that automatically optimizes resource allocation and usage across timelines. It uses advanced algorithms to ensure efficient resource utilization while maintaining temporal consistency.

## Core Features

- **Automatic Resource Allocation**: Dynamically allocates resources based on demand
- **Load Balancing**: Distributes resources evenly across timelines
- **Predictive Optimization**: Anticipates future resource needs
- **Conflict Resolution**: Resolves resource conflicts intelligently
- **Adaptive Strategies**: Adjusts optimization strategies based on workload

## API Reference

### Classes

#### ResourceOptimizer
Main class for resource optimization.

```cpp
class ResourceOptimizer {
public:
    // Constructor
    explicit ResourceOptimizer(ResourcePool& pool);
    
    // Optimize resource allocation
    OptimizationResult optimize(
        const std::vector<ResourceRequest>& requests,
        OptimizationStrategy strategy = OptimizationStrategy::BALANCED
    );
    
    // Add a custom optimization strategy
    void add_strategy(
        const std::string& name,
        std::function<OptimizationResult(const std::vector<ResourceRequest>&)> strategy
    );
    
    // Get optimization statistics
    const OptimizationStats& get_stats() const;
    
    // Set optimization parameters
    void set_parameters(const OptimizationParams& params);
};
```

### Types

#### ResourceRequest
```cpp
struct ResourceRequest {
    std::string resource_type;
    int min_amount;
    int max_amount;
    TimePoint deadline;
    int priority;
    std::map<std::string, Variant> constraints;
};

struct OptimizationResult {
    bool success;
    std::map<std::string, int> allocations;
    std::string strategy_used;
    std::string message;
};
```

## Usage Examples

### Basic Optimization

```cpp
// Create a resource pool
ResourcePool pool({
    {"cpu", 16},
    {"gpu", 2},
    {"memory", 32768}
});

// Create the optimizer
ResourceOptimizer optimizer(pool);

// Create resource requests
std::vector<ResourceRequest> requests = {
    {"cpu", 4, 8, now() + seconds(10), 1},
    {"gpu", 1, 1, now() + seconds(5), 2},
    {"memory", 8192, 16384, now() + seconds(15), 1}
};

// Optimize resource allocation
auto result = optimizer.optimize(requests);

if (result.success) {
    // Use the allocated resources
    for (const auto& [resource, amount] : result.allocations) {
        std::cout << resource << ": " << amount << std::endl;
    }
}
```

### Custom Strategy

```cpp
// Define a custom optimization strategy
auto custom_strategy = [](const std::vector<ResourceRequest>& requests) {
    OptimizationResult result;
    // Custom optimization logic here
    return result;
};

// Add the custom strategy
optimizer.add_strategy("custom", custom_strategy);

// Use the custom strategy
auto result = optimizer.optimize(requests, "custom");
```

## Advanced Features

### Predictive Optimization

```cpp
// Enable predictive optimization
OptimizationParams params;
params.enable_predictive_optimization = true;
params.lookahead_window = minutes(30);
optimizer.set_parameters(params);

// The optimizer will now consider future resource needs
// when making allocation decisions
```

### Resource Constraints

```cpp
// Add constraints to resource requests
ResourceRequest request;
request.resource_type = "gpu";
request.min_amount = 1;
request.max_amount = 1;
request.constraints["architecture"] = "cuda";
request.constraints["memory"] = 4096;  // Minimum GPU memory in MB

// The optimizer will only allocate resources that meet all constraints
```

## Performance Considerations

- **Time Complexity**: Varies based on strategy (typically O(n log n) to O(n²))
- **Memory Usage**: Proportional to the number of resource types and requests
- **Thread Safety**: All public methods are thread-safe
- **Caching**: Results are cached when possible for better performance

## Best Practices

1. **Use Appropriate Strategies**: Choose the right strategy for your workload
2. **Set Realistic Constraints**: Avoid over-constraining resource requests
3. **Monitor Performance**: Keep an eye on optimization metrics
4. **Update Parameters**: Adjust parameters based on changing workloads
5. **Handle Failures Gracefully**: Always check the optimization result

## Related Documentation

- [Resource Management Guide](../guides/resource_management.md)
- [Resource Tracker API](./resource_tracker.md)
- [Timeline Trader API](./timeline_trader.md)
