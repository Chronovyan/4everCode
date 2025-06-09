# Temporal Debt Tracker API

## Overview
The Temporal Debt Tracker is a specialized component that monitors and manages temporal debt - a concept representing the imbalance between resource allocation and actual usage across different timelines. It helps prevent temporal paradoxes and ensures system stability.

## Core Features

- **Debt Monitoring**: Tracks temporal debt across all timelines
- **Threshold Management**: Defines and enforces debt limits
- **Recovery Strategies**: Implements automatic debt recovery mechanisms
- **Anomaly Detection**: Identifies unusual patterns in temporal debt
- **Reporting**: Provides detailed debt metrics and analytics

## API Reference

### Classes

#### TemporalDebtTracker
Main class for tracking and managing temporal debt.

```cpp
class TemporalDebtTracker {
public:
    // Constructor
    explicit TemporalDebtTracker(const DebtTrackingConfig& config);
    
    // Record a temporal debt event
    void record_debt(
        const std::string& timeline_id,
        const TemporalDebt& debt,
        const std::string& reason = ""
    );
    
    // Get current debt level for a timeline
    double get_debt_level(const std::string& timeline_id) const;
    
    // Check if a timeline is in debt
    bool is_in_debt(const std::string& timeline_id) const;
    
    // Get debt statistics
    DebtStatistics get_statistics() const;
    
    // Add a debt threshold callback
    void add_threshold_callback(
        double threshold,
        std::function<void(const std::string&, double)> callback
    );
};
```

### Types

```cpp
struct TemporalDebt {
    double amount;                  // Amount of temporal debt
    TimePoint timestamp;            // When the debt was incurred
    std::string resource_type;      // Type of resource causing the debt
    std::string description;        // Human-readable description
    std::map<std::string, Variant> metadata;  // Additional context
};

struct DebtStatistics {
    double total_debt;              // Total debt across all timelines
    std::string max_debt_timeline;  // Timeline with maximum debt
    double max_debt_amount;         // Maximum debt amount
    TimePoint last_update;          // When statistics were last updated
};

struct DebtTrackingConfig {
    double warning_threshold = 0.7;  // Warning level (0-1)
    double critical_threshold = 0.9; // Critical level (0-1)
    TimeDuration debt_ttl = hours(24); // Time to keep debt records
    bool enable_auto_recovery = true; // Enable automatic debt recovery
};
```

## Usage Examples

### Basic Debt Tracking

```cpp
// Create a debt tracker with default config
DebtTrackingConfig config;
config.warning_threshold = 0.6;
config.critical_threshold = 0.9;

TemporalDebtTracker tracker(config);

// Record a debt event
tracker.record_debt(
    "timeline_42",
    {
        .amount = 150.5,
        .timestamp = now(),
        .resource_type = "cpu_cycles",
        .description = "Excessive loop iterations"
    }
);

// Check debt level
double debt = tracker.get_debt_level("timeline_42");
if (tracker.is_in_debt("timeline_42")) {
    // Take corrective action
}
```

### Threshold Callbacks

```cpp
// Add a warning threshold callback
tracker.add_threshold_callback(0.7, 
    [](const std::string& timeline_id, double level) {
        std::cerr << "WARNING: Timeline " << timeline_id 
                 << " approaching debt limit: " << level * 100 << "%" << std::endl;
    }
);

// Add a critical threshold callback
tracker.add_threshold_callback(0.9,
    [](const std::string& timeline_id, double level) {
        std::cerr << "CRITICAL: Timeline " << timeline_id 
                 << " exceeded debt threshold: " << level * 100 << "%" << std::endl;
        // Take immediate action to reduce debt
    }
);
```

## Advanced Features

### Debt Recovery

```cpp
// Custom debt recovery strategy
auto recovery_strategy = [](const std::string& timeline_id, double debt_amount) {
    // Implement custom recovery logic
    return RecoveryResult{
        .success = true,
        .debt_recovered = debt_amount * 0.8,  // Recover 80% of debt
        .message = "Applied custom recovery strategy"
    };
};

// Register recovery strategy
tracker.set_recovery_strategy("custom_recovery", recovery_strategy);

// Trigger recovery
RecoveryResult result = tracker.recover_debt("timeline_42", "custom_recovery");
```

### Anomaly Detection

```cpp
// Configure anomaly detection
AnomalyDetectionConfig anomaly_config{
    .sensitivity = 0.95,      // 95% confidence threshold
    .window_size = 100,       // Number of samples in the sliding window
    .min_anomaly_score = 3.0  // Minimum z-score to consider as anomaly
};

tracker.enable_anomaly_detection(anomaly_config);

// Check for anomalies
auto anomalies = tracker.detect_anomalies();
for (const auto& anomaly : anomalies) {
    std::cout << "Anomaly detected in " << anomaly.timeline_id
              << " with score " << anomaly.score << std::endl;
}
```

## Performance Considerations

- **Time Complexity**: 
  - Record operations: O(1) average, O(log n) worst case
  - Queries: O(1) average, O(n) worst case for full scans
- **Memory Usage**: Grows with the number of unique timelines and debt records
- **Thread Safety**: All public methods are thread-safe
- **Persistence**: Supports saving/loading state to/from disk

## Best Practices

1. **Set Appropriate Thresholds**: Configure warning and critical thresholds based on your application's needs
2. **Monitor Regularly**: Check debt levels at regular intervals
3. **Implement Recovery Strategies**: Define clear recovery procedures for different debt scenarios
4. **Analyze Patterns**: Look for recurring patterns in temporal debt to identify systemic issues
5. **Document Debt**: Always include descriptive information when recording debt events

## Related Documentation

- [Resource Management Guide](../guides/resource_management.md)
- [Resource Optimizer API](./resource_optimizer.md)
- [Timeline Trader API](./timeline_trader.md)
