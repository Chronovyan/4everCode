---
title: Temporal Debt Alert Demo
description: Documentation for temporal debt alert demo
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Temporal Debt Alert System Demo

This document demonstrates how to use the Temporal Debt Alert System through example code snippets and expected output.

## Basic Usage

First, create a debt tracker and an alert system:

```cpp
// Create a debt tracker with limits
auto tracker = std::make_shared<TemporalDebtTracker>(1000.0, 800.0);

// Configure alert system
AlertConfiguration config;
config.low_threshold = 0.3;      // 30% of limit
config.medium_threshold = 0.5;   // 50% of limit
config.high_threshold = 0.7;     // 70% of limit
config.critical_threshold = 0.9; // 90% of limit

// Create alert system
auto alert_system = std::make_shared<TemporalDebtAlertSystem>(tracker, config);

// Register custom callback for critical alerts
alert_system->registerAlertCallback(
    [](const DebtAlert& alert) {
        // Handle critical alert
        std::cout << "Custom handler: " << alert.message << std::endl;
    },
    {DebtAlertLevel::CRITICAL}
);
```

## Borrowing Resources and Triggering Alerts

When you borrow resources, alerts will be triggered based on your configured thresholds:

```cpp
// Borrow chronons for a temporal operation
tracker->borrowChronons(200.0, "operation-1", false);  // Non-critical operation
tracker->borrowChronons(300.0, "operation-2", true);   // Critical operation

// Borrow aethel for a temporal operation
tracker->borrowAethel(150.0, "operation-3", false);
tracker->borrowAethel(250.0, "operation-4", true);

// Check for alerts
alert_system->checkDebtLevels();

// Expected output (if console notification is enabled):
// [DEBT ALERT] MEDIUM: Total debt has reached medium level
// Debt Ratio: 0.50
```

## Continuous Monitoring

The alert system can continuously monitor debt levels in a background thread:

```cpp
// Start continuous monitoring (runs in background thread)
alert_system->startContinuousMonitoring();

// System will now automatically check for alerts at the configured interval
// and trigger notifications as needed

// Later, when done:
alert_system->stopContinuousMonitoring();
```

## Getting Alert History

You can retrieve the history of triggered alerts:

```cpp
// Get all alert history
std::vector<AlertHistory> history = alert_system->getAllAlertHistory();

// Get only critical alerts
std::vector<AlertHistory> critical_alerts = 
    alert_system->getAlertHistory(DebtAlertLevel::CRITICAL);

// Process history
for (const auto& alert_history : history) {
    std::cout << "Alert: " << alert_history.alert.message
              << " (Acknowledged: " << (alert_history.acknowledged ? "Yes" : "No")
              << ")" << std::endl;
}
```

## Acknowledging Alerts

After handling an alert, you can mark it as acknowledged:

```cpp
// Acknowledge an alert by its index in the history
bool success = alert_system->acknowledgeAlert(0);  // Acknowledge the first alert
```

## Generating Reports

The system can generate comprehensive reports about the current debt status:

```cpp
// Generate a comprehensive report
std::string report = alert_system->generateAlertReport(true);
std::cout << report << std::endl;

// Expected output:
// ===== TEMPORAL DEBT ALERT REPORT =====
// Generated: 2023-05-20 15:30:45
// 
// Current Debt Ratio: 0.50
// Temporal Paradox Risk: 25.00%
// Projected Debt (in 10 cycles): 0.65
// 
// Alert Statistics:
//   Total Alerts: 1
//   Critical Alerts: 0
//   High Alerts: 0
// 
// Alert History:
//   [0] 2023-05-20 15:28:30 - MEDIUM: Total debt has reached medium level
// 
// Recommendations:
// - Implement a structured repayment plan to reduce debt.
// - Limit new temporal operations that would increase debt.
// - Review operation efficiency to minimize resource usage.
// ===================================
```

## Visualizing Debt Trends

You can visualize debt trends with an ASCII chart:

```cpp
// Generate ASCII visualization of debt trends
std::string visualization = alert_system->visualizeDebtTrends();
std::cout << visualization << std::endl;

// Expected output:
// Temporal Debt Trend:
// --------------------
// |**          * 1000
// |**          *
// |***         *
// |****        *
// |*****       * 500
// |******      *
// |*******     *
// |********    *
// |*********   *
// |*********** 0
// +------------
// ^ Debt Amount
// Time ->
```

## Projecting Future Debt

The system can project future debt levels based on current trends:

```cpp
// Project debt levels 5 cycles into the future
double projected_debt = alert_system->projectFutureDebt(5);
std::cout << "Projected debt ratio in 5 cycles: " 
          << std::fixed << std::setprecision(2) << projected_debt << std::endl;
```

## Calculating Paradox Risk

You can calculate the risk of a temporal paradox based on current debt levels:

```cpp
// Calculate the risk of temporal paradox
double risk = alert_system->calculateParadoxRisk();
std::cout << "Current paradox risk: " 
          << std::fixed << std::setprecision(2) << (risk * 100.0) << "%" << std::endl;
```

## Getting Debt Management Recommendations

The system provides tailored recommendations for managing debt:

```cpp
// Get recommendations for managing debt
std::string recommendations = alert_system->getDebtManagementRecommendations();
std::cout << "Recommendations:\n" << recommendations << std::endl;
```

## Complete Usage Example

Here's a complete example showing how to use the system:

```cpp
#include "temporal_debt_tracker.h"
#include "temporal_debt_alert_system.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace chronovyan;

int main() {
    // Create debt tracker
    auto tracker = std::make_shared<TemporalDebtTracker>(1000.0, 800.0);
    
    // Configure alert system
    AlertConfiguration config;
    config.low_threshold = 0.3;
    config.medium_threshold = 0.5;
    config.high_threshold = 0.7;
    config.critical_threshold = 0.9;
    
    // Create alert system
    auto alert_system = std::make_shared<TemporalDebtAlertSystem>(tracker, config);
    
    // Register custom callback
    alert_system->registerAlertCallback(
        [](const DebtAlert& alert) {
            std::cout << "Custom handler: " << alert.message << std::endl;
        },
        {DebtAlertLevel::CRITICAL}
    );
    
    // Borrow resources
    std::cout << "Borrowing chronons for operation 1...\n";
    tracker->borrowChronons(300.0, "operation-1", false);
    
    std::cout << "Borrowing aethel for operation 2...\n";
    tracker->borrowAethel(250.0, "operation-2", true);
    
    // Check for alerts
    std::cout << "Checking for alerts...\n";
    alert_system->checkDebtLevels();
    
    // Borrow more resources
    std::cout << "Borrowing more chronons for operation 3...\n";
    tracker->borrowChronons(400.0, "operation-3", true);
    
    std::cout << "Borrowing more aethel for operation 4...\n";
    tracker->borrowAethel(350.0, "operation-4", false);
    
    // Check for alerts again
    std::cout << "Checking for alerts again...\n";
    alert_system->checkDebtLevels();
    
    // Generate a report
    std::cout << "\nGenerating debt alert report:\n";
    std::cout << alert_system->generateAlertReport() << std::endl;
    
    // Visualize debt trends
    std::cout << "\nVisualizing debt trends:\n";
    std::cout << alert_system->visualizeDebtTrends() << std::endl;
    
    // Apply some repayment
    std::cout << "Applying repayment for operation 1...\n";
    tracker->repayChronons(100.0, "operation-1");
    
    // Check for alerts after repayment
    std::cout << "Checking for alerts after repayment...\n";
    alert_system->checkDebtLevels();
    
    // Get recommendations
    std::cout << "\nDebt management recommendations:\n";
    std::cout << alert_system->getDebtManagementRecommendations() << std::endl;
    
    return 0;
}
```

## Expected Console Output

```
Borrowing chronons for operation 1...
Borrowing aethel for operation 2...
Checking for alerts...
[DEBT ALERT] MEDIUM: Total debt has reached medium level
  Debt Ratio: 0.51

Borrowing more chronons for operation 3...
Borrowing more aethel for operation 4...
Checking for alerts again...
[DEBT ALERT] HIGH: Total debt has reached high level
  Debt Ratio: 0.72

Generating debt alert report:
===== TEMPORAL DEBT ALERT REPORT =====
Generated: 2023-05-20 15:45:22

Current Debt Ratio: 0.72
Temporal Paradox Risk: 52.00%
Projected Debt (in 10 cycles): 0.82

Alert Statistics:
  Total Alerts: 2
  Critical Alerts: 0
  High Alerts: 1

Alert History:
  [0] 2023-05-20 15:44:10 - MEDIUM: Total debt has reached medium level
  [1] 2023-05-20 15:44:55 - HIGH: Total debt has reached high level

Recommendations:
- URGENT: Implement immediate debt reduction measures.
- Temporarily halt all non-essential temporal operations.
- Consider accelerated repayment schedules.
- Review all ongoing operations for optimization opportunities.
- WARNING: Debt is projected to increase significantly.
- Preemptive action recommended to prevent future crisis.
===================================

Visualizing debt trends:
Temporal Debt Trend:
--------------------
|*****      * 1300
|*****      *
|******     *
|******     *
|*******    * 650
|*******    *
|********   *
|********   *
|*********  *
|********** 0
+------------
^ Debt Amount
Time ->

Applying repayment for operation 1...
Checking for alerts after repayment...
[DEBT ALERT] MEDIUM: Total debt has reached medium level
  Debt Ratio: 0.64

Debt management recommendations:
- URGENT: Implement immediate debt reduction measures.
- Temporarily halt all non-essential temporal operations.
- Consider accelerated repayment schedules.
- Review all ongoing operations for optimization opportunities.
```

## Best Practices

1. **Configure Appropriate Thresholds**: Set alert thresholds that make sense for your system's stability requirements.

2. **Use Continuous Monitoring**: For critical systems, enable continuous monitoring to catch debt issues early.

3. **Register Custom Callbacks**: Integrate with your system's notification infrastructure by registering custom callbacks.

4. **Acknowledge Alerts**: Always acknowledge alerts once they've been addressed to maintain a clean alert history.

5. **Review Trend Visualizations**: Regularly review debt trend visualizations to spot patterns and address issues before they become critical.

6. **Follow Recommendations**: The system provides tailored recommendations based on your current debt situation; follow them to maintain system stability.

7. **Apply Regular Repayments**: Implement a regular schedule for debt repayment to avoid accumulation of temporal debt.

8. **Monitor Paradox Risk**: Keep the paradox risk below 50% to ensure temporal stability. 