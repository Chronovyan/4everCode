#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "resource_visualizer.h"

namespace chronovyan {

ResourceVisualizer::ResourceVisualizer(const ResourceTracker &tracker)
    : tracker_(tracker) {}

std::string ResourceVisualizer::generateAsciiGraph(size_t width,
                                                   size_t height) const {
  const auto &data = tracker_.getHistoricalData();
  if (data.empty()) {
    return "No data available for visualization.";
  }

  // Extract chronon usage values for histogram
  std::vector<double> chronon_values;
  std::vector<double> aethel_values;
  for (const auto &point : data) {
    chronon_values.push_back(point.chronon_usage);
    aethel_values.push_back(point.aethel_usage);
  }

  std::stringstream ss;
  ss << "Chronovyan Resource Visualization\n";
  ss << "==================================\n\n";

  // Generate chronon usage histogram
  ss << "Chronon Usage:\n";
  ss << generateAsciiHistogram(chronon_values, width, height / 2);
  ss << "\n\n";

  // Generate aethel usage histogram
  ss << "Aethel Usage:\n";
  ss << generateAsciiHistogram(aethel_values, width, height / 2);
  ss << "\n\n";

  // Generate timeline view if enough data points
  if (data.size() > 1) {
    ss << "Resource Timeline:\n";
    ss << generateAsciiTimeline(data, width);
    ss << "\n";

    // Add legend
    ss << generateLegend();
    ss << "\n";
  }

  return ss.str();
}

std::string ResourceVisualizer::generateTextSummary() const {
  auto stats = tracker_.getResourceStatistics();
  auto data = tracker_.getHistoricalData();

  std::ostringstream summary;
  summary << "Resource Usage Summary" << std::endl;
  summary << "------------------" << std::endl;

  if (data.empty()) {
    summary << "No data available" << std::endl;
    return summary.str();
  }

  summary << "Current Chronon Usage: " << std::fixed << std::setprecision(2)
          << stats["current_chronon_usage"] << std::endl;
  summary << "Current Aethel Usage: " << std::fixed << std::setprecision(2)
          << stats["current_aethel_usage"] << std::endl;
  summary << "Current Temporal Debt: " << std::fixed << std::setprecision(2)
          << stats["current_temporal_debt"] << std::endl;
  summary << "Current Paradox Risk: " << std::fixed << std::setprecision(2)
          << stats["current_paradox_risk"] << std::endl;

  // Add risk assessment for paradox risk
  double paradoxRisk = stats["current_paradox_risk"];
  summary << "Risk Assessment: ";
  if (paradoxRisk < 0.2) {
    summary << "Safe";
  } else if (paradoxRisk < 0.4) {
    summary << "Moderate";
  } else if (paradoxRisk < 0.7) {
    summary << "High";
  } else {
    summary << "CRITICAL";
  }
  summary << std::endl;

  return summary.str();
}

std::string ResourceVisualizer::generateDetailedReport() const {
  auto stats = tracker_.getResourceStatistics();
  auto data = tracker_.getHistoricalData();

  std::ostringstream report;
  report << "Chronovyan Resource Detailed Report" << std::endl;
  report << "==================================" << std::endl;

  if (data.empty()) {
    report << "No data available" << std::endl;
    return report.str();
  }

  // Current values
  report << "CURRENT VALUES:" << std::endl;
  report << "  Chronon Usage: " << std::fixed << std::setprecision(2)
         << stats["current_chronon_usage"] << std::endl;
  report << "  Aethel Usage: " << std::fixed << std::setprecision(2)
         << stats["current_aethel_usage"] << std::endl;
  report << "  Temporal Debt: " << std::fixed << std::setprecision(2)
         << stats["current_temporal_debt"] << std::endl;
  report << "  Paradox Risk: " << std::fixed << std::setprecision(2)
         << stats["current_paradox_risk"] << std::endl;
  report << std::endl;

  // Historical averages
  report << "HISTORICAL AVERAGES:" << std::endl;
  report << "  Avg Chronon Usage: " << std::fixed << std::setprecision(2)
         << stats["avg_chronon_usage"] << std::endl;
  report << "  Avg Aethel Usage: " << std::fixed << std::setprecision(2)
         << stats["avg_aethel_usage"] << std::endl;
  report << "  Avg Temporal Debt: " << std::fixed << std::setprecision(2)
         << stats["avg_temporal_debt"] << std::endl;
  report << "  Avg Paradox Risk: " << std::fixed << std::setprecision(2)
         << stats["avg_paradox_risk"] << std::endl;
  report << std::endl;

  // Maximum values
  report << "MAXIMUM VALUES:" << std::endl;
  report << "  Max Chronon Usage: " << std::fixed << std::setprecision(2)
         << stats["max_chronon_usage"] << std::endl;
  report << "  Max Aethel Usage: " << std::fixed << std::setprecision(2)
         << stats["max_aethel_usage"] << std::endl;
  report << "  Max Temporal Debt: " << std::fixed << std::setprecision(2)
         << stats["max_temporal_debt"] << std::endl;
  report << "  Max Paradox Risk: " << std::fixed << std::setprecision(2)
         << stats["max_paradox_risk"] << std::endl;
  report << std::endl;

  // Efficiency indicators
  report << "EFFICIENCY INDICATORS:" << std::endl;
  report << "  Chronon Efficiency: " << std::fixed << std::setprecision(2)
         << (stats["current_chronon_usage"] /
             (stats["max_chronon_usage"] + 0.0001) * 100)
         << "%" << std::endl;
  report << "  Aethel Efficiency: " << std::fixed << std::setprecision(2)
         << (stats["current_aethel_usage"] /
             (stats["max_aethel_usage"] + 0.0001) * 100)
         << "%" << std::endl;
  report << "  Temporal Stability: " << std::fixed << std::setprecision(2)
         << (100 - (stats["current_temporal_debt"] /
                    (stats["max_temporal_debt"] + 0.0001) * 100))
         << "%" << std::endl;
  report << "  Paradox Containment: " << std::fixed << std::setprecision(2)
         << (100 - (stats["current_paradox_risk"] /
                    (stats["max_paradox_risk"] + 0.0001) * 100))
         << "%" << std::endl;
  report << std::endl;

  // Data point count
  report << "Data points in history: " << data.size() << std::endl;

  return report.str();
}

std::string
ResourceVisualizer::generateAsciiHistogram(const std::vector<double> &values,
                                           size_t width, size_t height) const {
  if (values.empty()) {
    return "[No data]";
  }

  // Normalize values to fit within specified height
  auto normalized = normalizeValues(values, height);

  std::stringstream ss;

  // Generate histogram rows from top to bottom
  for (size_t row = height; row > 0; --row) {
    ss << std::setw(3) << row << " |";

    // Determine how many data points to show based on width
    size_t step = std::max(size_t(1), values.size() / (width - 5));

    for (size_t i = 0; i < normalized.size(); i += step) {
      if (normalized[i] >= row) {
        ss << "#";
      } else {
        ss << " ";
      }
    }
    ss << "\n";
  }

  // Generate x-axis
  ss << "    +";
  for (size_t i = 0; i < width - 5; ++i) {
    ss << "-";
  }
  ss << "\n";

  // Add scale markers if enough width
  if (width >= 20) {
    ss << "     ";
    size_t step = std::max(size_t(1), values.size() / (width - 5));
    size_t marker_step = (width - 5) / 4;

    for (size_t i = 0; i < width - 5; ++i) {
      if (i % marker_step == 0) {
        size_t data_idx = i * step;
        if (data_idx < values.size()) {
          ss << std::setw(marker_step) << std::left << data_idx;
        }
      }
    }
  }

  return ss.str();
}

std::string ResourceVisualizer::generateAsciiTimeline(
    const std::vector<ResourceDataPoint> &data, size_t width) const {
  if (data.size() < 2) {
    return "[Insufficient data for timeline]";
  }

  std::stringstream ss;

  // Simple ASCII timeline showing resource trends
  const size_t display_points = std::min(data.size(), width - 10);
  size_t step = std::max(size_t(1), data.size() / display_points);

  // Chronon line
  ss << "Chronon: ";
  for (size_t i = 0; i < data.size(); i += step) {
    if (i > 0) {
      if (data[i].chronon_usage > data[i - step].chronon_usage) {
        ss << "/";
      } else if (data[i].chronon_usage < data[i - step].chronon_usage) {
        ss << "\\";
      } else {
        ss << "-";
      }
    } else {
      ss << "*";
    }
  }
  ss << "\n";

  // Aethel line
  ss << "Aethel:  ";
  for (size_t i = 0; i < data.size(); i += step) {
    if (i > 0) {
      if (data[i].aethel_usage > data[i - step].aethel_usage) {
        ss << "/";
      } else if (data[i].aethel_usage < data[i - step].aethel_usage) {
        ss << "\\";
      } else {
        ss << "-";
      }
    } else {
      ss << "*";
    }
  }
  ss << "\n";

  // Debt line if any data points have debt
  bool has_debt = false;
  for (const auto &point : data) {
    if (point.temporal_debt > 0) {
      has_debt = true;
      break;
    }
  }

  if (has_debt) {
    ss << "Debt:    ";
    for (size_t i = 0; i < data.size(); i += step) {
      if (i > 0) {
        if (data[i].temporal_debt > data[i - step].temporal_debt) {
          ss << "/";
        } else if (data[i].temporal_debt < data[i - step].temporal_debt) {
          ss << "\\";
        } else {
          ss << "-";
        }
      } else {
        ss << "*";
      }
    }
    ss << "\n";
  }

  return ss.str();
}

std::vector<size_t>
ResourceVisualizer::normalizeValues(const std::vector<double> &values,
                                    size_t height) const {
  if (values.empty() || height == 0) {
    return {};
  }

  // Find the maximum value
  double max_value = *std::max_element(values.begin(), values.end());

  // If all values are zero, return all zeros
  if (max_value == 0) {
    return std::vector<size_t>(values.size(), 0);
  }

  // Normalize values to the range [0, height]
  std::vector<size_t> normalized;
  normalized.reserve(values.size());

  for (double value : values) {
    // Map value to height, with a minimum of 1 if value is non-zero
    size_t scaled =
        value > 0 ? std::max(size_t(1),
                             static_cast<size_t>((value / max_value) * height))
                  : 0;
    normalized.push_back(scaled);
  }

  return normalized;
}

std::string ResourceVisualizer::generateAsciiBar(size_t height,
                                                 size_t max_height) const {
  std::string bar;

  // Full blocks for complete units
  for (size_t i = 0; i < height; ++i) {
    bar += "#";
  }

  // Empty space for remaining height
  for (size_t i = height; i < max_height; ++i) {
    bar += " ";
  }

  return bar;
}

// New method to generate a legend for the visualization
std::string ResourceVisualizer::generateLegend() const {
  std::stringstream ss;

  ss << "Legend:\n";
  ss << "-------\n";
  ss << "# = Resource level bar\n";
  ss << "* = Starting point\n";
  ss << "/ = Increasing trend\n";
  ss << "\\ = Decreasing trend\n";
  ss << "- = Stable trend\n";

  return ss.str();
}

} // namespace chronovyan