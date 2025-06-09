#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../include/temporal_debt_alert_system.h"

namespace chronovyan {

// Constructor
TemporalDebtAlertSystem::TemporalDebtAlertSystem(
    std::shared_ptr<TemporalDebtTracker> debt_tracker,
    const AlertConfiguration &config)
    : debt_tracker_(debt_tracker), config_(config), monitoring_active_(false) {
  // Register ourselves as an alert handler with the debt tracker
  debt_tracker_->registerAlertCallback(
      [this](const DebtAlert &alert) { this->handleAlert(alert); });
}

// Destructor
TemporalDebtAlertSystem::~TemporalDebtAlertSystem() {
  // Make sure monitoring thread is stopped
  stopContinuousMonitoring();
}

// Start continuous monitoring
bool TemporalDebtAlertSystem::startContinuousMonitoring() {
  if (monitoring_active_) {
    // Already monitoring
    return false;
  }

  // Set flag and start thread
  monitoring_active_ = true;
  monitoring_thread_ =
      std::thread(&TemporalDebtAlertSystem::monitoringThreadFunction, this);

  return true;
}

// Stop continuous monitoring
void TemporalDebtAlertSystem::stopContinuousMonitoring() {
  if (!monitoring_active_) {
    // Not monitoring
    return;
  }

  {
    std::unique_lock<std::mutex> lock(monitoring_mutex_);
    monitoring_active_ = false;
    monitoring_cv_.notify_one();
  }

  // Wait for thread to finish
  if (monitoring_thread_.joinable()) {
    monitoring_thread_.join();
  }
}

// Monitoring thread function
void TemporalDebtAlertSystem::monitoringThreadFunction() {
  while (monitoring_active_) {
    // Check debt levels
    checkDebtLevels();

    // Wait for interval or until stopped
    std::unique_lock<std::mutex> lock(monitoring_mutex_);
    monitoring_cv_.wait_for(lock, config_.monitoring_interval,
                            [this]() { return !monitoring_active_; });
  }
}

// Check debt levels and trigger alerts if necessary
bool TemporalDebtAlertSystem::checkDebtLevels() {
  // Use the debt tracker to check debt levels
  bool alerts_triggered = debt_tracker_->checkAlerts();

  // Check if debt is approaching critical even if no alerts were triggered
  if (!alerts_triggered && isDebtApproachingCritical()) {
    // Create a warning alert
    DebtAlert approaching_alert(
        DebtAlertLevel::MEDIUM,
        "Debt is approaching critical levels based on trend analysis.", "",
        debt_tracker_->calculateDebtRatio(), "TREND");

    // Handle the alert
    handleAlert(approaching_alert);
    alerts_triggered = true;
  }

  return alerts_triggered;
}

// Handle an alert
void TemporalDebtAlertSystem::handleAlert(const DebtAlert &alert) {
  // Check if this is a duplicate alert (if suppression is enabled)
  if (config_.suppress_duplicate_alerts && isDuplicateAlert(alert)) {
    return;
  }

  // Add to history
  alert_history_.emplace_back(alert);

  // Add to recent alerts for deduplication
  if (config_.suppress_duplicate_alerts) {
    std::string key = alert.debt_type + "|" + alert.message;
    recent_alerts_[key] = std::chrono::system_clock::now();
  }

  // Send notifications based on configuration
  auto &methods = config_.notification_methods[alert.level];
  for (auto method : methods) {
    sendNotification(alert, method);
  }

  // Call custom callbacks for this alert level
  if (alert_callbacks_.find(alert.level) != alert_callbacks_.end()) {
    for (auto &callback : alert_callbacks_[alert.level]) {
      callback(alert);
    }
  }
}

// Check if an alert is a duplicate of a recent one
bool TemporalDebtAlertSystem::isDuplicateAlert(const DebtAlert &alert) const {
  // Create a key for this alert
  std::string key = alert.debt_type + "|" + alert.message;

  // Check if we've seen this alert recently
  auto it = recent_alerts_.find(key);
  if (it != recent_alerts_.end()) {
    // Check if it's within the suppression window
    auto now = std::chrono::system_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::minutes>(now - it->second);
    return elapsed < config_.duplicate_suppression_window;
  }

  return false;
}

// Send a notification using the specified method
void TemporalDebtAlertSystem::sendNotification(const DebtAlert &alert,
                                               AlertNotificationType type) {
  switch (type) {
  case AlertNotificationType::CONSOLE:
    // Output to console
    std::cout << "[DEBT ALERT] " << getAlertLevelString(alert.level) << ": "
              << alert.message << std::endl;
    std::cout << "  Debt Ratio: " << std::fixed << std::setprecision(2)
              << alert.debt_ratio << std::endl
              << std::endl;
    break;

  case AlertNotificationType::LOG_FILE:
    // Write to log file
    logAlertToFile(alert);
    break;

  case AlertNotificationType::EMAIL:
    // Email notification would go here
    // This is a placeholder - actual implementation would depend on the email
    // system
    std::cerr << "Email notification not implemented" << std::endl;
    break;

  case AlertNotificationType::UI_POPUP:
    // UI popup would go here
    // This is a placeholder - actual implementation would depend on the UI
    // system
    std::cerr << "UI popup notification not implemented" << std::endl;
    break;

  case AlertNotificationType::SOUND:
    // Sound alert would go here
    // This is a placeholder - actual implementation would depend on the sound
    // system
    std::cerr << "Sound notification not implemented" << std::endl;
    break;

  case AlertNotificationType::CALLBACK:
    // Custom callbacks are handled separately in handleAlert()
    break;
  }
}

// Log an alert to a file
void TemporalDebtAlertSystem::logAlertToFile(const DebtAlert &alert) const {
  // Open log file (append mode)
  std::ofstream logfile("temporal_debt_alerts.log", std::ios::app);
  if (!logfile) {
    std::cerr << "Error: Could not open log file" << std::endl;
    return;
  }

  // Get current timestamp
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);

  // Write log entry
  logfile << "["
          << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S")
          << "] " << getAlertLevelString(alert.level) << ": " << alert.message;

  // Add operation ID if present
  if (!alert.operation_id.empty()) {
    logfile << " (Operation: " << alert.operation_id << ")";
  }

  // Add debt ratio
  logfile << " - Debt Ratio: " << std::fixed << std::setprecision(2)
          << alert.debt_ratio << " - Type: " << alert.debt_type << std::endl;

  logfile.close();
}

// Get a string representation of an alert level
std::string
TemporalDebtAlertSystem::getAlertLevelString(DebtAlertLevel level) const {
  switch (level) {
  case DebtAlertLevel::LOW:
    return "LOW";
  case DebtAlertLevel::MEDIUM:
    return "MEDIUM";
  case DebtAlertLevel::HIGH:
    return "HIGH";
  case DebtAlertLevel::CRITICAL:
    return "CRITICAL";
  default:
    return "UNKNOWN";
  }
}

// Get a formatted timestamp string
std::string TemporalDebtAlertSystem::getTimestampString(
    const std::chrono::system_clock::time_point &timestamp) const {
  std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

// Set configuration
void TemporalDebtAlertSystem::setConfiguration(
    const AlertConfiguration &config) {
  // Stop monitoring if active
  bool was_monitoring = monitoring_active_;
  if (was_monitoring) {
    stopContinuousMonitoring();
  }

  // Update configuration
  config_ = config;

  // Restart monitoring if it was active
  if (was_monitoring) {
    startContinuousMonitoring();
  }
}

// Get configuration
AlertConfiguration TemporalDebtAlertSystem::getConfiguration() const {
  return config_;
}

// Register a custom callback for specific alert levels
void TemporalDebtAlertSystem::registerAlertCallback(
    std::function<void(const DebtAlert &)> callback,
    const std::vector<DebtAlertLevel> &levels) {
  if (levels.empty()) {
    // Register for all levels
    alert_callbacks_[DebtAlertLevel::LOW].push_back(callback);
    alert_callbacks_[DebtAlertLevel::MEDIUM].push_back(callback);
    alert_callbacks_[DebtAlertLevel::HIGH].push_back(callback);
    alert_callbacks_[DebtAlertLevel::CRITICAL].push_back(callback);
  } else {
    // Register for specified levels
    for (auto level : levels) {
      alert_callbacks_[level].push_back(callback);
    }
  }
}

// Get alert history for a specific level
std::vector<AlertHistory>
TemporalDebtAlertSystem::getAlertHistory(DebtAlertLevel level) const {
  std::vector<AlertHistory> filtered_history;

  for (const auto &history : alert_history_) {
    if (history.alert.level == level) {
      filtered_history.push_back(history);
    }
  }

  return filtered_history;
}

// Get all alert history
std::vector<AlertHistory> TemporalDebtAlertSystem::getAllAlertHistory() const {
  return alert_history_;
}

// Acknowledge an alert
bool TemporalDebtAlertSystem::acknowledgeAlert(size_t alert_index) {
  if (alert_index < alert_history_.size()) {
    alert_history_[alert_index].acknowledged = true;
    return true;
  }
  return false;
}

// Generate a report of debt alert status
std::string
TemporalDebtAlertSystem::generateAlertReport(bool include_history) const {
  std::stringstream report;

  // Get current timestamp
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);

  // Report header
  report << "===== TEMPORAL DEBT ALERT REPORT =====" << std::endl;
  report << "Generated: "
         << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S")
         << std::endl
         << std::endl;

  // Current debt status
  double debt_ratio = debt_tracker_->calculateDebtRatio();
  double paradox_risk = calculateParadoxRisk() * 100.0;
  double projected_debt = projectFutureDebt(config_.projection_cycles);

  report << "Current Debt Ratio: " << std::fixed << std::setprecision(2)
         << debt_ratio << std::endl;
  report << "Temporal Paradox Risk: " << std::fixed << std::setprecision(2)
         << paradox_risk << "%" << std::endl;
  report << "Projected Debt (in " << config_.projection_cycles
         << " cycles): " << std::fixed << std::setprecision(2) << projected_debt
         << std::endl
         << std::endl;

  // Alert statistics
  int total_alerts = alert_history_.size();
  int critical_alerts = 0;
  int high_alerts = 0;
  int unacknowledged_alerts = 0;

  for (const auto &history : alert_history_) {
    if (history.alert.level == DebtAlertLevel::CRITICAL) {
      critical_alerts++;
    } else if (history.alert.level == DebtAlertLevel::HIGH) {
      high_alerts++;
    }

    if (!history.acknowledged) {
      unacknowledged_alerts++;
    }
  }

  report << "Alert Statistics:" << std::endl;
  report << "  Total Alerts: " << total_alerts << std::endl;
  report << "  Critical Alerts: " << critical_alerts << std::endl;
  report << "  High Alerts: " << high_alerts << std::endl;
  if (unacknowledged_alerts > 0) {
    report << "  Unacknowledged Alerts: " << unacknowledged_alerts << std::endl;
  }
  report << std::endl;

  // Alert history
  if (include_history && !alert_history_.empty()) {
    report << "Alert History:" << std::endl;
    for (size_t i = 0; i < alert_history_.size(); ++i) {
      const auto &history = alert_history_[i];
      report << "  [" << i << "] " << getTimestampString(history.timestamp)
             << " - " << getAlertLevelString(history.alert.level) << ": "
             << history.alert.message;

      if (!history.acknowledged) {
        report << " (UNACKNOWLEDGED)";
      }

      report << std::endl;
    }
    report << std::endl;
  }

  // Recommendations
  report << "Recommendations:" << std::endl;
  report << getDebtManagementRecommendations() << std::endl;

  report << "===================================" << std::endl;

  return report.str();
}

// Project future debt based on current trends
double TemporalDebtAlertSystem::projectFutureDebt(int cycles) const {
  // Get current debt ratio
  double current_ratio = debt_tracker_->calculateDebtRatio();

  // Get a history of debt records to calculate the trend
  const auto &debt_records = debt_tracker_->getAllDebts();

  // If no debt or only one record, use a simple projection
  if (debt_records.size() <= 1) {
    // Assume 5% growth per cycle as a default
    return std::min(1.0, current_ratio * std::pow(1.05, cycles));
  }

  // Calculate average growth rate from recent history
  double total_growth_rate = 0.0;
  int growth_samples = 0;

  // Use the last 5 cycles or all available data
  size_t history_size = std::min(debt_records.size(), size_t(5));
  for (size_t i = 1; i < history_size; ++i) {
    // Calculate growth rate for each pair of consecutive records
    double prev_amount =
        debt_records[i - 1].chronon_debt + debt_records[i - 1].aethel_debt;
    double curr_amount =
        debt_records[i].chronon_debt + debt_records[i].aethel_debt;

    if (prev_amount > 0) {
      double growth_rate = curr_amount / prev_amount;
      total_growth_rate += growth_rate;
      growth_samples++;
    }
  }

  // Calculate average growth rate
  double avg_growth_rate =
      (growth_samples > 0) ? (total_growth_rate / growth_samples) : 1.05;

  // Project future debt ratio
  double projected_ratio = current_ratio * std::pow(avg_growth_rate, cycles);

  // Cap at 1.0 (100% of limit)
  return std::min(1.0, projected_ratio);
}

// Get debt management recommendations
std::string TemporalDebtAlertSystem::getDebtManagementRecommendations() const {
  std::stringstream recommendations;

  // Get current debt ratio and projected debt
  double current_ratio = debt_tracker_->calculateDebtRatio();
  double projected_ratio = projectFutureDebt(config_.projection_cycles);
  double paradox_risk = calculateParadoxRisk();

  // Generate recommendations based on current and projected debt
  if (current_ratio >= config_.critical_threshold || paradox_risk >= 0.8) {
    // Critical situation
    recommendations
        << "- CRITICAL: Immediate action required to prevent temporal paradox."
        << std::endl;
    recommendations << "- Stop all rebel operations immediately." << std::endl;
    recommendations << "- Implement emergency debt repayment plan."
                    << std::endl;
    recommendations << "- Consider requesting temporal bailout if available."
                    << std::endl;
    recommendations
        << "- System stability at high risk - prepare contingency measures."
        << std::endl;
  } else if (current_ratio >= config_.high_threshold || paradox_risk >= 0.5) {
    // High alert situation
    recommendations << "- URGENT: Implement immediate debt reduction measures."
                    << std::endl;
    recommendations
        << "- Temporarily halt all non-essential temporal operations."
        << std::endl;
    recommendations << "- Consider accelerated repayment schedules."
                    << std::endl;
    recommendations
        << "- Review all ongoing operations for optimization opportunities."
        << std::endl;
  } else if (current_ratio >= config_.medium_threshold || paradox_risk >= 0.3) {
    // Medium alert situation
    recommendations << "- Implement a structured repayment plan to reduce debt."
                    << std::endl;
    recommendations
        << "- Limit new temporal operations that would increase debt."
        << std::endl;
    recommendations
        << "- Review operation efficiency to minimize resource usage."
        << std::endl;
  } else if (current_ratio >= config_.low_threshold || paradox_risk >= 0.1) {
    // Low alert situation
    recommendations << "- Monitor debt levels closely." << std::endl;
    recommendations << "- Consider implementing proactive repayment strategies."
                    << std::endl;
    recommendations << "- Review upcoming operations for potential debt impact."
                    << std::endl;
  } else {
    // Normal situation
    recommendations
        << "- Debt levels are currently within acceptable parameters."
        << std::endl;
    recommendations << "- Continue regular monitoring and maintenance."
                    << std::endl;
  }

  // Add recommendations based on projected debt
  if (projected_ratio > current_ratio + 0.1) {
    // Significant projected increase
    recommendations << "- WARNING: Debt is projected to increase significantly."
                    << std::endl;
    recommendations
        << "- Preemptive action recommended to prevent future crisis."
        << std::endl;
  }

  // Add repayment strategy recommendations
  if (current_ratio >= config_.medium_threshold) {
    // Suggest repayment strategy
    recommendations << "- Recommended repayment strategy: "
                    << debt_tracker_->suggestRepaymentStrategy() << std::endl;
  }

  return recommendations.str();
}

// Generate a visual representation of debt trends
std::string TemporalDebtAlertSystem::visualizeDebtTrends() const {
  std::stringstream visualization;

  // Get debt history
  const auto &debt_records = debt_tracker_->getAllDebts();

  // If no debt history, return a message
  if (debt_records.empty()) {
    return "No debt history available for visualization.";
  }

  // Create an ASCII visualization of debt trends
  visualization << "Temporal Debt Trend:" << std::endl;
  visualization << "--------------------" << std::endl;

  // Constants for visualization
  const int width = 12;
  const int height = 10;

  // Calculate max debt value for scaling
  double max_debt = 0.0;
  std::vector<double> debt_values;

  for (const auto &record : debt_records) {
    double total_debt = record.chronon_debt + record.aethel_debt;
    max_debt = std::max(max_debt, total_debt);
    debt_values.push_back(total_debt);
  }

  // If no data or all zeros, return a message
  if (max_debt == 0.0) {
    return "No significant debt values to visualize.";
  }

  // Scale to fit in the chart (normalize values)
  std::vector<int> scaled_values;
  for (double value : debt_values) {
    int scaled = static_cast<int>((value / max_debt) * (height - 1));
    scaled_values.push_back(scaled);
  }

  // Generate the chart
  for (int y = 0; y < height; ++y) {
    visualization << "|";

    for (int x = 0; x < width && x < static_cast<int>(scaled_values.size());
         ++x) {
      if (height - y - 1 <= scaled_values[x]) {
        visualization << "*";
      } else {
        visualization << " ";
      }
    }

    // Add y-axis labels at certain positions
    if (y == 0) {
      visualization << " " << static_cast<int>(max_debt);
    } else if (y == height / 2) {
      visualization << " " << static_cast<int>(max_debt / 2);
    } else if (y == height - 1) {
      visualization << " 0";
    } else {
      visualization << " ";
    }

    visualization << std::endl;
  }

  // Add x-axis
  visualization << "+";
  for (int x = 0; x < width; ++x) {
    visualization << "-";
  }
  visualization << std::endl;

  // Add labels
  visualization << "^ Debt Amount" << std::endl;
  visualization << "Time ->" << std::endl;

  return visualization.str();
}

// Calculate the risk of temporal paradox
double TemporalDebtAlertSystem::calculateParadoxRisk() const {
  // Base risk on current debt ratio
  double debt_ratio = debt_tracker_->calculateDebtRatio();
  double base_risk =
      debt_ratio *
      debt_ratio; // Quadratic scaling for more sensitivity at higher ratios

  // Add risk based on critical debts
  const auto &debt_records = debt_tracker_->getAllDebts();
  int critical_count = 0;
  int past_due_critical_count = 0;

  for (const auto &record : debt_records) {
    if (record.critical) {
      critical_count++;
      if (record.is_past_due) {
        past_due_critical_count++;
      }
    }
  }

  // Each critical debt adds to risk
  double critical_risk = critical_count * 0.05;

  // Past due critical debts add significant risk
  double past_due_risk = past_due_critical_count * 0.15;

  // Calculate total risk (capped at 1.0)
  double total_risk = base_risk + critical_risk + past_due_risk;
  return std::min(1.0, total_risk);
}

// Check if debt is approaching critical levels
bool TemporalDebtAlertSystem::isDebtApproachingCritical() const {
  // Current debt ratio
  double current_ratio = debt_tracker_->calculateDebtRatio();

  // If already at high or critical level, return true
  if (current_ratio >= config_.high_threshold) {
    return true;
  }

  // Project debt a few cycles into the future
  double projected_ratio = projectFutureDebt(3); // Look 3 cycles ahead

  // If projected to reach critical level soon, return true
  return projected_ratio >= config_.critical_threshold;
}

} // namespace chronovyan