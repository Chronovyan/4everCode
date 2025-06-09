#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

#include "resource_optimizer.h"
#include "resource_tracker.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

namespace chronovyan {

// Helper functions to safely access stats from the map
namespace {
std::string getStringStat(const std::map<std::string, std::string> &stats,
                          const std::string &key,
                          const std::string &defaultValue = "N/A") {
  auto it = stats.find(key);
  if (it != stats.end()) {
    return it->second;
  }
  std::cerr << "[Stat Helper] Key '" << key << "' not found in snapshot stats."
            << std::endl;
  return defaultValue;
}

double getDoubleStat(const std::map<std::string, std::string> &stats,
                     const std::string &key, double defaultValue = 0.0) {
  auto it = stats.find(key);
  if (it != stats.end()) {
    try {
      return std::stod(it->second);
    } catch (const std::exception &e) {
      std::cerr << "[Stat Helper] Error converting stat '" << key
                << "' to double: " << e.what() << " (value: '" << it->second
                << "')" << std::endl;
    }
  }
  std::cerr << "[Stat Helper] Key '" << key
            << "' not found or conversion failed for double." << std::endl;
  return defaultValue;
}

long long getLongStat(const std::map<std::string, std::string> &stats,
                      const std::string &key, long long defaultValue = 0) {
  auto it = stats.find(key);
  if (it != stats.end()) {
    try {
      return std::stoll(it->second);
    } catch (const std::exception &e) {
      std::cerr << "[Stat Helper] Error converting stat '" << key
                << "' to long long: " << e.what() << " (value: '" << it->second
                << "')" << std::endl;
    }
  }
  std::cerr << "[Stat Helper] Key '" << key
            << "' not found or conversion failed for long long." << std::endl;
  return defaultValue;
}
} // anonymous namespace

// Static method to capture the current state
// static
ResourceSnapshot ResourceSnapshot::takeSnapshot(
    std::shared_ptr<TemporalRuntime> runtime,
    std::shared_ptr<TemporalDebtTracker> debtTracker,
    const std::string &context, std::shared_ptr<ResourceOptimizer> optimizer,
    std::shared_ptr<GraphicalResourceVisualizer> graphVisualizer) {
  std::cerr << "[Snapshot] Entered ResourceSnapshot::takeSnapshot. Context: "
            << context << std::endl;
  ResourceSnapshot snapshot;
  snapshot.m_context = context;
  snapshot.m_timestamp = std::chrono::system_clock::now();
  std::cerr << "[Snapshot] Initialized snapshot object." << std::endl;

  if (runtime) {
    std::cerr << "[Snapshot] Runtime pointer is valid. Accessing members..."
              << std::endl;
    try {
      std::cerr << "[Snapshot] Attempting runtime->getCycleNumber()..."
                << std::endl;
      long long cycle_num = runtime->getCycleNumber();
      std::cerr << "[Snapshot] runtime->getCycleNumber() successful. Value: "
                << cycle_num << std::endl;
      std::cerr << "[Snapshot] Attempting to assign CURRENT_CYCLE..."
                << std::endl;
      snapshot.m_resource_stats["CURRENT_CYCLE"] = std::to_string(cycle_num);
      std::cerr << "[Snapshot] Assigned CURRENT_CYCLE. Value: "
                << snapshot.m_resource_stats["CURRENT_CYCLE"] << std::endl;

      std::cerr << "[Snapshot] Attempting runtime->getChrononsLevel()..."
                << std::endl;
      double chronons_level = runtime->getChrononsLevel();
      std::cerr << "[Snapshot] runtime->getChrononsLevel() successful. Value: "
                << chronons_level << std::endl;
      std::cerr << "[Snapshot] Attempting to assign CHRONONS_LEVEL..."
                << std::endl;
      snapshot.m_resource_stats["CHRONONS_LEVEL"] =
          std::to_string(chronons_level);
      std::cerr << "[Snapshot] Assigned CHRONONS_LEVEL. Value: "
                << snapshot.m_resource_stats["CHRONONS_LEVEL"] << std::endl;

      std::cerr << "[Snapshot] Attempting runtime->getAethelLevel()..."
                << std::endl;
      double aethel_level = runtime->getAethelLevel();
      std::cerr << "[Snapshot] runtime->getAethelLevel() successful. Value: "
                << aethel_level << std::endl;
      std::cerr << "[Snapshot] Attempting to assign AETHEL_LEVEL..."
                << std::endl;
      snapshot.m_resource_stats["AETHEL_LEVEL"] = std::to_string(aethel_level);
      std::cerr << "[Snapshot] Assigned AETHEL_LEVEL. Value: "
                << snapshot.m_resource_stats["AETHEL_LEVEL"] << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "[Snapshot] EXCEPTION while accessing runtime members: "
                << e.what() << std::endl;
      snapshot.m_resource_stats["CURRENT_CYCLE"] = "ERROR_RUNTIME";
      snapshot.m_resource_stats["CHRONONS_LEVEL"] = "ERROR_RUNTIME";
      snapshot.m_resource_stats["AETHEL_LEVEL"] = "ERROR_RUNTIME";
    }
  } else {
    std::cerr << "[Snapshot] Runtime pointer is NULL." << std::endl;
    snapshot.m_resource_stats["CURRENT_CYCLE"] = "N/A (runtime null)";
    snapshot.m_resource_stats["CHRONONS_LEVEL"] = "N/A (runtime null)";
    snapshot.m_resource_stats["AETHEL_LEVEL"] = "N/A (runtime null)";
  }
  std::cerr << "[Snapshot] Finished processing runtime data." << std::endl;

  if (debtTracker) {
    std::cerr << "[Snapshot] DebtTracker pointer is valid. Accessing members..."
              << std::endl;
    try {
      std::cerr << "[Snapshot] Attempting debtTracker->getOverallDebtLevel()..."
                << std::endl;
      double overall_debt = debtTracker->getOverallDebtLevel();
      std::cerr
          << "[Snapshot] debtTracker->getOverallDebtLevel() successful. Value: "
          << overall_debt << std::endl;
      std::cerr << "[Snapshot] Attempting to assign OVERALL_DEBT_LEVEL..."
                << std::endl;
      snapshot.m_resource_stats["OVERALL_DEBT_LEVEL"] =
          std::to_string(overall_debt);
      std::cerr << "[Snapshot] Assigned OVERALL_DEBT_LEVEL. Value: "
                << snapshot.m_resource_stats["OVERALL_DEBT_LEVEL"] << std::endl;

      std::cerr << "[Snapshot] Attempting debtTracker->getParadoxRisk()..."
                << std::endl;
      double paradox_risk = debtTracker->getParadoxRisk();
      std::cerr
          << "[Snapshot] debtTracker->getParadoxRisk() successful. Value: "
          << paradox_risk << std::endl;
      std::cerr << "[Snapshot] Attempting to assign PARADOX_RISK..."
                << std::endl;
      snapshot.m_resource_stats["PARADOX_RISK"] = std::to_string(paradox_risk);
      std::cerr << "[Snapshot] Assigned PARADOX_RISK. Value: "
                << snapshot.m_resource_stats["PARADOX_RISK"] << std::endl;

      std::cerr << "[Snapshot] Attempting debtTracker->getOperationCounts()..."
                << std::endl;
      auto op_counts = debtTracker->getOperationCounts();
      std::cerr << "[Snapshot] debtTracker->getOperationCounts() successful. "
                   "Map size: "
                << op_counts.size() << std::endl;
      std::cerr << "[Snapshot] Attempting to process operation counts..."
                << std::endl;
      for (const auto &pair : op_counts) {
        std::string key_str =
            "OP_COUNT_" + std::string(getRebelOperationTypeName(pair.first));
        std::string val_str = std::to_string(pair.second);
        std::cerr << "[Snapshot] Processing op_count: " << key_str << " = "
                  << val_str << std::endl;
        snapshot.m_resource_stats[key_str] = val_str;
      }
      std::cerr << "[Snapshot] Processed operation counts." << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "[Snapshot] EXCEPTION while accessing debtTracker members: "
                << e.what() << std::endl;
      snapshot.m_resource_stats["OVERALL_DEBT_LEVEL"] = "ERROR_DEBT";
      snapshot.m_resource_stats["PARADOX_RISK"] = "ERROR_DEBT";
      snapshot.m_resource_stats["OP_COUNTS"] =
          "ERROR_DEBT"; // Generic error for op_counts
    }
  } else {
    std::cerr << "[Snapshot] DebtTracker pointer is NULL." << std::endl;
    snapshot.m_resource_stats["OVERALL_DEBT_LEVEL"] = "N/A (debtTracker null)";
    snapshot.m_resource_stats["PARADOX_RISK"] = "N/A (debtTracker null)";
  }
  std::cerr << "[Snapshot] Finished processing debtTracker data." << std::endl;

  if (optimizer) {
    std::cerr << "[Snapshot] Optimizer pointer is valid. Accessing members..."
              << std::endl;
    try {
      std::cerr << "[Snapshot] Attempting optimizer->getOverallEfficiency()..."
                << std::endl;
      double efficiency = optimizer->getOverallEfficiency();
      std::cerr
          << "[Snapshot] optimizer->getOverallEfficiency() successful. Value: "
          << efficiency << std::endl;
      std::cerr << "[Snapshot] Attempting to assign OVERALL_EFFICIENCY..."
                << std::endl;
      snapshot.m_resource_stats["OVERALL_EFFICIENCY"] =
          std::to_string(efficiency);
      std::cerr << "[Snapshot] Assigned OVERALL_EFFICIENCY. Value: "
                << snapshot.m_resource_stats["OVERALL_EFFICIENCY"] << std::endl;

      std::cerr << "[Snapshot] Attempting "
                   "optimizer->getLastOptimizationSuggestion()..."
                << std::endl;
      std::string suggestion = optimizer->getLastOptimizationSuggestion();
      std::cerr << "[Snapshot] optimizer->getLastOptimizationSuggestion() "
                   "successful. Value: "
                << suggestion << std::endl;
      std::cerr
          << "[Snapshot] Attempting to assign LAST_OPTIMIZATION_SUGGESTION..."
          << std::endl;
      snapshot.m_resource_stats["LAST_OPTIMIZATION_SUGGESTION"] = suggestion;
      std::cerr << "[Snapshot] Assigned LAST_OPTIMIZATION_SUGGESTION. Value: "
                << snapshot.m_resource_stats["LAST_OPTIMIZATION_SUGGESTION"]
                << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "[Snapshot] EXCEPTION while accessing optimizer members: "
                << e.what() << std::endl;
      snapshot.m_resource_stats["OVERALL_EFFICIENCY"] = "ERROR_OPTIMIZER";
      snapshot.m_resource_stats["LAST_OPTIMIZATION_SUGGESTION"] =
          "ERROR_OPTIMIZER";
    }
  } else {
    std::cerr << "[Snapshot] Optimizer pointer is NULL." << std::endl;
    snapshot.m_resource_stats["OVERALL_EFFICIENCY"] = "N/A (optimizer null)";
    snapshot.m_resource_stats["LAST_OPTIMIZATION_SUGGESTION"] =
        "N/A (optimizer null)";
  }
  std::cerr << "[Snapshot] Finished processing optimizer data." << std::endl;

  std::cerr << "[Snapshot] Exiting ResourceSnapshot::takeSnapshot."
            << std::endl;
  return snapshot;
}

// Implementation of ResourceVisualization
ResourceVisualization::ResourceVisualization(
    std::shared_ptr<TemporalRuntime> runtime,
    std::shared_ptr<TemporalDebtTracker> debt_tracker,
    std::shared_ptr<ResourceOptimizer> optimizer,
    std::shared_ptr<GraphicalResourceVisualizer> graph_visualizer)
    : m_runtime(runtime), m_debt_tracker(debt_tracker), m_optimizer(optimizer),
      m_graph_visualizer(graph_visualizer) // Store the new members
{
  if (!m_runtime) {
    throw std::invalid_argument(
        "TemporalRuntime cannot be null for ResourceVisualization");
  }
  if (!m_debt_tracker) {
    throw std::invalid_argument(
        "TemporalDebtTracker cannot be null for ResourceVisualization");
  }
  if (!m_optimizer) {
    // Consider if optimizer can be optional or if it should also throw
    // For now, let's assume it's required as per the constructor arguments
    // This matches the behavior in ResourceExportTest::SetUp
    // throw std::invalid_argument("ResourceOptimizer cannot be null for
    // ResourceVisualization");
    std::cerr << "Warning: ResourceOptimizer is null in ResourceVisualization "
                 "constructor."
              << std::endl;
  }
  if (!m_graph_visualizer) {
    // Similarly, consider if graph_visualizer is optional
    // For now, let's assume it's required.
    // This matches the behavior in ResourceExportTest::SetUp
    // throw std::invalid_argument("GraphicalResourceVisualizer cannot be null
    // for ResourceVisualization");
    std::cerr << "Warning: GraphicalResourceVisualizer is null in "
                 "ResourceVisualization constructor."
              << std::endl;
  }
  std::cerr << "ResourceVisualization constructed with "
            << (m_runtime ? "valid runtime, " : "null runtime, ")
            << (m_debt_tracker ? "valid debt_tracker, " : "null debt_tracker, ")
            << (m_optimizer ? "valid optimizer, " : "null optimizer, ")
            << (m_graph_visualizer ? "valid graph_visualizer."
                                   : "null graph_visualizer.")
            << std::endl;
}

// Implementation of the new constructor
ResourceVisualization::ResourceVisualization(
    std::shared_ptr<TemporalRuntime> runtime,
    std::shared_ptr<ResourceOptimizer> optimizer,
    std::shared_ptr<TemporalDebtTracker> debtTracker)
    : m_runtime(runtime), m_optimizer(optimizer), m_debt_tracker(debtTracker),
      m_resource_tracker(nullptr) {
  // Create a ResourceTracker instance but store it separately to manage
  // lifetime
  auto tracker = std::make_shared<ResourceTracker>();
  m_resource_tracker = tracker.get();

  // Store the shared_ptr to maintain ownership
  m_owned_resource_tracker = tracker;
}

// ADDED: Definition for the constructor taking ResourceTracker*
ResourceVisualization::ResourceVisualization(ResourceTracker *tracker)
    : m_resource_tracker(tracker), // Initialize m_resource_tracker
      m_runtime(nullptr),          // Default initialize other shared_ptrs
      m_debt_tracker(nullptr), m_optimizer(nullptr),
      m_graph_visualizer(nullptr) {
  if (!m_resource_tracker) {
    // Optionally, handle or log if tracker is null, but constructor signature
    // allows it. For now, we'll allow it to proceed. Some visualization methods
    // might behave differently.
    std::cerr << "Warning: ResourceVisualization constructed with a null "
                 "ResourceTracker."
              << std::endl;
  }
  // If m_resource_tracker is expected to be owned, this constructor might need
  // adjustment or the class needs to handle non-owned m_resource_tracker
  // appropriately. For now, this fulfills the constructor signature.
}

// ADDED: Definition for the constructor taking shared_ptr<ResourceTracker>,
// shared_ptr<TemporalRuntime>, shared_ptr<TemporalDebtTracker>
ResourceVisualization::ResourceVisualization(
    std::shared_ptr<ResourceTracker> resourceTracker,
    std::shared_ptr<TemporalRuntime> runtime,
    std::shared_ptr<TemporalDebtTracker> debtTracker)
    : m_resource_tracker(resourceTracker.get()), // Initialize from shared_ptr
      m_runtime(runtime), m_debt_tracker(debtTracker),
      m_optimizer(nullptr),        // Default initialize optimizer
      m_graph_visualizer(nullptr), // Default initialize graph_visualizer
      m_owned_resource_tracker(
          resourceTracker) // Store shared_ptr for ownership
{
  if (!m_resource_tracker) {
    // This indicates the shared_ptr<ResourceTracker> was null
    std::cerr << "Warning: ResourceVisualization constructed with a null "
                 "ResourceTracker shared_ptr."
              << std::endl;
  }
  if (!m_runtime) {
    std::cerr << "Warning: ResourceVisualization constructed with a null "
                 "TemporalRuntime shared_ptr."
              << std::endl;
    // Depending on requirements, might throw std::invalid_argument here
  }
  if (!m_debt_tracker) {
    std::cerr << "Warning: ResourceVisualization constructed with a null "
                 "TemporalDebtTracker "
                 "shared_ptr."
              << std::endl;
    // Depending on requirements, might throw std::invalid_argument here
  }
  // Note: m_optimizer can be set later if needed, or a constructor providing it
  // can be used.
}

// Destructor implementation
ResourceVisualization::~ResourceVisualization() {
  // Nothing special to do here, shared_ptr members will clean up automatically
}

std::string ResourceVisualization::generateTextReport() const {
  std::ostringstream report;
  auto snapshot = takeSnapshot("GenerateTextReport");
  if (!snapshot) {
    report << "Error: Failed to take snapshot." << std::endl;
    return report.str();
  }
  report << "Context: "
         << getStringStat(snapshot->m_resource_stats, "CONTEXT",
                          snapshot->m_context)
         << std::endl;
  report << "Timestamp: "
         << getStringStat(snapshot->m_resource_stats, "TIMESTAMP", "N/A")
         << std::endl;
  report << "Cycle: "
         << getLongStat(snapshot->m_resource_stats, "CURRENT_CYCLE")
         << std::endl;
  report << "Aethel Usage: "
         << getDoubleStat(snapshot->m_resource_stats, "AETHEL_LEVEL")
         << std::endl;
  report << "Chronon Usage: "
         << getDoubleStat(snapshot->m_resource_stats, "CHRONONS_LEVEL")
         << std::endl;
  report << "Overall Efficiency: "
         << getDoubleStat(snapshot->m_resource_stats, "OVERALL_EFFICIENCY")
         << std::endl;
  report << "Overall Debt Level: "
         << getDoubleStat(snapshot->m_resource_stats, "OVERALL_DEBT_LEVEL")
         << std::endl;
  report << "Paradox Risk: "
         << getDoubleStat(snapshot->m_resource_stats, "PARADOX_RISK")
         << std::endl;
  report << "Last Suggestion: "
         << getStringStat(snapshot->m_resource_stats,
                          "LAST_OPTIMIZATION_SUGGESTION")
         << std::endl;

  report << std::endl << "Rebel Operations:" << std::endl;
  const std::string op_prefix = "OP_COUNT_";
  bool found_ops = false;
  for (const auto &pair : snapshot->m_resource_stats) {
    if (pair.first.rfind(op_prefix, 0) == 0) {
      std::string op_name = pair.first.substr(op_prefix.length());
      report << "  " << op_name << ": "
             << getStringStat(snapshot->m_resource_stats, pair.first, "0")
             << std::endl;
      found_ops = true;
    }
  }
  if (!found_ops) {
    report << "  No rebel operations recorded in this snapshot." << std::endl;
  }

  return report.str();
}

bool ResourceVisualization::generateResourceGraph(
    const std::string &outputFile) const {
  // Placeholder implementation - would require a graphing library
  // For now, just return false to indicate the feature is not available
  return false;
}

bool ResourceVisualization::generateTimelineVisualization(
    const std::string &outputFile) const {
  // Placeholder implementation - would require a visualization library
  // For now, just return false to indicate the feature is not available
  return false;
}

bool ResourceVisualization::exportRebelOperationImpactReport(
    RebelOperationType type, VisualizationFormat format, int projection_cycles,
    int operations_per_cycle, const std::string &filename) {
  // Basic implementation - just create a text file
  try {
    std::ofstream file(filename);
    if (!file) {
      return false;
    }

    file << visualizeRebelOperationImpact(type, projection_cycles,
                                          operations_per_cycle, format);
    return true;
  } catch (...) {
    return false;
  }
}

std::string ResourceVisualization::generateReport(ReportType report_type,
                                                  VisualizationFormat format) {
  // Basic implementation
  switch (report_type) {
  case ReportType::SUMMARY:
    return generateTextReport();
  case ReportType::DETAILED:
    return generateDetailedReport(format, 10); // Default to 10 history points
  case ReportType::EFFICIENCY:
    return visualizeOptimizationOpportunities(format);
  case ReportType::DEBT_ANALYSIS:
    return visualizeDebtStatus(format);
  case ReportType::DEBT_IMPACT:
    return visualizeDebtImpact(10, format);
  case ReportType::REBEL_IMPACT:
    return "Rebel impact report not implemented";
  case ReportType::HISTORICAL_TRENDS:
    return visualizeTrends(10, format);
  case ReportType::COMPLETE:
    return generateTextReport() + "\n\n" + visualizeDebtStatus(format);
  default:
    return "Unsupported report type";
  }
}

std::string ResourceVisualization::generateResourceReport(
    ReportType type, VisualizationFormat format, int history_length) {
  // Basic implementation
  return generateReport(type, format);
}

bool ResourceVisualization::exportResourceReport(ReportType type,
                                                 const std::string &filename,
                                                 VisualizationFormat format,
                                                 int history_length) {
  // Basic implementation - just create a text file
  try {
    std::ofstream file(filename);
    if (!file) {
      return false;
    }

    file << generateResourceReport(type, format, history_length);
    return true;
  } catch (...) {
    return false;
  }
}

std::string ResourceVisualization::generateReportFilename(
    ReportType type, VisualizationFormat format) const {
  // Basic implementation
  std::ostringstream filename;

  switch (type) {
  case ReportType::SUMMARY:
    filename << "summary_report";
    break;
  case ReportType::DETAILED:
    filename << "detailed_report";
    break;
  case ReportType::EFFICIENCY:
    filename << "efficiency_report";
    break;
  case ReportType::DEBT_ANALYSIS:
    filename << "debt_analysis_report";
    break;
  case ReportType::DEBT_IMPACT:
    filename << "debt_impact_report";
    break;
  case ReportType::REBEL_IMPACT:
    filename << "rebel_impact_report";
    break;
  case ReportType::HISTORICAL_TRENDS:
    filename << "historical_trends_report";
    break;
  case ReportType::COMPLETE:
    filename << "complete_report";
    break;
  default:
    filename << "unknown_report";
    break;
  }

  switch (format) {
  case VisualizationFormat::TEXT:
    filename << ".txt";
    break;
  case VisualizationFormat::JSON:
    filename << ".json";
    break;
  case VisualizationFormat::HTML:
    filename << ".html";
    break;
  case VisualizationFormat::CSV:
    filename << ".csv";
    break;
  default:
    filename << ".dat";
    break;
  }

  return filename.str();
}

void ResourceVisualization::clearHistory() {
  // No-op implementation for now
}

std::string
ResourceVisualization::generateDetailedReport(VisualizationFormat format,
                                              int history_length) const {
  std::cerr << "[DEBUG] generateDetailedReport called with history_length="
            << history_length << std::endl;
  std::stringstream ss;

  // Generate a header based on the format
  if (format == VisualizationFormat::TEXT) {
    ss << "Resource Visualization Detailed Report" << std::endl;
    ss << "======================================" << std::endl;
    ss << "Generated: " << getCurrentTimestampString() << std::endl;
    ss << "History Length: " << history_length << " entries" << std::endl;
    ss << "--------------------------------------" << std::endl;
  } else if (format == VisualizationFormat::JSON) {
    ss << "{\n  \"report_type\": \"detailed\",\n";
    ss << "  \"generated_at\": \"" << getCurrentTimestampString() << "\",\n";
    ss << "  \"history_length\": " << history_length << ",\n";
    ss << "  \"data\": {\n";
  } else if (format == VisualizationFormat::HTML) {
    ss << "<div class='resource-detailed-report'>\n";
    ss << "<h2>Resource Visualization Detailed Report</h2>\n";
    ss << "<p>Generated: " << getCurrentTimestampString() << "</p>\n";
    ss << "<p>History Length: " << history_length << " entries</p>\n";
    ss << "<div class='report-content'>\n";
  }

  // Take a snapshot of the current state
  auto snapshot = takeSnapshot("Detailed Report");

  // Current Resources section
  if (format == VisualizationFormat::TEXT) {
    ss << "Current Resources" << std::endl;
    ss << "  Chronons: "
       << getDoubleStat(snapshot->m_resource_stats, "CHRONONS_LEVEL", 0.0)
       << std::endl;
    ss << "  Aethel: "
       << getDoubleStat(snapshot->m_resource_stats, "AETHEL_LEVEL", 0.0)
       << std::endl;
    ss << std::endl;

    ss << "Temporal Debt Information" << std::endl;
    ss << "  Total Chronon Debt: "
       << getDoubleStat(snapshot->m_resource_stats, "CHRONON_DEBT", 0.0)
       << std::endl;
    ss << "  Total Aethel Debt: "
       << getDoubleStat(snapshot->m_resource_stats, "AETHEL_DEBT", 0.0)
       << std::endl;
    ss << "  Overall Debt Level: "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_DEBT_LEVEL", 0.0)
       << std::endl;
    ss << "  Paradox Risk: "
       << getDoubleStat(snapshot->m_resource_stats, "PARADOX_RISK", 0.0)
       << std::endl;
    ss << std::endl;

    ss << "Efficiency Metrics" << std::endl;
    ss << "  Overall Efficiency: "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_EFFICIENCY", 1.0)
       << std::endl;
    ss << "  Last Suggestion: "
       << getStringStat(snapshot->m_resource_stats,
                        "LAST_OPTIMIZATION_SUGGESTION",
                        "No suggestions available")
       << std::endl;
  } else if (format == VisualizationFormat::JSON) {
    ss << "    \"current_resources\": {\n";
    ss << "      \"chronons\": "
       << getDoubleStat(snapshot->m_resource_stats, "CHRONONS_LEVEL", 0.0)
       << ",\n";
    ss << "      \"aethel\": "
       << getDoubleStat(snapshot->m_resource_stats, "AETHEL_LEVEL", 0.0)
       << "\n";
    ss << "    },\n";

    ss << "    \"temporal_debt\": {\n";
    ss << "      \"chronon_debt\": "
       << getDoubleStat(snapshot->m_resource_stats, "CHRONON_DEBT", 0.0)
       << ",\n";
    ss << "      \"aethel_debt\": "
       << getDoubleStat(snapshot->m_resource_stats, "AETHEL_DEBT", 0.0)
       << ",\n";
    ss << "      \"overall_debt_level\": "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_DEBT_LEVEL", 0.0)
       << ",\n";
    ss << "      \"paradox_risk\": "
       << getDoubleStat(snapshot->m_resource_stats, "PARADOX_RISK", 0.0)
       << "\n";
    ss << "    },\n";

    ss << "    \"efficiency_metrics\": {\n";
    ss << "      \"overall_efficiency\": "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_EFFICIENCY", 1.0)
       << ",\n";
    ss << "      \"last_suggestion\": \""
       << getStringStat(snapshot->m_resource_stats,
                        "LAST_OPTIMIZATION_SUGGESTION",
                        "No suggestions available")
       << "\"\n";
    ss << "    }";
  } else if (format == VisualizationFormat::HTML) {
    ss << "<h3>Current Resources</h3>\n";
    ss << "<ul>\n";
    ss << "  <li>Chronons: "
       << getDoubleStat(snapshot->m_resource_stats, "CHRONONS_LEVEL", 0.0)
       << "</li>\n";
    ss << "  <li>Aethel: "
       << getDoubleStat(snapshot->m_resource_stats, "AETHEL_LEVEL", 0.0)
       << "</li>\n";
    ss << "</ul>\n";

    ss << "<h3>Temporal Debt Information</h3>\n";
    ss << "<ul>\n";
    ss << "  <li>Total Chronon Debt: "
       << getDoubleStat(snapshot->m_resource_stats, "CHRONON_DEBT", 0.0)
       << "</li>\n";
    ss << "  <li>Total Aethel Debt: "
       << getDoubleStat(snapshot->m_resource_stats, "AETHEL_DEBT", 0.0)
       << "</li>\n";
    ss << "  <li>Overall Debt Level: "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_DEBT_LEVEL", 0.0)
       << "</li>\n";
    ss << "  <li>Paradox Risk: "
       << getDoubleStat(snapshot->m_resource_stats, "PARADOX_RISK", 0.0)
       << "</li>\n";
    ss << "</ul>\n";

    ss << "<h3>Efficiency Metrics</h3>\n";
    ss << "<ul>\n";
    ss << "  <li>Overall Efficiency: "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_EFFICIENCY", 1.0)
       << "</li>\n";
    ss << "</ul>\n";
  }

  // Historical data if available
  std::vector<std::shared_ptr<ResourceSnapshot>> history;
  {
    std::lock_guard<std::mutex> lock(m_history_mutex);
    // Get only the requested number of history entries
    int count = std::min(history_length, static_cast<int>(m_history.size()));
    history.reserve(count);

    // Copy the pointers to avoid holding the lock longer than needed
    for (int i = 0; i < count && i < static_cast<int>(m_history.size()); ++i) {
      history.push_back(m_history[i]);
    }
  }

  if (!history.empty()) {
    if (format == VisualizationFormat::TEXT) {
      ss << std::endl
         << "Historical Data (" << history.size() << " entries):" << std::endl;
      for (size_t i = 0; i < history.size(); ++i) {
        ss << "Entry " << (i + 1) << ":" << std::endl;
        ss << "  Context: " << history[i]->m_context << std::endl;
        ss << "  Chronons: "
           << getDoubleStat(history[i]->m_resource_stats, "CHRONONS_LEVEL", 0.0)
           << std::endl;
        ss << "  Aethel: "
           << getDoubleStat(history[i]->m_resource_stats, "AETHEL_LEVEL", 0.0)
           << std::endl;
        ss << std::endl;
      }
    } else if (format == VisualizationFormat::JSON) {
      ss << ",\n    \"history\": [\n";
      for (size_t i = 0; i < history.size(); ++i) {
        ss << "      {\n";
        ss << "        \"context\": \"" << history[i]->m_context << "\",\n";
        ss << "        \"chronons\": "
           << getDoubleStat(history[i]->m_resource_stats, "CHRONONS_LEVEL", 0.0)
           << ",\n";
        ss << "        \"aethel\": "
           << getDoubleStat(history[i]->m_resource_stats, "AETHEL_LEVEL", 0.0)
           << "\n";
        ss << "      }";
        if (i < history.size() - 1) {
          ss << ",";
        }
        ss << "\n";
      }
      ss << "    ]\n";
    } else if (format == VisualizationFormat::HTML) {
      ss << "<h3>Historical Data (" << history.size() << " entries)</h3>\n";
      ss << "<div class='history-entries'>\n";
      for (size_t i = 0; i < history.size(); ++i) {
        ss << "<div class='history-entry'>\n";
        ss << "<h4>Entry " << (i + 1) << "</h4>\n";
        ss << "<ul>\n";
        ss << "  <li>Context: " << history[i]->m_context << "</li>\n";
        ss << "  <li>Chronons: "
           << getDoubleStat(history[i]->m_resource_stats, "CHRONONS_LEVEL", 0.0)
           << "</li>\n";
        ss << "  <li>Aethel: "
           << getDoubleStat(history[i]->m_resource_stats, "AETHEL_LEVEL", 0.0)
           << "</li>\n";
        ss << "</ul>\n";
        ss << "</div>\n";
      }
      ss << "</div>\n";
    }
  } else {
    if (format == VisualizationFormat::TEXT) {
      ss << std::endl << "No historical data available." << std::endl;
    } else if (format == VisualizationFormat::JSON) {
      ss << ",\n    \"history\": []\n";
    } else if (format == VisualizationFormat::HTML) {
      ss << "<h3>Historical Data</h3>\n";
      ss << "<p>No historical data available.</p>\n";
    }
  }

  // Add footer based on format
  if (format == VisualizationFormat::TEXT) {
    ss << "--------------------------------------" << std::endl;
    ss << "End of Detailed Report" << std::endl;
  } else if (format == VisualizationFormat::JSON) {
    ss << "  }\n}";
  } else if (format == VisualizationFormat::HTML) {
    ss << "</div></div>";
  }

  return ss.str();
}

std::string
ResourceVisualization::visualizeDebtImpact(int projection_cycles,
                                           VisualizationFormat format) {
  std::ostringstream report;
  auto snapshot = takeSnapshot("VisualizeDebtImpact");
  if (!snapshot) {
    report << "Error: Failed to take snapshot for Debt Impact." << std::endl;
    return report.str();
  }

  // Get current debt and stability metrics
  double currentDebt =
      getDoubleStat(snapshot->m_resource_stats, "OVERALL_DEBT_LEVEL", 0.0);
  double currentStability =
      getDoubleStat(snapshot->m_resource_stats, "PARADOX_RISK", 0.0);
  double chrononsDebt =
      getDoubleStat(snapshot->m_resource_stats, "CHRONONS_DEBT", 0.0);
  double aethelDebt =
      getDoubleStat(snapshot->m_resource_stats, "AETHEL_DEBT", 0.0);
  double rebelDebt =
      getDoubleStat(snapshot->m_resource_stats, "REBEL_OPS_DEBT", 0.0);

  // Calculate projections
  double noRepaymentDebt = currentDebt * (1.0 + 0.05 * projection_cycles);
  double minimalRepaymentDebt = currentDebt * (1.0 + 0.02 * projection_cycles);
  double aggressiveRepaymentDebt =
      currentDebt * (1.0 - 0.03 * projection_cycles);

  // Calculate stability projections
  double noRepaymentStability =
      currentStability * (1.0 - 0.02 * projection_cycles);
  double minimalRepaymentStability =
      currentStability * (1.0 - 0.01 * projection_cycles);
  double aggressiveRepaymentStability =
      currentStability * (1.0 + 0.01 * projection_cycles);

  // Calculate resource impacts
  double minimalRepaymentImpact = currentDebt * 0.05;
  double aggressiveRepaymentImpact = currentDebt * 0.15;

  // Determine recommendation based on debt level
  std::string recommendation;
  if (currentDebt > 75.0) {
    recommendation = "CRITICAL: Aggressive debt repayment strongly recommended "
                     "to avoid paradox collapse.";
  } else if (currentDebt > 50.0) {
    recommendation = "WARNING: Debt levels are concerning. Implement regular "
                     "repayment strategy.";
  } else if (currentDebt > 25.0) {
    recommendation =
        "CAUTION: Monitor debt levels closely and begin minimal repayment.";
  } else {
    recommendation = "STABLE: Current debt levels are manageable. Continue "
                     "routine operations.";
  }

  switch (format) {
  case VisualizationFormat::TEXT:
    report << "TEMPORAL DEBT IMPACT ANALYSIS" << std::endl;
    report << "===========================" << std::endl << std::endl;

    report << "CURRENT STATUS" << std::endl;
    report << "-------------" << std::endl;
    report << "Total Debt: " << currentDebt << std::endl;
    report << "Chronons Debt: " << chrononsDebt << std::endl;
    report << "Aethel Debt: " << aethelDebt << std::endl;
    report << "Rebel Operations Debt: " << rebelDebt << std::endl;
    report << "Current Stability Metric: " << currentStability << std::endl
           << std::endl;

    report << "DEBT PROJECTION OVER " << projection_cycles << " CYCLES"
           << std::endl;
    report << "-----------------------------" << std::endl;
    report << "No Repayment Strategy: " << noRepaymentDebt << std::endl;
    report << "Minimal Repayment Strategy: " << minimalRepaymentDebt
           << std::endl;
    report << "Aggressive Repayment Strategy: " << aggressiveRepaymentDebt
           << std::endl
           << std::endl;

    report << "STABILITY PROJECTION" << std::endl;
    report << "-------------------" << std::endl;
    report << "Current Stability: " << currentStability << std::endl;
    report << "Projected Stability (No Repayment): " << noRepaymentStability
           << std::endl;
    report << "Projected Stability (Minimal Repayment): "
           << minimalRepaymentStability << std::endl;
    report << "Projected Stability (Aggressive Repayment): "
           << aggressiveRepaymentStability << std::endl
           << std::endl;

    report << "IMPACT ANALYSIS" << std::endl;
    report << "--------------" << std::endl;
    report << "Resource Impact (Minimal Repayment): " << minimalRepaymentImpact
           << " units/cycle" << std::endl;
    report << "Resource Impact (Aggressive Repayment): "
           << aggressiveRepaymentImpact << " units/cycle" << std::endl;
    report << "Operation Impact: Extended operation times, reduced efficiency"
           << std::endl
           << std::endl;

    report << "RECOMMENDATION" << std::endl;
    report << "-------------" << std::endl;
    report << recommendation << std::endl;
    break;

  case VisualizationFormat::JSON:
    report << "{\n";
    report << "  \"debt_impact_analysis\": {\n";
    report << "    \"current_status\": {\n";
    report << "      \"total_debt\": " << currentDebt << ",\n";
    report << "      \"chronons_debt\": " << chrononsDebt << ",\n";
    report << "      \"aethel_debt\": " << aethelDebt << ",\n";
    report << "      \"rebel_ops_debt\": " << rebelDebt << ",\n";
    report << "      \"stability\": " << currentStability << "\n";
    report << "    },\n";
    report << "    \"debt_projection\": {\n";
    report << "      \"cycles\": " << projection_cycles << ",\n";
    report << "      \"no_repayment\": " << noRepaymentDebt << ",\n";
    report << "      \"minimal_repayment\": " << minimalRepaymentDebt << ",\n";
    report << "      \"aggressive_repayment\": " << aggressiveRepaymentDebt
           << "\n";
    report << "    },\n";
    report << "    \"stability_projection\": {\n";
    report << "      \"current\": " << currentStability << ",\n";
    report << "      \"no_repayment\": " << noRepaymentStability << ",\n";
    report << "      \"minimal_repayment\": " << minimalRepaymentStability
           << ",\n";
    report << "      \"aggressive_repayment\": " << aggressiveRepaymentStability
           << "\n";
    report << "    },\n";
    report << "    \"impact_analysis\": {\n";
    report << "      \"minimal_repayment_impact\": " << minimalRepaymentImpact
           << ",\n";
    report << "      \"aggressive_repayment_impact\": "
           << aggressiveRepaymentImpact << ",\n";
    report << "      \"operation_impact\": \"Extended operation times, reduced "
              "efficiency\"\n";
    report << "    },\n";
    report << "    \"recommendation\": \"" << recommendation << "\"\n";
    report << "  }\n";
    report << "}";
    break;

  case VisualizationFormat::CSV:
    report << "Section,Metric,Value\n";
    report << "Current Status,Total Debt," << currentDebt << "\n";
    report << "Current Status,Chronons Debt," << chrononsDebt << "\n";
    report << "Current Status,Aethel Debt," << aethelDebt << "\n";
    report << "Current Status,Rebel Operations Debt," << rebelDebt << "\n";
    report << "Current Status,Stability," << currentStability << "\n";
    report << "Projection,Cycles," << projection_cycles << "\n";
    report << "Projection,No_Repayment_Debt," << noRepaymentDebt << "\n";
    report << "Projection,Minimal_Repayment_Debt," << minimalRepaymentDebt
           << "\n";
    report << "Projection,Aggressive_Repayment_Debt," << aggressiveRepaymentDebt
           << "\n";
    report << "Stability,Current," << currentStability << "\n";
    report << "Stability,No_Repayment," << noRepaymentStability << "\n";
    report << "Stability,Minimal_Repayment," << minimalRepaymentStability
           << "\n";
    report << "Stability,Aggressive_Repayment," << aggressiveRepaymentStability
           << "\n";
    report << "Impact,Minimal_Repayment," << minimalRepaymentImpact << "\n";
    report << "Impact,Aggressive_Repayment," << aggressiveRepaymentImpact
           << "\n";
    report << "Recommendation,Advice,\"" << recommendation << "\"\n";
    break;

  case VisualizationFormat::HTML:
    report << "<div class='debt-impact-analysis'>\n";
    report << "  <h1>TEMPORAL DEBT IMPACT ANALYSIS</h1>\n";

    report << "  <div class='section'>\n";
    report << "    <h2>CURRENT STATUS</h2>\n";
    report << "    <p>Total Debt: " << currentDebt << "</p>\n";
    report << "    <p>Chronons Debt: " << chrononsDebt << "</p>\n";
    report << "    <p>Aethel Debt: " << aethelDebt << "</p>\n";
    report << "    <p>Rebel Operations Debt: " << rebelDebt << "</p>\n";
    report << "    <p>Current Stability Metric: " << currentStability
           << "</p>\n";
    report << "  </div>\n";

    report << "  <div class='section'>\n";
    report << "    <h2>DEBT PROJECTION OVER " << projection_cycles
           << " CYCLES</h2>\n";
    report << "    <p>No Repayment Strategy: " << noRepaymentDebt << "</p>\n";
    report << "    <p>Minimal Repayment Strategy: " << minimalRepaymentDebt
           << "</p>\n";
    report << "    <p>Aggressive Repayment Strategy: "
           << aggressiveRepaymentDebt << "</p>\n";
    report << "  </div>\n";

    report << "  <div class='section'>\n";
    report << "    <h2>STABILITY PROJECTION</h2>\n";
    report << "    <p>Current Stability: " << currentStability << "</p>\n";
    report << "    <p>Projected Stability (No Repayment): "
           << noRepaymentStability << "</p>\n";
    report << "    <p>Projected Stability (Minimal Repayment): "
           << minimalRepaymentStability << "</p>\n";
    report << "    <p>Projected Stability (Aggressive Repayment): "
           << aggressiveRepaymentStability << "</p>\n";
    report << "  </div>\n";

    report << "  <div class='section'>\n";
    report << "    <h2>IMPACT ANALYSIS</h2>\n";
    report << "    <p>Resource Impact (Minimal Repayment): "
           << minimalRepaymentImpact << " units/cycle</p>\n";
    report << "    <p>Resource Impact (Aggressive Repayment): "
           << aggressiveRepaymentImpact << " units/cycle</p>\n";
    report << "    <p>Operation Impact: Extended operation times, reduced "
              "efficiency</p>\n";
    report << "  </div>\n";

    report << "  <div class='section'>\n";
    report << "    <h2>RECOMMENDATION</h2>\n";
    report << "    <p>" << recommendation << "</p>\n";
    report << "  </div>\n";
    report << "</div>";
    break;

  case VisualizationFormat::COMPACT:
    report << "DEBT_IMPACT|"
           << "TOTAL:" << currentDebt << "|"
           << "STABILITY:" << currentStability << "|"
           << "PROJ_DEBT:" << noRepaymentDebt << "|"
           << "PROJ_STABILITY:" << noRepaymentStability << "|"
           << "RECOMMENDATION:" << recommendation;
    break;
  }

  return report.str();
}

// Implement setThemeType
void ResourceVisualization::setThemeType(ThemeType type) {
  setVisualizationTheme(VisualizationTheme::createTheme(type));
}

// Implement setVisualizationTheme
void ResourceVisualization::setVisualizationTheme(
    const VisualizationTheme &theme) {
  // Simple implementation - in a real scenario, we would store the theme
}

// Implement generateASCIIChart
std::string ResourceVisualization::generateASCIIChart(
    const std::vector<double> &data, int width, int height,
    const std::string &title, const std::string &x_label,
    const std::string &y_label) const {
  // Simple ASCII chart implementation
  std::ostringstream chart;

  // Add title
  chart << title << std::endl;

  // Determine min and max values
  double max_value =
      data.empty() ? 1.0 : *std::max_element(data.begin(), data.end());
  double min_value =
      data.empty() ? 0.0 : *std::min_element(data.begin(), data.end());
  if (max_value == min_value)
    max_value = min_value + 1.0; // Avoid division by zero

  // Generate chart rows from top to bottom
  for (int row = 0; row < height; ++row) {
    // Y-axis label for this row
    double row_value =
        max_value - (row * (max_value - min_value) / (height - 1));
    chart << std::setw(8) << std::fixed << std::setprecision(2) << row_value
          << " |";

    // Data points for this row
    for (size_t col = 0; col < data.size() && col < static_cast<size_t>(width);
         ++col) {
      double normalized_value =
          (data[col] - min_value) / (max_value - min_value);
      double threshold = 1.0 - (static_cast<double>(row) / (height - 1));

      chart << (normalized_value >= threshold ? '*' : ' ');
    }
    chart << std::endl;
  }

  // X-axis
  chart << "        +";
  for (int i = 0; i < width; ++i) {
    chart << "-";
  }
  chart << std::endl;

  // X-axis label
  chart << "         " << x_label << std::endl;

  return chart.str();
}

// Implement generateASCIIGauge
std::string
ResourceVisualization::generateASCIIGauge(double value, int width,
                                          const std::string &title) const {
  // Simple ASCII gauge implementation
  std::ostringstream gauge;

  // Ensure value is between 0 and 1
  value = std::max(0.0, std::min(1.0, value));

  // Calculate the number of filled positions
  int filled = static_cast<int>(value * width);

  // Add title
  gauge << title << ": ";

  // Generate the gauge
  gauge << "[";
  for (int i = 0; i < width; ++i) {
    gauge << (i < filled ? '#' : '-');
  }
  gauge << "] " << std::fixed << std::setprecision(1) << (value * 100.0) << "%";

  return gauge.str();
}

// Implement generateThemedBar
std::string ResourceVisualization::generateThemedBar(double value,
                                                     int width) const {
  // Simple themed bar implementation (just using '#' characters)
  std::ostringstream bar;

  // Ensure value is between 0 and 1
  value = std::max(0.0, std::min(1.0, value));

  // Calculate the number of filled positions
  int filled = static_cast<int>(value * width);

  // Generate the bar
  for (int i = 0; i < width; ++i) {
    bar << (i < filled ? '#' : ' ');
  }

  return bar.str();
}

// Implement the VisualizationTheme::createTheme static method
VisualizationTheme VisualizationTheme::createTheme(ThemeType type) {
  VisualizationTheme theme;

  switch (type) {
  case ThemeType::DARK:
    theme.title_color = "cyan";
    theme.header_color = "blue";
    theme.text_color = "white";
    theme.positive_color = "green";
    theme.negative_color = "red";
    theme.warning_color = "yellow";
    theme.critical_color = "red";
    theme.text_muted = "darkgray";
    theme.html_background_color = "#1e1e1e";
    theme.html_text_color = "#ffffff";
    theme.html_header_background = "#333333";
    theme.html_accent_color = "#61dafb";
    theme.ascii_chars.bar_character = '#';
    theme.ascii_chars.border_horizontal = '-';
    theme.ascii_chars.border_vertical = '|';
    theme.ascii_chars.border_corner = '+';
    theme.ascii_chars.axis_marker = '+';
    theme.ascii_chars.data_point = '*';
    theme.ascii_chars.uptrend_marker = "^";
    theme.ascii_chars.downtrend_marker = "v";
    theme.ascii_chars.stable_marker = ">";
    theme.ascii_chars.filled_gauge = '#';
    theme.ascii_chars.empty_gauge = '.';
    break;

  case ThemeType::LIGHT:
    theme.title_color = "blue";
    theme.header_color = "navy";
    theme.text_color = "black";
    theme.positive_color = "darkgreen";
    theme.negative_color = "darkred";
    theme.warning_color = "orange";
    theme.critical_color = "red";
    theme.text_muted = "gray";
    theme.html_background_color = "#ffffff";
    theme.html_text_color = "#000000";
    theme.html_header_background = "#f8f9fa";
    theme.html_accent_color = "#007bff";
    theme.ascii_chars.bar_character = '=';
    theme.ascii_chars.data_point = 'o';
    theme.ascii_chars.uptrend_marker = "^";
    theme.ascii_chars.downtrend_marker = "v";
    theme.ascii_chars.stable_marker = ">";
    theme.ascii_chars.filled_gauge = '=';
    theme.ascii_chars.empty_gauge = '-';
    break;

  case ThemeType::MINIMAL:
    theme.title_color = "default";
    theme.header_color = "default";
    theme.text_color = "default";
    theme.positive_color = "default";
    theme.negative_color = "default";
    theme.warning_color = "default";
    theme.critical_color = "default";
    theme.text_muted = "default";
    theme.use_colors = false;
    theme.use_bold = false;
    theme.use_unicode = false;
    theme.ascii_chars.bar_character = '#';
    theme.ascii_chars.border_horizontal = '-';
    theme.ascii_chars.border_vertical = '|';
    theme.ascii_chars.border_corner = '+';
    theme.ascii_chars.axis_marker = '+';
    theme.ascii_chars.data_point = '*';
    theme.ascii_chars.uptrend_marker = "^";
    theme.ascii_chars.downtrend_marker = "v";
    theme.ascii_chars.stable_marker = ">";
    theme.ascii_chars.filled_gauge = '#';
    theme.ascii_chars.empty_gauge = '-';
    break;

  case ThemeType::ELABORATE:
    theme.title_color = "magenta";
    theme.header_color = "blue";
    theme.text_color = "cyan";
    theme.positive_color = "green";
    theme.negative_color = "red";
    theme.warning_color = "yellow";
    theme.critical_color = "red";
    theme.text_muted = "gray";
    theme.html_background_color = "#2c3e50";
    theme.html_text_color = "#ecf0f1";
    theme.html_header_background = "#34495e";
    theme.html_accent_color = "#9b59b6";
    theme.ascii_chars.bar_character = '#';
    theme.ascii_chars.border_horizontal = '=';
    theme.ascii_chars.border_vertical = '|';
    theme.ascii_chars.border_corner = '+';
    theme.ascii_chars.axis_marker = '+';
    theme.ascii_chars.data_point = '@';
    theme.ascii_chars.uptrend_marker = "^";
    theme.ascii_chars.downtrend_marker = "v";
    theme.ascii_chars.stable_marker = ">";
    theme.ascii_chars.filled_gauge = '#';
    theme.ascii_chars.empty_gauge = '.';
    break;

  case ThemeType::CUSTOM:
    // For custom theme, we just return the default and expect the caller to
    // modify it
    break;

  case ThemeType::DEFAULT:
  default:
    // Default theme is already set up by the struct's default values
    break;
  }

  return theme;
}

// Dashboard related methods - Stubs
std::string ResourceVisualization::updateDashboard(VisualizationFormat format) {
  std::stringstream ss;

  // Generate a header based on the format
  if (format == VisualizationFormat::TEXT) {
    ss << generateDashboardHeader(format);
  } else if (format == VisualizationFormat::JSON) {
    ss << "{\n  \"report_type\": \"dashboard\",\n";
    ss << "  \"generated_at\": \"" << getCurrentTimestampString() << "\",\n";
    ss << "  \"data\": {\n";
  } else if (format == VisualizationFormat::HTML) {
    ss << "<div class='resource-dashboard'>\n";
    ss << "<h2>Chronovyan Resource Dashboard</h2>\n";
    ss << "<p>Generated: " << getCurrentTimestampString() << "</p>\n";
    ss << "<div class='dashboard-content'>\n";
  }

  // Add sections based on dashboard configuration
  if (m_dashboard_config.show_resource_levels) {
    ss << generateResourceSection(format);
  }

  if (m_dashboard_config.show_debt_status) {
    ss << generateDebtSection(format);
  }

  if (m_dashboard_config.show_efficiency_metrics) {
    ss << generateEfficiencySection(format);
  }

  if (m_dashboard_config.show_trends) {
    ss << generateTrendsSection(format);
  }

  // Add footer based on format
  if (format == VisualizationFormat::TEXT) {
    ss << generateDashboardFooter(format);
  } else if (format == VisualizationFormat::JSON) {
    ss << "  }\n}";
  } else if (format == VisualizationFormat::HTML) {
    ss << "</div></div>";
  }

  return ss.str();
}

void ResourceVisualization::setDashboardConfiguration(
    const DashboardConfiguration &config) {
  m_dashboard_config = config; // Save the configuration
}

DashboardConfiguration
ResourceVisualization::getDashboardConfiguration() const {
  return m_dashboard_config; // Return the current configuration
}

bool ResourceVisualization::startRealTimeMonitoring(
    std::function<void(const std::string &)> update_callback,
    const DashboardConfiguration &config) {
  // Lock for thread safety
  std::lock_guard<std::mutex> lock(m_monitor_mutex);

  std::cerr << "Starting real-time monitoring" << std::endl;

  // If monitoring is already active, return false
  if (m_monitoring_active) {
    std::cerr << "Monitoring is already active, returning false" << std::endl;
    return false;
  }

  // Save the configuration and callback
  m_dashboard_config = config;
  m_update_callback = update_callback;

  // Set the monitoring flag and start the thread
  m_monitoring_active = true;

  std::cerr << "Calling update_callback immediately" << std::endl;
  // Call the callback immediately with the initial dashboard
  if (m_update_callback) {
    std::string dash = updateDashboard();
    std::cerr << "Initial dashboard length: " << dash.length() << std::endl;
    m_update_callback(dash);
    std::cerr << "Called update_callback" << std::endl;
  } else {
    std::cerr << "update_callback is null" << std::endl;
  }

  std::cerr << "Starting monitor thread" << std::endl;
  m_monitor_thread =
      std::thread(&ResourceVisualization::monitorResources, this);

  return true;
}

bool ResourceVisualization::stopRealTimeMonitoring() {
  // Lock for thread safety
  std::lock_guard<std::mutex> lock(m_monitor_mutex);

  // If monitoring is not active, return false
  if (!m_monitoring_active) {
    return false;
  }

  // Stop the monitoring thread
  m_monitoring_active = false;
  if (m_monitor_thread.joinable()) {
    m_monitor_thread.join();
  }

  return true;
}

void ResourceVisualization::monitorResources() {
  while (m_monitoring_active) {
    // Generate and send the dashboard update via the callback
    if (m_update_callback) {
      m_update_callback(updateDashboard());
    }

    // Sleep for the configured update interval
    std::this_thread::sleep_for(
        std::chrono::milliseconds(m_dashboard_config.update_frequency_ms));
  }
}

std::string
ResourceVisualization::generateDashboardHeader(VisualizationFormat format) {
  std::ostringstream header;

  switch (format) {
  case VisualizationFormat::TEXT:
  default:
    header << "REAL-TIME RESOURCE MONITORING DASHBOARD\n";
    header << "=======================================\n\n";
    header << "Update frequency: " << m_dashboard_config.update_frequency_ms
           << "ms\n";
    header << "Dashboard mode: ";

    switch (m_dashboard_config.mode) {
    case DashboardMode::HIGH_FIDELITY:
      header << "HIGH FIDELITY";
      break;
    case DashboardMode::BALANCED:
      header << "BALANCED";
      break;
    case DashboardMode::LEAN:
      header << "LEAN";
      break;
    case DashboardMode::AUTO:
      header << "AUTO";
      break;
    case DashboardMode::MINIMAL:
      header << "MINIMAL";
      break;
    case DashboardMode::PERFORMANCE:
      header << "PERFORMANCE";
      break;
    }
    header << "\n\n";
    break;
  }

  return header.str();
}

std::string
ResourceVisualization::generateDashboardFooter(VisualizationFormat format) {
  std::ostringstream footer;

  switch (format) {
  case VisualizationFormat::TEXT:
  default:
    footer << "=======================================\n";
    footer << "End of Real-time Resource Dashboard\n";
    break;
  }

  return footer.str();
}

std::string
ResourceVisualization::generateResourceSection(VisualizationFormat format) {
  std::ostringstream section;

  switch (format) {
  case VisualizationFormat::TEXT:
  default:
    section << "CURRENT RESOURCE LEVELS\n";
    section << "----------------------\n";

    if (m_runtime) {
      section << "Chronons: " << m_runtime->getChrononsLevel() << "\n";
      section << "Aethel: " << m_runtime->getAethelLevel() << "\n";
    } else {
      section << "No runtime available for resource levels.\n";
    }
    section << "\n";
    break;
  }

  return section.str();
}

std::string
ResourceVisualization::generateDebtSection(VisualizationFormat format) {
  std::ostringstream section;

  switch (format) {
  case VisualizationFormat::TEXT:
  default:
    section << "DEBT STATUS\n";
    section << "-----------\n";

    if (m_debt_tracker) {
      section << "Chronon debt: " << m_debt_tracker->getTotalChrononsDebt()
              << "\n";
      section << "Aethel debt: " << m_debt_tracker->getTotalAethelDebt()
              << "\n";
      section << "Stability: " << m_debt_tracker->getStability() << "\n";
      section << "Paradox risk: " << m_debt_tracker->getParadoxRisk() << "\n";
    } else {
      section << "No debt tracker available for debt status.\n";
    }
    section << "\n";
    break;
  }

  return section.str();
}

std::string
ResourceVisualization::generateEfficiencySection(VisualizationFormat format) {
  std::ostringstream section;

  switch (format) {
  case VisualizationFormat::TEXT:
  default:
    section << "EFFICIENCY METRICS\n";
    section << "------------------\n";

    if (m_optimizer) {
      auto metrics = m_optimizer->getEfficiencyMetrics();
      for (const auto &metric : metrics) {
        section << metric.first << ": " << metric.second << "\n";
      }
    } else {
      section << "No optimizer available for efficiency metrics.\n";
    }
    section << "\n";
    break;
  }

  return section.str();
}

std::string
ResourceVisualization::generateTrendsSection(VisualizationFormat format) {
  std::ostringstream section;

  switch (format) {
  case VisualizationFormat::TEXT:
  default:
    section << "RESOURCE TRENDS\n";
    section << "--------------\n";

    if (m_history_manager && !m_history_manager->isEmpty()) {
      auto history = m_history_manager->getRecentEntries(5);
      if (history.empty()) {
        section << "No historical data available for trends.\n";
      } else {
        section << "Recent resource trends (last " << history.size()
                << " snapshots):\n\n";

        // Show chronon trends
        section << "Chronon levels: ";
        for (const auto &entry : history) {
          section << entry.getDoubleStat("chronons", 0.0) << " ";
        }
        section << "\n";

        // Show aethel trends
        section << "Aethel levels: ";
        for (const auto &entry : history) {
          section << entry.getDoubleStat("aethel", 0.0) << " ";
        }
        section << "\n";
      }
    } else {
      section << "No history manager available for trends.\n";
    }
    section << "\n";
    break;
  }

  return section.str();
}

bool ResourceVisualization::shouldUpdateDashboard() const {
  auto now = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                     now - m_last_update_time)
                     .count();
  return elapsed >= m_dashboard_config.update_frequency_ms;
}

// ADDED STUB IMPLEMENTATIONS
std::string
ResourceVisualization::visualizeCurrentState(VisualizationFormat /*format*/) {
  // In a previous step, we attempted to change takeSnapshot() calls here.
  // For now, ensuring a stub exists. Actual logic would call
  // takeSnapshot("VisualizeCurrentState").
  return "[Stub] Current state visualization not fully implemented.";
}

std::string
ResourceVisualization::visualizeTrends(int /*history_length*/,
                                       VisualizationFormat /*format*/) {
  // In a previous step, we attempted to change takeSnapshot() calls here.
  // For now, ensuring a stub exists. Actual logic would call
  // takeSnapshot("VisualizeTrends").
  return "[Stub] Trends visualization not fully implemented.";
}

std::string
ResourceVisualization::visualizeDebtStatus(VisualizationFormat /*format*/) {
  // In a previous step, we attempted to change takeSnapshot() calls here.
  // For now, ensuring a stub exists. Actual logic would call
  // takeSnapshot("VisualizeDebtStatus").
  return "[Stub] Debt status visualization not fully implemented.";
}

std::string ResourceVisualization::visualizeOptimizationOpportunities(
    VisualizationFormat /*format*/) {
  auto snapshot = takeSnapshot("OptimizeOpportunities");
  if (!snapshot)
    return "[Error] Failed to get snapshot for Optimization Opportunities.";
  // Placeholder: Access some data from snapshot if needed for a basic stub
  std::string suggestion = getStringStat(
      snapshot->m_resource_stats, "LAST_OPTIMIZATION_SUGGESTION", "None");
  return "[Stub] Optimization opportunities: " + suggestion;
}

std::string ResourceVisualization::visualizeRebelOperationImpact(
    RebelOperationType /*type*/, int /*projection_cycles*/,
    int /*operations_per_cycle*/, VisualizationFormat /*format*/) {
  auto snapshot = takeSnapshot("RebelImpact");
  if (!snapshot)
    return "[Error] Failed to get snapshot for Rebel Impact.";
  return "[Stub] Rebel operation impact not fully implemented.";
}

std::string
ResourceVisualization::generateDashboard(VisualizationFormat /*format*/) {
  auto snapshot = takeSnapshot("GenerateDashboard");
  if (!snapshot)
    return "[Error] Failed to get snapshot for Dashboard.";
  return "[Stub] Dashboard generation not fully implemented.";
}

// Implement the missing generateSummaryReport method
std::string
ResourceVisualization::generateSummaryReport(VisualizationFormat format) const {
  std::stringstream ss;

  // Generate a header based on the format
  if (format == VisualizationFormat::TEXT) {
    ss << "Resource Visualization Summary Report" << std::endl;
    ss << "=====================================" << std::endl;
    ss << "Generated: " << getCurrentTimestampString() << std::endl;
    ss << "-------------------------------------" << std::endl;
  } else if (format == VisualizationFormat::JSON) {
    ss << "{\n  \"report_type\": \"summary\",\n";
    ss << "  \"generated_at\": \"" << getCurrentTimestampString() << "\",\n";
    ss << "  \"data\": {\n";
  } else if (format == VisualizationFormat::HTML) {
    ss << "<div class='resource-summary-report'>\n";
    ss << "<h2>Resource Visualization Summary Report</h2>\n";
    ss << "<p>Generated: " << getCurrentTimestampString() << "</p>\n";
    ss << "<div class='report-content'>\n";
  }

  // Take a snapshot of the current state
  auto snapshot = takeSnapshot("Summary Report");

  // Current Resources section
  if (format == VisualizationFormat::TEXT) {
    ss << "Current Resources" << std::endl;
    ss << "  Chronons: "
       << getDoubleStat(snapshot->m_resource_stats, "CHRONONS_LEVEL", 0.0)
       << std::endl;
    ss << "  Aethel: "
       << getDoubleStat(snapshot->m_resource_stats, "AETHEL_LEVEL", 0.0)
       << std::endl;
    ss << std::endl;

    ss << "Debt Status" << std::endl;
    ss << "  Overall Debt: "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_DEBT_LEVEL", 0.0)
       << std::endl;
    ss << "  Paradox Risk: "
       << getDoubleStat(snapshot->m_resource_stats, "PARADOX_RISK", 0.0)
       << std::endl;
    ss << std::endl;

    ss << "Efficiency" << std::endl;
    ss << "  Overall Efficiency: "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_EFFICIENCY", 1.0)
       << std::endl;
  } else if (format == VisualizationFormat::JSON) {
    ss << "    \"current_resources\": {\n";
    ss << "      \"chronons\": "
       << getDoubleStat(snapshot->m_resource_stats, "CHRONONS_LEVEL", 0.0)
       << ",\n";
    ss << "      \"aethel\": "
       << getDoubleStat(snapshot->m_resource_stats, "AETHEL_LEVEL", 0.0)
       << "\n";
    ss << "    },\n";

    ss << "    \"debt_status\": {\n";
    ss << "      \"overall_debt\": "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_DEBT_LEVEL", 0.0)
       << ",\n";
    ss << "      \"paradox_risk\": "
       << getDoubleStat(snapshot->m_resource_stats, "PARADOX_RISK", 0.0)
       << "\n";
    ss << "    },\n";

    ss << "    \"efficiency\": {\n";
    ss << "      \"overall_efficiency\": "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_EFFICIENCY", 1.0)
       << "\n";
    ss << "    }\n";
  } else if (format == VisualizationFormat::HTML) {
    ss << "<h3>Current Resources</h3>\n";
    ss << "<ul>\n";
    ss << "  <li>Chronons: "
       << getDoubleStat(snapshot->m_resource_stats, "CHRONONS_LEVEL", 0.0)
       << "</li>\n";
    ss << "  <li>Aethel: "
       << getDoubleStat(snapshot->m_resource_stats, "AETHEL_LEVEL", 0.0)
       << "</li>\n";
    ss << "</ul>\n";

    ss << "<h3>Debt Status</h3>\n";
    ss << "<ul>\n";
    ss << "  <li>Overall Debt: "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_DEBT_LEVEL", 0.0)
       << "</li>\n";
    ss << "  <li>Paradox Risk: "
       << getDoubleStat(snapshot->m_resource_stats, "PARADOX_RISK", 0.0)
       << "</li>\n";
    ss << "</ul>\n";

    ss << "<h3>Efficiency</h3>\n";
    ss << "<ul>\n";
    ss << "  <li>Overall Efficiency: "
       << getDoubleStat(snapshot->m_resource_stats, "OVERALL_EFFICIENCY", 1.0)
       << "</li>\n";
    ss << "</ul>\n";
  }

  // Add footer based on format
  if (format == VisualizationFormat::TEXT) {
    ss << "-------------------------------------" << std::endl;
    ss << "End of Summary Report" << std::endl;
  } else if (format == VisualizationFormat::JSON) {
    ss << "  }\n}";
  } else if (format == VisualizationFormat::HTML) {
    ss << "</div></div>";
  }

  return ss.str();
}

// Helper function to get current timestamp as string
std::string ResourceVisualization::getCurrentTimestampString() const {
  auto now = std::chrono::system_clock::now();
  auto time_t_now = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

std::shared_ptr<ResourceSnapshot>
ResourceVisualization::takeSnapshot(const std::string &context) const {
  // Create a new ResourceSnapshot using the static method
  auto snapshot =
      std::make_shared<ResourceSnapshot>(ResourceSnapshot::takeSnapshot(
          m_runtime, m_debt_tracker, context, m_optimizer, m_graph_visualizer));

  // Add the snapshot to the history
  {
    std::lock_guard<std::mutex> lock(m_history_mutex);
    m_history.push_front(snapshot);

    // Limit history size if needed
    const size_t MAX_HISTORY_SIZE = 100;
    while (m_history.size() > MAX_HISTORY_SIZE) {
      m_history.pop_back();
    }
  }

  return snapshot;
}

} // namespace chronovyan