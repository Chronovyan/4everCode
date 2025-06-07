#pragma once
#ifndef CHRONOVYAN_TEMPORAL_DEBT_ALERT_SYSTEM_H
#define CHRONOVYAN_TEMPORAL_DEBT_ALERT_SYSTEM_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "temporal_debt_tracker.h"

namespace chronovyan {

/**
 * @enum AlertNotificationType
 * @brief Defines the type of notification for debt alerts
 */
enum class AlertNotificationType {
    CONSOLE,   // Output to console
    LOG_FILE,  // Write to a log file
    EMAIL,     // Send email notification
    UI_POPUP,  // Display in UI
    SOUND,     // Play alert sound
    CALLBACK   // Trigger custom callback
};

/**
 * @struct AlertConfiguration
 * @brief Configuration for the debt alert system
 */
struct AlertConfiguration {
    // Alert thresholds (ratio of debt to debt limit)
    double low_threshold = 0.3;       // Default for low alerts
    double medium_threshold = 0.5;    // Default for medium alerts
    double high_threshold = 0.7;      // Default for high alerts
    double critical_threshold = 0.9;  // Default for critical alerts

    // Notification settings
    std::map<DebtAlertLevel, std::vector<AlertNotificationType>> notification_methods;

    // Monitoring settings
    bool enable_continuous_monitoring = false;
    std::chrono::seconds monitoring_interval = std::chrono::seconds(60);

    // Alert suppression settings
    bool suppress_duplicate_alerts = true;
    std::chrono::minutes duplicate_suppression_window = std::chrono::minutes(10);

    // Projection settings
    int projection_cycles = 10;  // How many cycles ahead to project debt

    // Constructor with default notification methods
    AlertConfiguration() {
        // By default, all alert levels output to console
        notification_methods[DebtAlertLevel::LOW] = {AlertNotificationType::CONSOLE};
        notification_methods[DebtAlertLevel::MEDIUM] = {AlertNotificationType::CONSOLE};
        notification_methods[DebtAlertLevel::HIGH] = {AlertNotificationType::CONSOLE,
                                                      AlertNotificationType::LOG_FILE};
        notification_methods[DebtAlertLevel::CRITICAL] = {AlertNotificationType::CONSOLE,
                                                          AlertNotificationType::LOG_FILE,
                                                          AlertNotificationType::CALLBACK};
    }
};

/**
 * @struct AlertHistory
 * @brief Records historical alert data
 */
struct AlertHistory {
    DebtAlert alert;
    std::chrono::system_clock::time_point timestamp;
    bool acknowledged;

    AlertHistory(const DebtAlert& a)
        : alert(a), timestamp(std::chrono::system_clock::now()), acknowledged(false) {}
};

/**
 * @class TemporalDebtAlertSystem
 * @brief Enhanced alert system for temporal debt management
 *
 * This class extends the basic alert functionality in TemporalDebtTracker
 * with more sophisticated alert management, visualization, and proactive
 * debt management strategies.
 */
class TemporalDebtAlertSystem {
public:
    /**
     * @brief Construct a new Temporal Debt Alert System
     * @param debt_tracker The debt tracker to monitor
     * @param config Configuration for the alert system
     */
    TemporalDebtAlertSystem(std::shared_ptr<TemporalDebtTracker> debt_tracker,
                            const AlertConfiguration& config = AlertConfiguration());

    /**
     * @brief Destructor that ensures monitoring thread is stopped
     */
    ~TemporalDebtAlertSystem();

    /**
     * @brief Start continuous monitoring of debt levels
     * @return True if monitoring was started successfully
     */
    bool startContinuousMonitoring();

    /**
     * @brief Stop continuous monitoring of debt levels
     */
    void stopContinuousMonitoring();

    /**
     * @brief Check debt levels and trigger alerts if necessary
     * @return True if any alerts were triggered
     */
    bool checkDebtLevels();

    /**
     * @brief Set the alert configuration
     * @param config The new configuration to use
     */
    void setConfiguration(const AlertConfiguration& config);

    /**
     * @brief Get the current alert configuration
     * @return The current configuration
     */
    AlertConfiguration getConfiguration() const;

    /**
     * @brief Register a custom callback for specific alert levels
     * @param callback The function to call when an alert is triggered
     * @param levels The alert levels to register for (empty means all levels)
     */
    void registerAlertCallback(std::function<void(const DebtAlert&)> callback,
                               const std::vector<DebtAlertLevel>& levels = {});

    /**
     * @brief Get alert history for a specific level
     * @param level The alert level to retrieve history for
     * @return Vector of historical alerts for the specified level
     */
    std::vector<AlertHistory> getAlertHistory(DebtAlertLevel level) const;

    /**
     * @brief Get all alert history
     * @return Vector of all historical alerts
     */
    std::vector<AlertHistory> getAllAlertHistory() const;

    /**
     * @brief Mark an alert as acknowledged
     * @param alert_index Index of the alert in the history
     * @return True if the alert was found and acknowledged
     */
    bool acknowledgeAlert(size_t alert_index);

    /**
     * @brief Generate a report of debt alert status
     * @param include_history Whether to include alert history in the report
     * @return String containing the debt alert report
     */
    std::string generateAlertReport(bool include_history = true) const;

    /**
     * @brief Project future debt based on current trends
     * @param cycles Number of cycles to project into the future
     * @return Projected debt level after the specified number of cycles
     */
    double projectFutureDebt(int cycles) const;

    /**
     * @brief Get recommended debt management strategies based on current debt levels
     * @return String containing recommendations for debt management
     */
    std::string getDebtManagementRecommendations() const;

    /**
     * @brief Generate a visual representation of debt trends
     * @return String containing the visualization
     */
    std::string visualizeDebtTrends() const;

    /**
     * @brief Calculate the risk of temporal paradox based on debt levels
     * @return Value between 0.0 (no risk) and 1.0 (extreme risk)
     */
    double calculateParadoxRisk() const;

    /**
     * @brief Check if any debt is approaching critical levels
     * @return True if any debt is approaching critical levels
     */
    bool isDebtApproachingCritical() const;

private:
    std::shared_ptr<TemporalDebtTracker> debt_tracker_;
    AlertConfiguration config_;

    // Alert history storage
    std::vector<AlertHistory> alert_history_;

    // Custom callbacks for specific alert levels
    std::map<DebtAlertLevel, std::vector<std::function<void(const DebtAlert&)>>> alert_callbacks_;

    // Recently triggered alerts (for deduplication)
    std::map<std::string, std::chrono::system_clock::time_point> recent_alerts_;

    // Continuous monitoring thread
    std::atomic<bool> monitoring_active_;
    std::thread monitoring_thread_;
    std::mutex monitoring_mutex_;
    std::condition_variable monitoring_cv_;

    // Internal methods
    void handleAlert(const DebtAlert& alert);
    void monitoringThreadFunction();
    bool isDuplicateAlert(const DebtAlert& alert) const;
    void sendNotification(const DebtAlert& alert, AlertNotificationType type);
    void logAlertToFile(const DebtAlert& alert) const;
    std::string getAlertLevelString(DebtAlertLevel level) const;
    std::string getTimestampString(const std::chrono::system_clock::time_point& timestamp) const;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_TEMPORAL_DEBT_ALERT_SYSTEM_H