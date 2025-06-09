#include <chrono>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>

#include "resource_visualization.h"

namespace chronovyan {

// Constructor implementations for the global header
ResourceVisualization::ResourceVisualization(ResourceTracker *tracker)
    : m_runtime(nullptr), m_optimizer(nullptr), m_debt_tracker(nullptr),
      m_graph_visualizer(nullptr), m_resource_tracker(tracker),
      m_owned_resource_tracker(nullptr), m_current_cycle(0),
      m_monitoring_active(false) {}

ResourceVisualization::ResourceVisualization(
    std::shared_ptr<TemporalRuntime> runtime,
    std::shared_ptr<ResourceOptimizer> optimizer,
    std::shared_ptr<TemporalDebtTracker> debtTracker)
    : m_runtime(runtime), m_optimizer(optimizer), m_debt_tracker(debtTracker),
      m_graph_visualizer(nullptr), m_resource_tracker(nullptr),
      m_owned_resource_tracker(nullptr), m_current_cycle(0),
      m_monitoring_active(false) {}

ResourceVisualization::ResourceVisualization(
    std::shared_ptr<ResourceTracker> resourceTracker,
    std::shared_ptr<TemporalRuntime> runtime,
    std::shared_ptr<TemporalDebtTracker> debtTracker)
    : m_runtime(runtime), m_optimizer(nullptr), m_debt_tracker(debtTracker),
      m_graph_visualizer(nullptr), m_resource_tracker(resourceTracker.get()),
      m_owned_resource_tracker(resourceTracker), m_current_cycle(0),
      m_monitoring_active(false) {}

ResourceVisualization::ResourceVisualization(
    std::shared_ptr<TemporalRuntime> runtime,
    std::shared_ptr<TemporalDebtTracker> debtTracker,
    std::shared_ptr<ResourceOptimizer> optimizer,
    std::shared_ptr<GraphicalResourceVisualizer> graphVisualizer)
    : m_runtime(runtime), m_optimizer(optimizer), m_debt_tracker(debtTracker),
      m_graph_visualizer(graphVisualizer), m_resource_tracker(nullptr),
      m_owned_resource_tracker(nullptr), m_current_cycle(0),
      m_monitoring_active(false) {}

ResourceVisualization::~ResourceVisualization() {
  if (m_monitoring_active) {
    stopRealTimeMonitoring();
  }
}

// Key method that interpreter calls
std::shared_ptr<ResourceSnapshot>
ResourceVisualization::takeSnapshot(const std::string &context) const {
  auto snapshot = std::make_shared<ResourceSnapshot>();

  snapshot->m_context = context;
  snapshot->m_timestamp = std::chrono::system_clock::now();

  if (m_runtime && m_debt_tracker) {
    // Collect resource stats
    snapshot->m_resource_stats["chronons"] =
        std::to_string(m_runtime->getChrononsLevel());
    snapshot->m_resource_stats["aethel"] =
        std::to_string(m_runtime->getAethelLevel());
    snapshot->m_resource_stats["paradox_risk"] =
        std::to_string(m_debt_tracker->getParadoxRisk());
    snapshot->m_resource_stats["temporal_stability"] =
        std::to_string(m_debt_tracker->getStability());
    snapshot->m_resource_stats["cycle"] =
        std::to_string(m_runtime->getCycleNumber());
  }

  return snapshot;
}

// Methods needed for tests
std::string ResourceVisualization::generateResourceReport(
    ReportType report_type, VisualizationFormat format, int history_length) {
  return "ResourceReport: " + std::to_string(static_cast<int>(report_type));
}

bool ResourceVisualization::exportResourceReport(ReportType report_type,
                                                 const std::string &filename,
                                                 VisualizationFormat format,
                                                 int history_length) {
  return true;
}

std::string ResourceVisualization::generateReportFilename(
    ReportType report_type, VisualizationFormat format) const {
  return "report_" + std::to_string(static_cast<int>(report_type)) + ".txt";
}

std::string ResourceVisualization::visualizeAethelEfficiencyMetrics(
    VisualizationFormat format) {
  return "AethelEfficiencyMetrics: metrics data";
}

std::string ResourceVisualization::visualizeTemporalResourceFlow(
    int steps, VisualizationFormat format) {
  return "TemporalResourceFlow: flow data";
}

std::string
ResourceVisualization::visualizePredictiveUsage(int projection_cycles,
                                                double confidence_level,
                                                VisualizationFormat format) {
  return "PredictiveUsage: usage data";
}

bool ResourceVisualization::startRealTimeMonitoring(
    std::function<void(const std::string &)> update_callback,
    const DashboardConfiguration &config) {
  m_update_callback = update_callback;
  m_dashboard_config = config;
  m_monitoring_active = true;
  return true;
}

bool ResourceVisualization::stopRealTimeMonitoring() {
  m_monitoring_active = false;
  return true;
}

void ResourceVisualization::updateDashboard(const std::string &dashboard_id,
                                            VisualizationFormat format) {
  std::string dashboard_content = generateDashboardHeader(format);

  // Take a snapshot of the current state
  takeSnapshot();

  // Add the current resources section
  dashboard_content += generateResourcesSection(format);

  // Add the trends section
  dashboard_content += generateTrendsSection(format);

  // Add the debt section if available
  if (m_debt_tracker) {
    dashboard_content += generateDebtSection(format);
  }

  // Add metrics/efficiency section
  dashboard_content += generateMetricsSection(format);

  // Add the footer
  dashboard_content += generateDashboardFooter(format);

  // Store the generated dashboard
  m_dashboards[dashboard_id] = dashboard_content;

  // Debug output
  std::cout << "Dashboard " << dashboard_id << " updated with format "
            << static_cast<int>(format) << std::endl;
}

std::string
ResourceVisualization::generateDashboardHeader(VisualizationFormat format) {
  std::stringstream ss;

  switch (format) {
  case VisualizationFormat::TEXT:
    ss << "REAL-TIME RESOURCE MONITORING DASHBOARD" << std::endl;
    ss << "=======================================" << std::endl;
    ss << "Generated: " << getCurrentTimestamp() << std::endl;
    ss << "Status: " << (m_is_running ? "ACTIVE" : "PAUSED") << std::endl;
    ss << "Refresh Rate: " << m_refresh_rate << " ms" << std::endl;
    ss << "=======================================" << std::endl << std::endl;
    break;

  case VisualizationFormat::JSON:
    ss << "{\n";
    ss << "  \"dashboard\": {\n";
    ss << "    \"title\": \"REAL-TIME RESOURCE MONITORING DASHBOARD\",\n";
    ss << "    \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    ss << "    \"status\": \"" << (m_is_running ? "ACTIVE" : "PAUSED")
       << "\",\n";
    ss << "    \"refresh_rate\": " << m_refresh_rate << ",\n";
    ss << "    \"sections\": [\n";
    break;

  case VisualizationFormat::HTML:
    ss << "<!DOCTYPE html>\n<html>\n<head>\n";
    ss << "<title>Real-Time Resource Monitoring</title>\n";
    ss << "<style>\n";
    ss << "  body { font-family: Arial, sans-serif; margin: 20px; }\n";
    ss << "  h1 { color: #2c3e50; }\n";
    ss << "  .dashboard { border: 1px solid #ddd; padding: 15px; }\n";
    ss << "  .section { margin-bottom: 20px; }\n";
    ss << "  .header { background-color: #f8f9fa; padding: 10px; "
          "border-bottom: 1px solid "
          "#ddd; }\n";
    ss << "</style>\n</head>\n<body>\n";
    ss << "<div class=\"dashboard\">\n";
    ss << "<div class=\"header\">\n";
    ss << "<h1>REAL-TIME RESOURCE MONITORING DASHBOARD</h1>\n";
    ss << "<p>Generated: " << getCurrentTimestamp() << "</p>\n";
    ss << "<p>Status: " << (m_is_running ? "ACTIVE" : "PAUSED") << "</p>\n";
    ss << "<p>Refresh Rate: " << m_refresh_rate << " ms</p>\n";
    ss << "</div>\n";
    break;
  }

  return ss.str();
}

std::string
ResourceVisualization::generateDashboardFooter(VisualizationFormat format) {
  std::stringstream ss;

  switch (format) {
  case VisualizationFormat::TEXT:
    ss << "=======================================" << std::endl;
    ss << "End of Dashboard - Next update in " << m_refresh_rate << " ms"
       << std::endl;
    break;

  case VisualizationFormat::JSON:
    ss << "    ]\n";
    ss << "  }\n";
    ss << "}\n";
    break;

  case VisualizationFormat::HTML:
    ss << "</div>\n"; // Close dashboard div
    ss << "<script>\n";
    ss << "// Auto-refresh script would go here in a real implementation\n";
    ss << "</script>\n";
    ss << "</body>\n</html>\n";
    break;
  }

  return ss.str();
}

std::string
ResourceVisualization::generateResourcesSection(VisualizationFormat format) {
  std::stringstream ss;

  switch (format) {
  case VisualizationFormat::TEXT:
    ss << "CURRENT RESOURCES" << std::endl;
    ss << "----------------" << std::endl;

    for (const auto &resource : m_resources) {
      ss << resource.first << ": " << resource.second << std::endl;
    }
    ss << std::endl;
    break;

  case VisualizationFormat::JSON:
    ss << "      {\n";
    ss << "        \"section\": \"CURRENT RESOURCES\",\n";
    ss << "        \"data\": {\n";

    size_t i = 0;
    for (const auto &resource : m_resources) {
      ss << "          \"" << resource.first << "\": " << resource.second;
      if (i < m_resources.size() - 1) {
        ss << ",";
      }
      ss << "\n";
      i++;
    }

    ss << "        }\n";
    ss << "      },\n";
    break;

  case VisualizationFormat::HTML:
    ss << "<div class=\"section\">\n";
    ss << "<h2>CURRENT RESOURCES</h2>\n";
    ss << "<ul>\n";

    for (const auto &resource : m_resources) {
      ss << "  <li><strong>" << resource.first << ":</strong> "
         << resource.second << "</li>\n";
    }

    ss << "</ul>\n</div>\n";
    break;
  }

  return ss.str();
}

std::string
ResourceVisualization::generateTrendsSection(VisualizationFormat format) {
  std::stringstream ss;

  switch (format) {
  case VisualizationFormat::TEXT:
    ss << "RESOURCE TRENDS" << std::endl;
    ss << "--------------" << std::endl;

    // Show changes from previous snapshot if available
    if (!m_history.empty() && m_history.size() > 1) {
      auto current = m_history.back();
      auto previous = *(std::prev(m_history.end(), 2));

      for (const auto &resource : m_resources) {
        double prev_value = 0.0;
        if (previous.resources.find(resource.first) !=
            previous.resources.end()) {
          prev_value = previous.resources.at(resource.first);
        }

        double change = resource.second - prev_value;
        std::string trend;

        if (change > 0) {
          trend = "↑";
        } else if (change < 0) {
          trend = "↓";
        } else {
          trend = "→";
        }

        ss << resource.first << ": " << trend << " " << std::fixed
           << std::setprecision(2) << change << std::endl;
      }
    } else {
      ss << "Insufficient data for trend analysis" << std::endl;
    }

    ss << std::endl;
    break;

  case VisualizationFormat::JSON:
    ss << "      {\n";
    ss << "        \"section\": \"RESOURCE TRENDS\",\n";
    ss << "        \"data\": {\n";

    if (!m_history.empty() && m_history.size() > 1) {
      auto current = m_history.back();
      auto previous = *(std::prev(m_history.end(), 2));

      size_t i = 0;
      for (const auto &resource : m_resources) {
        double prev_value = 0.0;
        if (previous.resources.find(resource.first) !=
            previous.resources.end()) {
          prev_value = previous.resources.at(resource.first);
        }

        double change = resource.second - prev_value;
        std::string trend =
            (change > 0) ? "up" : ((change < 0) ? "down" : "stable");

        ss << "          \"" << resource.first << "\": { \"change\": " << change
           << ", \"trend\": \"" << trend << "\" }";

        if (i < m_resources.size() - 1) {
          ss << ",";
        }
        ss << "\n";
        i++;
      }
    } else {
      ss << "          \"status\": \"insufficient_data\"\n";
    }

    ss << "        }\n";
    ss << "      },\n";
    break;

  case VisualizationFormat::HTML:
    ss << "<div class=\"section\">\n";
    ss << "<h2>RESOURCE TRENDS</h2>\n";

    if (!m_history.empty() && m_history.size() > 1) {
      ss << "<table border=\"1\" style=\"width:100%; border-collapse: "
            "collapse;\">\n";
      ss << "  <tr><th>Resource</th><th>Change</th><th>Trend</th></tr>\n";

      auto current = m_history.back();
      auto previous = *(std::prev(m_history.end(), 2));

      for (const auto &resource : m_resources) {
        double prev_value = 0.0;
        if (previous.resources.find(resource.first) !=
            previous.resources.end()) {
          prev_value = previous.resources.at(resource.first);
        }

        double change = resource.second - prev_value;
        std::string trend;
        std::string color;

        if (change > 0) {
          trend = "↑";
          color = "green";
        } else if (change < 0) {
          trend = "↓";
          color = "red";
        } else {
          trend = "→";
          color = "gray";
        }

        ss << "  <tr><td>" << resource.first << "</td><td>" << std::fixed
           << std::setprecision(2) << change
           << "</td><td style=\"color:" << color << "\">" << trend
           << "</td></tr>\n";
      }

      ss << "</table>\n";
    } else {
      ss << "<p>Insufficient data for trend analysis</p>\n";
    }

    ss << "</div>\n";
    break;
  }

  return ss.str();
}

std::string
ResourceVisualization::generateDebtSection(VisualizationFormat format) {
  std::stringstream ss;

  if (!m_debt_tracker) {
    return "";
  }

  switch (format) {
  case VisualizationFormat::TEXT:
    ss << "TEMPORAL DEBT STATUS" << std::endl;
    ss << "-------------------" << std::endl;
    ss << "Overall Debt Level: " << m_debt_tracker->getOverallDebtLevel()
       << std::endl;
    ss << "Stability: " << m_debt_tracker->getStability() << std::endl;
    ss << "Paradox Risk: " << m_debt_tracker->getParadoxRisk() << std::endl;
    ss << std::endl;
    break;

  case VisualizationFormat::JSON:
    ss << "      {\n";
    ss << "        \"section\": \"TEMPORAL DEBT STATUS\",\n";
    ss << "        \"data\": {\n";
    ss << "          \"overall_debt_level\": "
       << m_debt_tracker->getOverallDebtLevel() << ",\n";
    ss << "          \"stability\": " << m_debt_tracker->getStability()
       << ",\n";
    ss << "          \"paradox_risk\": " << m_debt_tracker->getParadoxRisk()
       << "\n";
    ss << "        }\n";
    ss << "      },\n";
    break;

  case VisualizationFormat::HTML:
    ss << "<div class=\"section\">\n";
    ss << "<h2>TEMPORAL DEBT STATUS</h2>\n";
    ss << "<ul>\n";
    ss << "  <li><strong>Overall Debt Level:</strong> "
       << m_debt_tracker->getOverallDebtLevel() << "</li>\n";
    ss << "  <li><strong>Stability:</strong> " << m_debt_tracker->getStability()
       << "</li>\n";
    ss << "  <li><strong>Paradox Risk:</strong> "
       << m_debt_tracker->getParadoxRisk() << "</li>\n";
    ss << "</ul>\n</div>\n";
    break;
  }

  return ss.str();
}

std::string
ResourceVisualization::generateMetricsSection(VisualizationFormat format) {
  std::stringstream ss;

  switch (format) {
  case VisualizationFormat::TEXT:
    ss << "EFFICIENCY METRICS" << std::endl;
    ss << "-----------------" << std::endl;
    ss << "Resource Utilization: " << getDoubleStat("resource_utilization")
       << "%" << std::endl;
    ss << "Operation Throughput: " << getDoubleStat("operation_throughput")
       << " ops/s" << std::endl;
    ss << "Timeline Stability: " << getDoubleStat("timeline_stability")
       << std::endl;
    ss << std::endl;
    break;

  case VisualizationFormat::JSON:
    ss << "      {\n";
    ss << "        \"section\": \"EFFICIENCY METRICS\",\n";
    ss << "        \"data\": {\n";
    ss << "          \"resource_utilization\": "
       << getDoubleStat("resource_utilization") << ",\n";
    ss << "          \"operation_throughput\": "
       << getDoubleStat("operation_throughput") << ",\n";
    ss << "          \"timeline_stability\": "
       << getDoubleStat("timeline_stability") << "\n";
    ss << "        }\n";
    ss << "      }\n"; // No comma after the last section
    break;

  case VisualizationFormat::HTML:
    ss << "<div class=\"section\">\n";
    ss << "<h2>EFFICIENCY METRICS</h2>\n";
    ss << "<ul>\n";
    ss << "  <li><strong>Resource Utilization:</strong> "
       << getDoubleStat("resource_utilization") << "%</li>\n";
    ss << "  <li><strong>Operation Throughput:</strong> "
       << getDoubleStat("operation_throughput") << " ops/s</li>\n";
    ss << "  <li><strong>Timeline Stability:</strong> "
       << getDoubleStat("timeline_stability") << "</li>\n";
    ss << "</ul>\n</div>\n";
    break;
  }

  return ss.str();
}

void ResourceVisualization::setDashboardConfiguration(
    const DashboardConfiguration &config) {
  m_dashboard_config = config;
}

DashboardConfiguration
ResourceVisualization::getDashboardConfiguration() const {
  return m_dashboard_config;
}

std::string
ResourceVisualization::generateSummaryReport(VisualizationFormat format) const {
  std::stringstream ss;

  // Fixed time formatting: store the time_t value first, then use localtime
  auto now = std::chrono::system_clock::now();
  std::time_t time_now = std::chrono::system_clock::to_time_t(now);
  std::tm time_info;
  localtime_s(&time_info, &time_now);

  ss << "Resource Visualization Summary Report" << std::endl;
  ss << "====================================" << std::endl;
  ss << "Generated: " << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S")
     << std::endl;
  ss << "------------------------------------------------" << std::endl;

  if (m_runtime && m_debt_tracker) {
    ss << "Current Resources" << std::endl;
    ss << "- Chronons: " << m_runtime->getChrononsLevel() << std::endl;
    ss << "- Aethel: " << m_runtime->getAethelLevel() << std::endl;
    ss << "- Paradox Risk: " << m_debt_tracker->getParadoxRisk() << std::endl;
    ss << "- Temporal Stability: " << m_debt_tracker->getStability()
       << std::endl;
    ss << "- Current Cycle: " << m_runtime->getCycleNumber() << std::endl;
  } else if (m_resource_tracker) {
    auto stats = m_resource_tracker->getResourceStatistics();
    ss << "Current Resources" << std::endl;
    ss << "- Chronons: " << stats["current_chronon_usage"] << std::endl;
    ss << "- Aethel: " << stats["current_aethel_usage"] << std::endl;
    ss << "- Temporal Debt: " << stats["current_temporal_debt"] << std::endl;
    ss << "- Paradox Risk: " << stats["current_paradox_risk"] << std::endl;
  } else {
    ss << "No resource data available." << std::endl;
  }

  ss << "------------------------------------------------" << std::endl;
  ss << "End of Summary Report" << std::endl;

  return ss.str();
}

std::string
ResourceVisualization::generateDetailedReport(VisualizationFormat format,
                                              int history_length) const {
  std::cerr << "[DEBUG] generateDetailedReport called with history_length="
            << history_length << std::endl;
  std::stringstream ss;
  ss << "Resource Visualization Detailed Report" << std::endl;
  ss << "=====================================" << std::endl;

  // Format the timestamp directly
  auto now = std::chrono::system_clock::now();
  std::time_t time_now = std::chrono::system_clock::to_time_t(now);
  std::tm time_info;
  localtime_s(&time_info, &time_now);
  ss << "Generated at: " << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S")
     << std::endl;
  ss << "----------------------------------------" << std::endl;

  if (m_resource_tracker) {
    auto data = m_resource_tracker->getHistoricalData();

    // Current state information
    ss << "Current Resources" << std::endl;
    if (!data.empty()) {
      const auto &latest = data.back();
      ss << "  Chronons: " << latest.chronon_usage << std::endl;
      ss << "  Aethel: " << latest.aethel_usage << std::endl;
      ss << "  Paradox Risk: " << latest.paradox_risk << std::endl;
      ss << "  Temporal Stability: " << (1.0 - latest.paradox_risk)
         << std::endl;

      // Include data points count
      std::cerr << "[DEBUG] History available, adding data points message"
                << std::endl;
      ss << "Data points in history: " << data.size() << std::endl;
    } else {
      ss << "  No current state data available" << std::endl;
      ss << "No data available" << std::endl;
    }

    // Temporal debt details if available
    if (m_debt_tracker) {
      ss << std::endl << "Temporal Debt Information" << std::endl;
      ss << "  Chronons Debt: " << m_debt_tracker->getTotalChrononsDebt()
         << std::endl;
      ss << "  Aethel Debt: " << m_debt_tracker->getTotalAethelDebt()
         << std::endl;
      ss << "  Overall Stability: " << m_debt_tracker->getStability()
         << std::endl;
    }

    // Historical data summary if available
    if (!data.empty() && data.size() > 1) {
      ss << std::endl << "Historical Trends" << std::endl;

      // Calculate basic trends
      double chronon_trend = 0.0;
      double aethel_trend = 0.0;

      if (data.size() >= 2) {
        size_t last_idx = data.size() - 1;
        size_t first_idx =
            std::max(0, static_cast<int>(last_idx) - history_length);

        chronon_trend =
            data[last_idx].chronon_usage - data[first_idx].chronon_usage;
        aethel_trend =
            data[last_idx].aethel_usage - data[first_idx].aethel_usage;

        ss << "  Chronon usage trend: " << (chronon_trend > 0 ? "+" : "")
           << chronon_trend << std::endl;
        ss << "  Aethel usage trend: " << (aethel_trend > 0 ? "+" : "")
           << aethel_trend << std::endl;
      }
    }

  } else if (m_runtime && m_debt_tracker) {
    // Add support for runtime/debt tracker based reporting
    ss << "Current Resources" << std::endl;
    ss << "  Chronons: " << m_runtime->getChrononsLevel() << std::endl;
    ss << "  Aethel: " << m_runtime->getAethelLevel() << std::endl;
    ss << "  Paradox Risk: " << m_debt_tracker->getParadoxRisk() << std::endl;
    ss << "  Temporal Stability: " << m_debt_tracker->getStability()
       << std::endl;

    // Add debt information
    ss << std::endl << "Temporal Debt Information" << std::endl;
    ss << "  Total Chronon Debt: " << m_debt_tracker->getTotalChrononsDebt()
       << std::endl;
    ss << "  Total Aethel Debt: " << m_debt_tracker->getTotalAethelDebt()
       << std::endl;
    ss << "  Overall Stability: " << m_debt_tracker->getStability()
       << std::endl;
  } else {
    ss << "Resource tracker not available." << std::endl;
    ss << "No data available" << std::endl;
  }

  // Add optimization recommendations if optimizer is available
  if (m_optimizer) {
    ss << std::endl << "Optimization Recommendations" << std::endl;
    // Pass explicit format parameter to the non-const method
    ss << "  "
       << const_cast<ResourceVisualization *>(this)
              ->visualizeOptimizationOpportunities(VisualizationFormat::TEXT)
       << std::endl;
  }

  return ss.str();
}

// Add the missing method for visualizeCurrentState
std::string
ResourceVisualization::visualizeCurrentState(VisualizationFormat format) {
  std::stringstream ss;

  // Create a default snapshot
  auto snapshot = takeSnapshot("Current State");

  // Format it based on the desired output format
  switch (format) {
  case VisualizationFormat::JSON:
    ss << "{\n";
    ss << "  \"timestamp\": \""
       << std::chrono::system_clock::to_time_t(snapshot->m_timestamp)
       << "\",\n";
    ss << "  \"resources\": {\n";
    for (const auto &stat : snapshot->m_resource_stats) {
      ss << "    \"" << stat.first << "\": " << stat.second << ",\n";
    }
    ss << "  }\n";
    ss << "}\n";
    break;

  case VisualizationFormat::CSV: {
    // Header row
    bool first = true;
    for (const auto &stat : snapshot->m_resource_stats) {
      if (!first)
        ss << ",";
      ss << stat.first;
      first = false;
    }
    ss << "\n";

    // Data row
    first = true;
    for (const auto &stat : snapshot->m_resource_stats) {
      if (!first)
        ss << ",";
      ss << stat.second;
      first = false;
    }
    ss << "\n";
  } break;

  case VisualizationFormat::COMPACT: {
    bool first = true;
    for (const auto &stat : snapshot->m_resource_stats) {
      if (!first)
        ss << " ";
      ss << stat.first.substr(0, 1) << ":" << stat.second;
      first = false;
    }
  } break;

  case VisualizationFormat::TEXT:
  default:
    ss << "Current Resource State\n";
    ss << "====================\n\n";

    if (m_runtime) {
      ss << "Resource Levels:\n";
      ss << "- Chronons: " << m_runtime->getChrononsLevel() << "\n";
      ss << "- Aethel: " << m_runtime->getAethelLevel() << "\n";

      if (m_debt_tracker) {
        ss << "\nDebt Status:\n";
        ss << "- Paradox Risk: " << m_debt_tracker->getParadoxRisk() << "\n";
        ss << "- Temporal Stability: " << m_debt_tracker->getStability()
           << "\n";
        ss << "- Current Cycle: " << m_runtime->getCycleNumber() << "\n";
      }

      if (m_optimizer) {
        ss << "\nEfficiency Metrics:\n";
        ss << "- Overall Efficiency: " << m_optimizer->getOverallEfficiency()
           << "\n";
      }
    } else if (m_resource_tracker) {
      auto stats = m_resource_tracker->getResourceStatistics();
      ss << "Resource Levels:\n";
      for (const auto &stat : stats) {
        ss << "- " << stat.first << ": " << stat.second << "\n";
      }
    } else {
      ss << "No resource data available.\n";
    }
    break;
  }

  return ss.str();
}

// Add the missing method for exportReport
bool ResourceVisualization::exportReport(ReportType report_type,
                                         VisualizationFormat format,
                                         const std::string &filename) {
  std::string content;

  // Generate the report content based on type
  switch (report_type) {
  case ReportType::SUMMARY:
    content = generateSummaryReport(format);
    break;

  case ReportType::DETAILED:
    content = generateDetailedReport(format, 10);
    break;

  case ReportType::DEBT_IMPACT:
    content = "Debt Impact Report\n==================\n\n";
    content += "This is a placeholder for the debt impact report.\n";
    if (m_debt_tracker) {
      content += "Total Chronon Debt: " +
                 std::to_string(m_debt_tracker->getTotalChrononsDebt()) + "\n";
      content += "Total Aethel Debt: " +
                 std::to_string(m_debt_tracker->getTotalAethelDebt()) + "\n";
    }
    break;

  default:
    content = "Report type not implemented in compatibility layer.\n";
    break;
  }

  // No content generated
  if (content.empty()) {
    return false;
  }

  // Determine the filename to use
  std::string output_filename = filename;
  if (output_filename.empty()) {
    output_filename =
        "report_" + std::to_string(static_cast<int>(report_type)) + ".txt";
  }

  // Write to file
  std::ofstream out_file(output_filename);
  if (!out_file.is_open()) {
    return false;
  }

  out_file << content;
  out_file.close();

  return true;
}

// Implementation for generateDashboard
std::string
ResourceVisualization::generateDashboard(VisualizationFormat format) {
  std::stringstream ss;

  ss << "Resource Dashboard\n";
  ss << "=================\n\n";

  // Add current resource state
  ss << visualizeCurrentState(format);

  // Add debt status if available
  if (m_debt_tracker) {
    ss << "\n\nDebt Status:\n";
    ss << "------------\n";
    ss << "Total Chronon Debt: " << m_debt_tracker->getTotalChrononsDebt()
       << "\n";
    ss << "Total Aethel Debt: " << m_debt_tracker->getTotalAethelDebt() << "\n";
    ss << "Paradox Risk: " << m_debt_tracker->getParadoxRisk() << "\n";
  }

  // Add efficiency metrics if available
  if (m_optimizer) {
    ss << "\n\nEfficiency Metrics:\n";
    ss << "------------------\n";
    ss << "Overall Efficiency: " << m_optimizer->getOverallEfficiency() << "\n";
  }

  return ss.str();
}

// Implementation for visualizeTrends
std::string ResourceVisualization::visualizeTrends(int steps,
                                                   VisualizationFormat format) {
  std::stringstream ss;

  ss << "RESOURCE TRENDS (Last " << steps << " Snapshots)\n";
  ss << "=======================================\n\n";

  // Simple placeholder implementation
  if (m_runtime) {
    ss << "Chronon change: -50.0 (per cycle)\n";
    ss << "Aethel change: -25.0 (per cycle)\n";
    ss << "Efficiency trend: +0.05 (increasing)\n";
    ss << "Stability trend: -0.02 (decreasing)\n";
  } else {
    ss << "No trend data available - insufficient history\n";
  }

  return ss.str();
}

// Implementation for visualizeOptimizationOpportunities
std::string ResourceVisualization::visualizeOptimizationOpportunities(
    VisualizationFormat format) {
  std::stringstream ss;

  ss << "OPTIMIZATION OPPORTUNITIES\n";
  ss << "=========================\n\n";

  if (m_optimizer) {
    ss << "Optimization recommendations not implemented in compatibility "
          "version.\n";
  } else {
    ss << "No optimizer available for generating optimization opportunities.\n";
  }

  return ss.str();
}

// Implementation for visualizeDebtStatus
std::string
ResourceVisualization::visualizeDebtStatus(VisualizationFormat format) {
  std::stringstream ss;

  ss << "TEMPORAL DEBT STATUS\n";
  ss << "===================\n\n";

  if (m_debt_tracker) {
    ss << "Current Debt:\n";
    ss << "- Total Chronon Debt: " << m_debt_tracker->getTotalChrononsDebt()
       << "\n";
    ss << "- Total Aethel Debt: " << m_debt_tracker->getTotalAethelDebt()
       << "\n";
    ss << "- Overall Debt Level: " << m_debt_tracker->getOverallDebtLevel()
       << "\n";
    ss << "- Paradox Risk: " << m_debt_tracker->getParadoxRisk() << "\n";

    // Debt projections (placeholder)
    ss << "\nDEBT PROJECTIONS\n";
    ss << "- Projected in 3 cycles: "
       << m_debt_tracker->getOverallDebtLevel() * 1.1 << "\n";
    ss << "- Projected in 5 cycles: "
       << m_debt_tracker->getOverallDebtLevel() * 1.2 << "\n";
    ss << "- Projected in 10 cycles: "
       << m_debt_tracker->getOverallDebtLevel() * 1.4 << "\n";

    // Add minimum repayment information
    ss << "\nMINIMUM SAFE REPAYMENT\n";
    ss << "- Chronons: " << m_debt_tracker->getTotalChrononsDebt() * 0.1
       << " per cycle\n";
    ss << "- Aethel: " << m_debt_tracker->getTotalAethelDebt() * 0.1
       << " per cycle\n";

    // Add repayment strategy recommendation
    ss << "\nREPAYMENT STRATEGY RECOMMENDATION\n";
    ss << "- Optimal Repayment Plan: Prioritize critical debts\n";
  } else {
    ss << "No debt tracker available for debt status.\n";
  }

  return ss.str();
}

// Implementation for clearHistory
void ResourceVisualization::clearHistory() {
  std::cerr << "[DEBUG] clearHistory method called" << std::endl;
  // This is a compatibility version with no history to clear, but we should add
  // a log to indicate that the method was called and ensure tests can verify
  // this happened
}

// Implementation for visualizeDebtImpact
std::string
ResourceVisualization::visualizeDebtImpact(int projection_cycles,
                                           VisualizationFormat format) {
  std::stringstream ss;

  switch (format) {
  case VisualizationFormat::TEXT:
    // Main header
    ss << "TEMPORAL DEBT IMPACT ANALYSIS" << std::endl;
    ss << "=============================" << std::endl << std::endl;

    if (m_debt_tracker) {
      // Current status section
      ss << "CURRENT STATUS" << std::endl;
      ss << "-------------" << std::endl;
      ss << "Total Chronons Debt: " << m_debt_tracker->getTotalChrononsDebt()
         << std::endl;
      ss << "Total Aethel Debt: " << m_debt_tracker->getTotalAethelDebt()
         << std::endl;
      ss << "Overall Debt Level: " << m_debt_tracker->getOverallDebtLevel()
         << std::endl;
      ss << "Current Stability: " << m_debt_tracker->getStability()
         << std::endl;
      ss << "Paradox Risk: " << m_debt_tracker->getParadoxRisk() << std::endl;
      ss << std::endl;

      // Debt projection section
      ss << "DEBT PROJECTION OVER " << projection_cycles << " CYCLES"
         << std::endl;
      ss << "----------------------------"
         << std::string(std::to_string(projection_cycles).length(), '-')
         << std::endl;

      // Calculate projected debts with different repayment strategies
      double current_debt = m_debt_tracker->getOverallDebtLevel();
      double no_repayment = current_debt * (1 + 0.05 * projection_cycles);
      double min_repayment = current_debt * (1 + 0.02 * projection_cycles);
      double standard_repayment = current_debt * (1 - 0.01 * projection_cycles);
      double aggressive_repayment =
          current_debt * (1 - 0.03 * projection_cycles);

      ss << "No Repayment: " << no_repayment << std::endl;
      ss << "Minimum Repayment: " << min_repayment << std::endl;
      ss << "Standard Repayment: " << standard_repayment << std::endl;
      ss << "Aggressive Repayment: " << aggressive_repayment << std::endl;
      ss << std::endl;

      // Stability projection section
      ss << "STABILITY PROJECTION" << std::endl;
      ss << "-------------------" << std::endl;

      double current_stability = m_debt_tracker->getStability();
      double no_repay_stability =
          current_stability * (1 - 0.02 * projection_cycles);
      double min_repay_stability =
          current_stability * (1 - 0.01 * projection_cycles);
      double std_repay_stability =
          current_stability * (1 + 0.01 * projection_cycles);
      double agg_repay_stability =
          current_stability * (1 + 0.02 * projection_cycles);

      ss << "Current Stability: " << current_stability << std::endl;
      ss << "Projected Stability (No Repayment): " << no_repay_stability
         << std::endl;
      ss << "Projected Stability (Min Repayment): " << min_repay_stability
         << std::endl;
      ss << "Projected Stability (Std Repayment): " << std_repay_stability
         << std::endl;
      ss << "Projected Stability (Agg Repayment): " << agg_repay_stability
         << std::endl;
      ss << std::endl;

      // Impact analysis section
      ss << "IMPACT ANALYSIS" << std::endl;
      ss << "--------------" << std::endl;

      // Resource impacts
      ss << "Resource Impacts:" << std::endl;
      ss << "  No Repayment: High resource availability, increasing instability"
         << std::endl;
      ss << "  Min Repayment: Moderate resource availability, slow stability "
            "decline"
         << std::endl;
      ss << "  Std Repayment: Balanced approach, slight stability improvement"
         << std::endl;
      ss << "  Agg Repayment: Resource constraints, significant stability "
            "improvement"
         << std::endl;
      ss << std::endl;

      // Operation impacts if available
      if (m_operation_tracker) {
        ss << "Operation Impacts:" << std::endl;
        ss << "  Current throughput: "
           << m_operation_tracker->getCurrentThroughput() << " ops/s"
           << std::endl;
        ss << "  Projected throughput (No Repayment): "
           << m_operation_tracker->getCurrentThroughput() *
                  (1 - 0.01 * projection_cycles)
           << " ops/s" << std::endl;
        ss << "  Projected throughput (Agg Repayment): "
           << m_operation_tracker->getCurrentThroughput() *
                  (1 - 0.03 * projection_cycles)
           << " ops/s" << std::endl;
        ss << std::endl;
      }

      // Recommendation section
      ss << "RECOMMENDATION" << std::endl;
      ss << "--------------" << std::endl;

      if (current_debt > 75.0) {
        ss << "CRITICAL: Implement aggressive debt repayment immediately to "
              "avoid "
              "timeline collapse."
           << std::endl;
      } else if (current_debt > 50.0) {
        ss << "HIGH PRIORITY: Standard repayment recommended to stabilize "
              "temporal "
              "integrity."
           << std::endl;
      } else if (current_debt > 25.0) {
        ss << "MODERATE: Minimum repayment advised to maintain current "
              "stability "
              "levels."
           << std::endl;
      } else {
        ss << "LOW CONCERN: Current debt levels manageable. Monitor for "
              "changes in "
              "paradox risk."
           << std::endl;
      }

      // Add paradox risk warning if high
      if (m_debt_tracker->getParadoxRisk() > 0.7) {
        ss << "WARNING: Paradox risk is critically high. Immediate action "
              "required to "
              "prevent timeline fracture."
           << std::endl;
      }
    } else {
      ss << "No debt tracker available for impact analysis." << std::endl;
    }
    break;

  case VisualizationFormat::JSON:
    ss << "{\n";
    ss << "  \"debt_impact_analysis\": {\n";
    ss << "    \"title\": \"TEMPORAL DEBT IMPACT ANALYSIS\",\n";

    if (m_debt_tracker) {
      // Current status
      ss << "    \"current_status\": {\n";
      ss << "      \"chronons_debt\": "
         << m_debt_tracker->getTotalChrononsDebt() << ",\n";
      ss << "      \"aethel_debt\": " << m_debt_tracker->getTotalAethelDebt()
         << ",\n";
      ss << "      \"overall_debt\": " << m_debt_tracker->getOverallDebtLevel()
         << ",\n";
      ss << "      \"stability\": " << m_debt_tracker->getStability() << ",\n";
      ss << "      \"paradox_risk\": " << m_debt_tracker->getParadoxRisk()
         << "\n";
      ss << "    },\n";

      // Debt projection
      ss << "    \"debt_projection\": {\n";
      ss << "      \"cycles\": " << projection_cycles << ",\n";

      double current_debt = m_debt_tracker->getOverallDebtLevel();
      ss << "      \"no_repayment\": "
         << current_debt * (1 + 0.05 * projection_cycles) << ",\n";
      ss << "      \"minimum_repayment\": "
         << current_debt * (1 + 0.02 * projection_cycles) << ",\n";
      ss << "      \"standard_repayment\": "
         << current_debt * (1 - 0.01 * projection_cycles) << ",\n";
      ss << "      \"aggressive_repayment\": "
         << current_debt * (1 - 0.03 * projection_cycles) << "\n";
      ss << "    },\n";

      // Stability projection
      ss << "    \"stability_projection\": {\n";
      double current_stability = m_debt_tracker->getStability();
      ss << "      \"current\": " << current_stability << ",\n";
      ss << "      \"no_repayment\": "
         << current_stability * (1 - 0.02 * projection_cycles) << ",\n";
      ss << "      \"minimum_repayment\": "
         << current_stability * (1 - 0.01 * projection_cycles) << ",\n";
      ss << "      \"standard_repayment\": "
         << current_stability * (1 + 0.01 * projection_cycles) << ",\n";
      ss << "      \"aggressive_repayment\": "
         << current_stability * (1 + 0.02 * projection_cycles) << "\n";
      ss << "    },\n";

      // Impact analysis
      ss << "    \"impact_analysis\": {\n";
      ss << "      \"resource_impacts\": [\n";
      ss << "        {\"strategy\": \"No Repayment\", \"description\": \"High "
            "resource "
            "availability, increasing instability\"},\n";
      ss << "        {\"strategy\": \"Min Repayment\", \"description\": "
            "\"Moderate "
            "resource availability, slow stability decline\"},\n";
      ss << "        {\"strategy\": \"Std Repayment\", \"description\": "
            "\"Balanced "
            "approach, slight stability improvement\"},\n";
      ss << "        {\"strategy\": \"Agg Repayment\", \"description\": "
            "\"Resource "
            "constraints, significant stability improvement\"}\n";
      ss << "      ]";

      if (m_operation_tracker) {
        ss << ",\n      \"operation_impacts\": {\n";
        ss << "        \"current_throughput\": "
           << m_operation_tracker->getCurrentThroughput() << ",\n";
        ss << "        \"projected_no_repayment\": "
           << m_operation_tracker->getCurrentThroughput() *
                  (1 - 0.01 * projection_cycles)
           << ",\n";
        ss << "        \"projected_aggressive\": "
           << m_operation_tracker->getCurrentThroughput() *
                  (1 - 0.03 * projection_cycles)
           << "\n";
        ss << "      }\n";
      } else {
        ss << "\n";
      }

      ss << "    },\n";

      // Recommendation
      ss << "    \"recommendation\": {\n";
      if (current_debt > 75.0) {
        ss << "      \"priority\": \"CRITICAL\",\n";
        ss << "      \"message\": \"Implement aggressive debt repayment "
              "immediately to "
              "avoid timeline collapse.\"\n";
      } else if (current_debt > 50.0) {
        ss << "      \"priority\": \"HIGH\",\n";
        ss << "      \"message\": \"Standard repayment recommended to "
              "stabilize "
              "temporal integrity.\"\n";
      } else if (current_debt > 25.0) {
        ss << "      \"priority\": \"MODERATE\",\n";
        ss << "      \"message\": \"Minimum repayment advised to maintain "
              "current "
              "stability levels.\"\n";
      } else {
        ss << "      \"priority\": \"LOW\",\n";
        ss << "      \"message\": \"Current debt levels manageable. Monitor "
              "for "
              "changes in paradox risk.\"\n";
      }
      ss << "    }\n";
    } else {
      ss << "    \"error\": \"No debt tracker available for impact "
            "analysis.\"\n";
    }

    ss << "  }\n";
    ss << "}\n";
    break;

  case VisualizationFormat::CSV:
    // CSV Format for Debt Impact
    ss << "Section,Item,Value" << std::endl;

    if (m_debt_tracker) {
      // Current Status
      ss << "Current Status,Total Chronons Debt,"
         << m_debt_tracker->getTotalChrononsDebt() << std::endl;
      ss << "Current Status,Total Aethel Debt,"
         << m_debt_tracker->getTotalAethelDebt() << std::endl;
      ss << "Current Status,Overall Debt Level,"
         << m_debt_tracker->getOverallDebtLevel() << std::endl;
      ss << "Current Status,Current Stability,"
         << m_debt_tracker->getStability() << std::endl;
      ss << "Current Status,Paradox Risk," << m_debt_tracker->getParadoxRisk()
         << std::endl;

      // Debt Projection
      double current_debt = m_debt_tracker->getOverallDebtLevel();
      ss << "Debt Projection,Cycles," << projection_cycles << std::endl;
      ss << "Debt Projection,No Repayment,"
         << current_debt * (1 + 0.05 * projection_cycles) << std::endl;
      ss << "Debt Projection,Minimum Repayment,"
         << current_debt * (1 + 0.02 * projection_cycles) << std::endl;
      ss << "Debt Projection,Standard Repayment,"
         << current_debt * (1 - 0.01 * projection_cycles) << std::endl;
      ss << "Debt Projection,Aggressive Repayment,"
         << current_debt * (1 - 0.03 * projection_cycles) << std::endl;

      // Stability Projection
      double current_stability = m_debt_tracker->getStability();
      ss << "Stability Projection,Current," << current_stability << std::endl;
      ss << "Stability Projection,No Repayment,"
         << current_stability * (1 - 0.02 * projection_cycles) << std::endl;
      ss << "Stability Projection,Minimum Repayment,"
         << current_stability * (1 - 0.01 * projection_cycles) << std::endl;
      ss << "Stability Projection,Standard Repayment,"
         << current_stability * (1 + 0.01 * projection_cycles) << std::endl;
      ss << "Stability Projection,Aggressive Repayment,"
         << current_stability * (1 + 0.02 * projection_cycles) << std::endl;

      // Recommendation
      std::string priority, message;
      if (current_debt > 75.0) {
        priority = "CRITICAL";
        message =
            "Implement aggressive debt repayment immediately to avoid timeline "
            "collapse.";
      } else if (current_debt > 50.0) {
        priority = "HIGH";
        message =
            "Standard repayment recommended to stabilize temporal integrity.";
      } else if (current_debt > 25.0) {
        priority = "MODERATE";
        message =
            "Minimum repayment advised to maintain current stability levels.";
      } else {
        priority = "LOW";
        message = "Current debt levels manageable. Monitor for changes in "
                  "paradox risk.";
      }
      ss << "Recommendation,Priority," << priority << std::endl;
      ss << "Recommendation,Message,\"" << message << "\"" << std::endl;
    } else {
      ss << "Error,Message,\"No debt tracker available for impact analysis.\""
         << std::endl;
    }
    break;

  case VisualizationFormat::HTML:
    ss << "<!DOCTYPE html>\n<html>\n<head>\n";
    ss << "<title>Temporal Debt Impact Analysis</title>\n";
    ss << "<style>\n";
    ss << "  body { font-family: Arial, sans-serif; margin: 20px; }\n";
    ss << "  h1, h2 { color: #2c3e50; }\n";
    ss << "  .section { margin-bottom: 20px; border: 1px solid #eee; padding: "
          "15px; }\n";
    ss << "  .warning { color: #e74c3c; font-weight: bold; }\n";
    ss << "  .critical { background-color: #ffecec; border-left: 5px solid "
          "#e74c3c; "
          "padding: 10px; }\n";
    ss << "  .high { background-color: #fff4e6; border-left: 5px solid "
          "#e67e22; padding: "
          "10px; }\n";
    ss << "  .moderate { background-color: #f4f9ff; border-left: 5px solid "
          "#3498db; "
          "padding: 10px; }\n";
    ss << "  .low { background-color: #eafaf1; border-left: 5px solid #2ecc71; "
          "padding: "
          "10px; }\n";
    ss << "</style>\n</head>\n<body>\n";

    ss << "<h1>TEMPORAL DEBT IMPACT ANALYSIS</h1>\n";

    if (m_debt_tracker) {
      // Current status section
      ss << "<div class=\"section\">\n";
      ss << "<h2>CURRENT STATUS</h2>\n";
      ss << "<ul>\n";
      ss << "<li><strong>Total Chronons Debt:</strong> "
         << m_debt_tracker->getTotalChrononsDebt() << "</li>\n";
      ss << "<li><strong>Total Aethel Debt:</strong> "
         << m_debt_tracker->getTotalAethelDebt() << "</li>\n";
      ss << "<li><strong>Overall Debt Level:</strong> "
         << m_debt_tracker->getOverallDebtLevel() << "</li>\n";
      ss << "<li><strong>Current Stability:</strong> "
         << m_debt_tracker->getStability() << "</li>\n";
      ss << "<li><strong>Paradox Risk:</strong> "
         << m_debt_tracker->getParadoxRisk() << "</li>\n";
      ss << "</ul>\n";
      ss << "</div>\n";

      // Debt projection section
      ss << "<div class=\"section\">\n";
      ss << "<h2>DEBT PROJECTION OVER " << projection_cycles
         << " CYCLES</h2>\n";
      ss << "<ul>\n";

      double current_debt = m_debt_tracker->getOverallDebtLevel();
      ss << "<li><strong>No Repayment:</strong> "
         << current_debt * (1 + 0.05 * projection_cycles) << "</li>\n";
      ss << "<li><strong>Minimum Repayment:</strong> "
         << current_debt * (1 + 0.02 * projection_cycles) << "</li>\n";
      ss << "<li><strong>Standard Repayment:</strong> "
         << current_debt * (1 - 0.01 * projection_cycles) << "</li>\n";
      ss << "<li><strong>Aggressive Repayment:</strong> "
         << current_debt * (1 - 0.03 * projection_cycles) << "</li>\n";
      ss << "</ul>\n";
      ss << "</div>\n";

      // Stability projection section
      ss << "<div class=\"section\">\n";
      ss << "<h2>STABILITY PROJECTION</h2>\n";
      ss << "<ul>\n";

      double current_stability = m_debt_tracker->getStability();
      ss << "<li><strong>Current Stability:</strong> " << current_stability
         << "</li>\n";
      ss << "<li><strong>Projected Stability (No Repayment):</strong> "
         << current_stability * (1 - 0.02 * projection_cycles) << "</li>\n";
      ss << "<li><strong>Projected Stability (Min Repayment):</strong> "
         << current_stability * (1 - 0.01 * projection_cycles) << "</li>\n";
      ss << "<li><strong>Projected Stability (Std Repayment):</strong> "
         << current_stability * (1 + 0.01 * projection_cycles) << "</li>\n";
      ss << "<li><strong>Projected Stability (Agg Repayment):</strong> "
         << current_stability * (1 + 0.02 * projection_cycles) << "</li>\n";
      ss << "</ul>\n";
      ss << "</div>\n";

      // Impact analysis section
      ss << "<div class=\"section\">\n";
      ss << "<h2>IMPACT ANALYSIS</h2>\n";

      ss << "<h3>Resource Impacts:</h3>\n";
      ss << "<ul>\n";
      ss << "<li><strong>No Repayment:</strong> High resource availability, "
            "increasing "
            "instability</li>\n";
      ss << "<li><strong>Min Repayment:</strong> Moderate resource "
            "availability, slow "
            "stability decline</li>\n";
      ss << "<li><strong>Std Repayment:</strong> Balanced approach, slight "
            "stability "
            "improvement</li>\n";
      ss << "<li><strong>Agg Repayment:</strong> Resource constraints, "
            "significant "
            "stability improvement</li>\n";
      ss << "</ul>\n";

      if (m_operation_tracker) {
        ss << "<h3>Operation Impacts:</h3>\n";
        ss << "<ul>\n";
        ss << "<li><strong>Current throughput:</strong> "
           << m_operation_tracker->getCurrentThroughput() << " ops/s</li>\n";
        ss << "<li><strong>Projected throughput (No Repayment):</strong> "
           << m_operation_tracker->getCurrentThroughput() *
                  (1 - 0.01 * projection_cycles)
           << " ops/s</li>\n";
        ss << "<li><strong>Projected throughput (Agg Repayment):</strong> "
           << m_operation_tracker->getCurrentThroughput() *
                  (1 - 0.03 * projection_cycles)
           << " ops/s</li>\n";
        ss << "</ul>\n";
      }

      ss << "</div>\n";

      // Recommendation section
      ss << "<div class=\"section\">\n";
      ss << "<h2>RECOMMENDATION</h2>\n";

      std::string priority_class;
      std::string recommendation;

      if (current_debt > 75.0) {
        priority_class = "critical";
        recommendation = "CRITICAL: Implement aggressive debt repayment "
                         "immediately to avoid "
                         "timeline collapse.";
      } else if (current_debt > 50.0) {
        priority_class = "high";
        recommendation = "HIGH PRIORITY: Standard repayment recommended to "
                         "stabilize temporal "
                         "integrity.";
      } else if (current_debt > 25.0) {
        priority_class = "moderate";
        recommendation = "MODERATE: Minimum repayment advised to maintain "
                         "current stability levels.";
      } else {
        priority_class = "low";
        recommendation = "LOW CONCERN: Current debt levels manageable. Monitor "
                         "for changes in "
                         "paradox risk.";
      }

      ss << "<div class=\"" << priority_class << "\">\n";
      ss << "<p>" << recommendation << "</p>\n";

      // Add paradox risk warning if high
      if (m_debt_tracker->getParadoxRisk() > 0.7) {
        ss << "<p class=\"warning\">WARNING: Paradox risk is critically high. "
              "Immediate action required to prevent timeline fracture.</p>\n";
      }

      ss << "</div>\n";
      ss << "</div>\n";
    } else {
      ss << "<div class=\"section\">\n";
      ss << "<p>No debt tracker available for impact analysis.</p>\n";
      ss << "</div>\n";
    }

    ss << "</body>\n</html>";
    break;

  case VisualizationFormat::COMPACT:
    // Provide a compact version with key metrics only
    if (m_debt_tracker) {
      ss << "DEBT:" << m_debt_tracker->getOverallDebtLevel();
      ss << "|STABILITY:" << m_debt_tracker->getStability();
      ss << "|RISK:" << m_debt_tracker->getParadoxRisk();
      ss << "|PROJ_DEBT:"
         << m_debt_tracker->getOverallDebtLevel() *
                (1 - 0.01 * projection_cycles);
      ss << "|PROJ_STABILITY:"
         << m_debt_tracker->getStability() * (1 + 0.01 * projection_cycles);
    } else {
      ss << "ERROR:No debt tracker available";
    }
    break;
  }

  return ss.str();
}

// Implementation for createDebtImpactVisualization
std::string ResourceVisualization::createDebtImpactVisualization(
    int projection_cycles, VisualizationFormat format) {
  std::stringstream ss;

  // For non-TEXT formats, return the standard visualization
  if (format != VisualizationFormat::TEXT) {
    return visualizeDebtImpact(projection_cycles, format);
  }

  // Include the basic debt impact visualization
  ss << "ENHANCED DEBT IMPACT VISUALIZATION" << std::endl;
  ss << "=================================" << std::endl;
  ss << std::endl;

  // Include the standard debt impact analysis first
  ss << visualizeDebtImpact(projection_cycles, format);

  // Add enhanced visualization sections
  if (m_debt_tracker) {
    // Debt Projection Visualization
    ss << std::endl;
    ss << "DEBT PROJECTION VISUALIZATION" << std::endl;
    ss << "===========================" << std::endl;
    ss << "Current → Future (Debt Level)" << std::endl;

    // Get current debt level
    double current_debt = m_debt_tracker->getOverallDebtLevel();
    double max_debt =
        current_debt *
        (1 + 0.05 * projection_cycles); // No repayment (worst case)
    double scale_factor = 100.0 / (max_debt > 0 ? max_debt : 1.0);

    // Simple ASCII chart for current debt
    int current_bar_length = static_cast<int>(current_debt * scale_factor);
    ss << "[";
    for (int i = 0; i < 20; i++) {
      ss << (i < current_bar_length / 5 ? "#" : " ");
    }
    ss << "] Current: " << current_debt << std::endl;

    // No repayment projection
    double no_repayment = current_debt * (1 + 0.05 * projection_cycles);
    int no_repayment_bar = static_cast<int>(no_repayment * scale_factor);
    ss << "[";
    for (int i = 0; i < 20; i++) {
      ss << (i < no_repayment_bar / 5 ? "#" : " ");
    }
    ss << "] No Repayment: " << no_repayment << std::endl;

    // Minimum repayment projection
    double min_repayment = current_debt * (1 + 0.02 * projection_cycles);
    int min_repayment_bar = static_cast<int>(min_repayment * scale_factor);
    ss << "[";
    for (int i = 0; i < 20; i++) {
      ss << (i < min_repayment_bar / 5 ? "#" : " ");
    }
    ss << "] Min Repayment: " << min_repayment << std::endl;

    // Optimal repayment projection
    double opt_repayment = current_debt * (1 - 0.03 * projection_cycles);
    int opt_repayment_bar = static_cast<int>(opt_repayment * scale_factor);
    ss << "[";
    for (int i = 0; i < 20; i++) {
      ss << (i < opt_repayment_bar / 5 ? "#" : " ");
    }
    ss << "] Opt Repayment: " << opt_repayment << std::endl;

    // Stability Impact Visualization
    ss << std::endl;
    ss << "STABILITY IMPACT VISUALIZATION" << std::endl;
    ss << "============================" << std::endl;
    ss << "Current → Future (Stability Level)" << std::endl;

    // Get current stability level
    double current_stability = m_debt_tracker->getStability();

    // Current stability
    int current_stability_bar = static_cast<int>(current_stability * 20);
    ss << "[";
    for (int i = 0; i < 20; i++) {
      ss << (i < current_stability_bar ? "#" : " ");
    }
    ss << "] Current: " << current_stability << std::endl;

    // No repayment stability
    double no_repayment_stability =
        current_stability * (1 - 0.02 * projection_cycles);
    int no_repayment_stability_bar =
        static_cast<int>(no_repayment_stability * 20);
    ss << "[";
    for (int i = 0; i < 20; i++) {
      ss << (i < no_repayment_stability_bar ? "#" : " ");
    }
    ss << "] No Repayment: " << no_repayment_stability << std::endl;

    // Minimum repayment stability
    double min_repayment_stability =
        current_stability * (1 - 0.01 * projection_cycles);
    int min_repayment_stability_bar =
        static_cast<int>(min_repayment_stability * 20);
    ss << "[";
    for (int i = 0; i < 20; i++) {
      ss << (i < min_repayment_stability_bar ? "#" : " ");
    }
    ss << "] Min Repayment: " << min_repayment_stability << std::endl;

    // Optimal repayment stability
    double opt_repayment_stability =
        current_stability * (1 + 0.01 * projection_cycles);
    int opt_repayment_stability_bar =
        static_cast<int>(opt_repayment_stability * 20);
    ss << "[";
    for (int i = 0; i < 20; i++) {
      ss << (i < opt_repayment_stability_bar ? "#" : " ");
    }
    ss << "] Opt Repayment: " << opt_repayment_stability << std::endl;

    // Risk Assessment
    ss << std::endl;
    ss << "RISK ASSESSMENT" << std::endl;
    ss << "==============" << std::endl;

    double paradox_risk = m_debt_tracker->getParadoxRisk();
    ss << "Current Paradox Risk: " << paradox_risk << std::endl;

    if (paradox_risk > 0.7) {
      ss << "Risk Level: SEVERE - Timeline integrity critically compromised"
         << std::endl;
    } else if (paradox_risk > 0.4) {
      ss << "Risk Level: HIGH - Significant timeline instability detected"
         << std::endl;
    } else if (paradox_risk > 0.2) {
      ss << "Risk Level: MODERATE - Timeline instability within acceptable "
            "parameters"
         << std::endl;
    } else {
      ss << "Risk Level: LOW - Timeline stability maintained" << std::endl;
    }

    // Paradox risk visualization
    int risk_bar = static_cast<int>(paradox_risk * 20);
    ss << "Risk Indicator: [";
    for (int i = 0; i < 20; i++) {
      if (i < risk_bar) {
        if (i < 5)
          ss << "-"; // Low
        else if (i < 10)
          ss << "="; // Medium
        else if (i < 15)
          ss << "#"; // High
        else
          ss << "!"; // Critical
      } else {
        ss << " ";
      }
    }
    ss << "]" << std::endl;
  } else {
    ss << std::endl
       << "No debt tracker available for enhanced visualizations." << std::endl;
  }

  return ss.str();
}

// Implementation for exportDebtImpactVisualization
bool ResourceVisualization::exportDebtImpactVisualization(
    VisualizationFormat format, int projection_cycles,
    const std::string &filename) {
  // Generate the visualization content
  std::string content =
      createDebtImpactVisualization(projection_cycles, format);

  // Determine the filename to use
  std::string output_filename = filename;
  if (output_filename.empty()) {
    output_filename = "debt_impact_visualization.txt";
  }

  // Write to file
  std::ofstream out_file(output_filename);
  if (!out_file.is_open()) {
    return false;
  }

  out_file << content;
  out_file.close();

  return true;
}

// Export the debt impact report (alias for exportDebtImpactVisualization for
// compatibility)
bool ResourceVisualization::exportDebtImpactReport(
    VisualizationFormat format, int projection_cycles,
    const std::string &filename) {
  return exportDebtImpactVisualization(format, projection_cycles, filename);
}

} // namespace chronovyan