#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "../include/temporal_debt_alert_system.h"
#include "../include/temporal_debt_tracker.h"

using namespace chronovyan;

// Helper function for printing dividers in output
void printDivider(const std::string& title = "") {
    std::cout << "\n";
    std::cout << "==========================================================\n";
    if (!title.empty()) {
        std::cout << "= " << title << "\n";
        std::cout << "==========================================================\n";
    }
    std::cout << "\n";
}

// Simulate some temporal operations to create debt
void simulateTemporalOperations(std::shared_ptr<TemporalDebtTracker> tracker, int num_operations) {
    printDivider("Simulating Temporal Operations");

    // Cycle through different Rebel operations
    const std::vector<RebelOperationType> operation_types = {
        RebelOperationType::TIMELINE_BRANCH, RebelOperationType::CAUSALITY_INVERSION,
        RebelOperationType::TEMPORAL_ECHO, RebelOperationType::PARADOX_SHIFT};

    // Simulate operations
    for (int i = 0; i < num_operations; ++i) {
        std::string op_id = "OP-" + std::to_string(i);
        RebelOperationType op_type = operation_types[i % operation_types.size()];
        bool is_critical = (i % 3 == 0);  // Every third operation is critical

        // Accrue debt for this operation
        tracker->accrueRebelDebt(op_type, op_id, -1.0, is_critical);

        std::cout << "Simulated operation " << op_id << ": "
                  << tracker->getRebelOperationName(op_type) << (is_critical ? " (CRITICAL)" : "")
                  << "\n";
    }

    // Advance cycle to apply interest
    int due_debts = tracker->advanceCycle();
    std::cout << "Advanced cycle. " << due_debts << " debts are now due.\n";

    // Make one debt past due
    if (num_operations > 0) {
        std::string past_due_op = "OP-0";
        DebtRecord& record = const_cast<DebtRecord&>(tracker->getAllDebts()[0]);
        record.is_past_due = true;
        std::cout << "Made debt for operation " << past_due_op << " past due.\n";
    }
}

// Test basic alert functionality
void testBasicAlerts(std::shared_ptr<TemporalDebtTracker> tracker,
                     std::shared_ptr<TemporalDebtAlertSystem> alert_system) {
    printDivider("Testing Basic Alert Functionality");

    // Register a custom callback for all alert levels
    int alert_count = 0;
    alert_system->registerAlertCallback([&alert_count](const DebtAlert& alert) {
        std::cout << "Custom callback received alert: " << alert.message << " (Level: ";

        switch (alert.level) {
            case DebtAlertLevel::LOW:
                std::cout << "LOW";
                break;
            case DebtAlertLevel::MEDIUM:
                std::cout << "MEDIUM";
                break;
            case DebtAlertLevel::HIGH:
                std::cout << "HIGH";
                break;
            case DebtAlertLevel::CRITICAL:
                std::cout << "CRITICAL";
                break;
            default:
                std::cout << "UNKNOWN";
                break;
        }

        std::cout << ")\n";
        alert_count++;
    });

    // Check alerts before adding debt
    std::cout << "Checking alerts before adding debt:\n";
    bool alerts_triggered = alert_system->checkDebtLevels();
    std::cout << "Alerts triggered: " << (alerts_triggered ? "Yes" : "No") << "\n";

    // Add some debt to trigger alerts
    std::cout << "\nAdding debt to trigger alerts:\n";
    for (int i = 0; i < 5; ++i) {
        std::string op_id = "TEST-" + std::to_string(i);
        double amount = 100.0 * (i + 1);  // Increasing amounts

        tracker->borrowChronons(amount, op_id, (i % 2 == 0));  // Alternate critical flag
        std::cout << "Borrowed " << amount << " chronons for operation " << op_id << "\n";

        // Check alerts after each borrowing
        alerts_triggered = alert_system->checkDebtLevels();
        std::cout << "Alerts triggered: " << (alerts_triggered ? "Yes" : "No") << "\n\n";
    }

    // Verify alert history
    auto history = alert_system->getAllAlertHistory();
    std::cout << "Alert history contains " << history.size() << " entries\n";
    assert(!history.empty() && "Alert history should not be empty");

    // Acknowledge an alert
    if (!history.empty()) {
        bool acknowledged = alert_system->acknowledgeAlert(0);
        std::cout << "Acknowledged first alert: " << (acknowledged ? "Yes" : "No") << "\n";
    }
}

// Test continuous monitoring
void testContinuousMonitoring(std::shared_ptr<TemporalDebtTracker> tracker,
                              std::shared_ptr<TemporalDebtAlertSystem> alert_system) {
    printDivider("Testing Continuous Monitoring");

    // Configure shorter monitoring interval for testing
    AlertConfiguration config = alert_system->getConfiguration();
    config.monitoring_interval = std::chrono::seconds(2);
    alert_system->setConfiguration(config);

    // Start monitoring
    std::cout << "Starting continuous monitoring...\n";
    bool started = alert_system->startContinuousMonitoring();
    assert(started && "Continuous monitoring should start successfully");

    // Simulate some operations while monitoring is active
    std::cout << "Performing operations while monitoring is active:\n";
    for (int i = 0; i < 3; ++i) {
        std::string op_id = "MONITOR-" + std::to_string(i);
        double amount = 150.0 * (i + 1);

        tracker->borrowChronons(amount, op_id, true);  // All critical
        std::cout << "Borrowed " << amount << " chronons for operation " << op_id << "\n";

        // Wait briefly to allow monitoring thread to run
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Stop monitoring
    std::cout << "Stopping continuous monitoring...\n";
    alert_system->stopContinuousMonitoring();
}

// Test visualization and reporting
void testVisualizationAndReporting(std::shared_ptr<TemporalDebtTracker> tracker,
                                   std::shared_ptr<TemporalDebtAlertSystem> alert_system) {
    printDivider("Testing Visualization and Reporting");

    // Generate and display alert report
    std::cout << "Generating alert report:\n";
    std::string report = alert_system->generateAlertReport(true);
    std::cout << report << "\n";

    // Visualize debt trends
    std::cout << "Visualizing debt trends:\n";
    std::string visualization = alert_system->visualizeDebtTrends();
    std::cout << visualization << "\n";

    // Calculate paradox risk
    double risk = alert_system->calculateParadoxRisk();
    std::cout << "Current paradox risk: " << std::fixed << std::setprecision(2) << (risk * 100.0)
              << "%\n";

    // Project future debt
    double projected_debt = alert_system->projectFutureDebt(5);
    std::cout << "Projected debt ratio in 5 cycles: " << std::fixed << std::setprecision(2)
              << projected_debt << "\n";

    // Get debt management recommendations
    std::cout << "Debt management recommendations:\n";
    std::string recommendations = alert_system->getDebtManagementRecommendations();
    std::cout << recommendations << "\n";
}

// Main function
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

    // Run tests
    simulateTemporalOperations(tracker, 6);
    testBasicAlerts(tracker, alert_system);
    testContinuousMonitoring(tracker, alert_system);
    testVisualizationAndReporting(tracker, alert_system);

    printDivider("All Tests Completed Successfully");
    return 0;
}