#include <fstream>
#include <iomanip>
#include <sstream>

#include "compiler/proving_grounds/TemporalBenchmark.h"

namespace chronovyan {
namespace compiler {
namespace proving_grounds {

TemporalBenchmark::TemporalBenchmark() : isMeasuring(false) {}

TemporalBenchmark::~TemporalBenchmark() = default;

void TemporalBenchmark::startMeasuring() {
  if (!isMeasuring) {
    isMeasuring = true;
    startTime = std::chrono::high_resolution_clock::now();
  }
}

void TemporalBenchmark::stopMeasuring() {
  if (isMeasuring) {
    isMeasuring = false;
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the total duration in milliseconds
    double duration =
        std::chrono::duration<double, std::milli>(endTime - startTime).count();

    // Record the duration for the "total" operation
    operationDurations["total"] = duration;
  }
}

void TemporalBenchmark::reset() {
  isMeasuring = false;
  operationStartTimes.clear();
  operationDurations.clear();
  resourceUsages.clear();
}

void TemporalBenchmark::markOperationStart(const std::string &operationName) {
  if (isMeasuring) {
    operationStartTimes[operationName] =
        std::chrono::high_resolution_clock::now();
  }
}

void TemporalBenchmark::markOperationEnd(const std::string &operationName) {
  if (isMeasuring &&
      operationStartTimes.find(operationName) != operationStartTimes.end()) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto startTime = operationStartTimes[operationName];

    // Calculate the duration in milliseconds
    double duration =
        std::chrono::duration<double, std::milli>(endTime - startTime).count();

    // Record the duration
    operationDurations[operationName] = duration;
  }
}

double TemporalBenchmark::getOperationDuration(
    const std::string &operationName) const {
  auto it = operationDurations.find(operationName);
  if (it != operationDurations.end()) {
    return it->second;
  }
  return 0.0;
}

std::map<std::string, double>
TemporalBenchmark::getAllOperationDurations() const {
  return operationDurations;
}

void TemporalBenchmark::recordResourceUsage(const std::string &resourceName,
                                            double usage) {
  resourceUsages[resourceName] = usage;
}

double
TemporalBenchmark::getResourceUsage(const std::string &resourceName) const {
  auto it = resourceUsages.find(resourceName);
  if (it != resourceUsages.end()) {
    return it->second;
  }
  return 0.0;
}

std::map<std::string, double> TemporalBenchmark::getAllResourceUsages() const {
  return resourceUsages;
}

double
TemporalBenchmark::calculateEfficiency(const std::string &operationName,
                                       const std::string &resourceName) const {
  double duration = getOperationDuration(operationName);
  double usage = getResourceUsage(resourceName);

  if (usage > 0.0) {
    // Efficiency is the amount of work done per unit of resource
    // We'll use the inverse of duration as the amount of work done
    // (i.e., faster is better)
    return 1000.0 / (duration * usage);
  }

  return 0.0;
}

std::map<std::string, double> TemporalBenchmark::calculateAllEfficiencies(
    const std::string &operationName) const {
  std::map<std::string, double> efficiencies;

  for (const auto &[resourceName, usage] : resourceUsages) {
    efficiencies[resourceName] =
        calculateEfficiency(operationName, resourceName);
  }

  return efficiencies;
}

TemporalBenchmark::BenchmarkResult
TemporalBenchmark::getBenchmarkResult(const std::string &operationName) const {
  BenchmarkResult result;
  result.duration = getOperationDuration(operationName);
  result.resourceUsage = resourceUsages;
  result.efficiency = calculateAllEfficiencies(operationName);

  return result;
}

std::map<std::string, TemporalBenchmark::BenchmarkResult>
TemporalBenchmark::getAllBenchmarkResults() const {
  std::map<std::string, BenchmarkResult> results;

  for (const auto &[operationName, duration] : operationDurations) {
    results[operationName] = getBenchmarkResult(operationName);
  }

  return results;
}

std::string TemporalBenchmark::generateReport(const std::string &format) const {
  std::ostringstream report;

  if (format == "json") {
    // Generate JSON report
    report << "{\n";

    // Operations
    report << "  \"operations\": {\n";
    size_t opCount = 0;
    for (const auto &[operationName, duration] : operationDurations) {
      report << "    \"" << operationName << "\": {\n";
      report << "      \"duration\": " << duration << ",\n";

      // Efficiencies
      report << "      \"efficiency\": {\n";
      const auto &efficiencies = calculateAllEfficiencies(operationName);
      size_t effCount = 0;
      for (const auto &[resourceName, efficiency] : efficiencies) {
        report << "        \"" << resourceName << "\": " << efficiency;
        if (++effCount < efficiencies.size()) {
          report << ",";
        }
        report << "\n";
      }
      report << "      }\n";

      report << "    }";
      if (++opCount < operationDurations.size()) {
        report << ",";
      }
      report << "\n";
    }
    report << "  },\n";

    // Resources
    report << "  \"resources\": {\n";
    size_t resCount = 0;
    for (const auto &[resourceName, usage] : resourceUsages) {
      report << "    \"" << resourceName << "\": " << usage;
      if (++resCount < resourceUsages.size()) {
        report << ",";
      }
      report << "\n";
    }
    report << "  }\n";

    report << "}\n";
  } else if (format == "html") {
    // Generate HTML report
    report << "<!DOCTYPE html>\n";
    report << "<html>\n";
    report << "<head>\n";
    report << "  <title>Chronovyan Temporal Benchmark Report</title>\n";
    report << "  <style>\n";
    report << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    report << "    h1, h2 { color: #336699; }\n";
    report << "    table { border-collapse: collapse; width: 100%; "
              "margin-bottom: 20px; }\n";
    report << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: "
              "left; }\n";
    report << "    th { background-color: #f2f2f2; }\n";
    report << "    tr:nth-child(even) { background-color: #f9f9f9; }\n";
    report << "  </style>\n";
    report << "</head>\n";
    report << "<body>\n";

    report << "  <h1>Chronovyan Temporal Benchmark Report</h1>\n";

    // Operations
    report << "  <h2>Operations</h2>\n";
    report << "  <table>\n";
    report << "    <tr><th>Operation</th><th>Duration (ms)</th></tr>\n";
    for (const auto &[operationName, duration] : operationDurations) {
      report << "    <tr><td>" << operationName << "</td><td>" << duration
             << "</td></tr>\n";
    }
    report << "  </table>\n";

    // Resources
    report << "  <h2>Resources</h2>\n";
    report << "  <table>\n";
    report << "    <tr><th>Resource</th><th>Usage</th></tr>\n";
    for (const auto &[resourceName, usage] : resourceUsages) {
      report << "    <tr><td>" << resourceName << "</td><td>" << usage
             << "</td></tr>\n";
    }
    report << "  </table>\n";

    // Efficiencies
    report << "  <h2>Efficiencies</h2>\n";
    report << "  <table>\n";
    report
        << "    "
           "<tr><th>Operation</th><th>Resource</th><th>Efficiency</th></tr>\n";
    for (const auto &[operationName, duration] : operationDurations) {
      const auto &efficiencies = calculateAllEfficiencies(operationName);
      for (const auto &[resourceName, efficiency] : efficiencies) {
        report << "    <tr><td>" << operationName << "</td><td>" << resourceName
               << "</td><td>" << efficiency << "</td></tr>\n";
      }
    }
    report << "  </table>\n";

    report << "</body>\n";
    report << "</html>\n";
  } else {
    // Default to text format
    report << "Chronovyan Temporal Benchmark Report\n";
    report << "===================================\n\n";

    // Operations
    report << "Operations:\n";
    for (const auto &[operationName, duration] : operationDurations) {
      report << "  " << operationName << ": " << duration << " ms\n";
    }
    report << "\n";

    // Resources
    report << "Resources:\n";
    for (const auto &[resourceName, usage] : resourceUsages) {
      report << "  " << resourceName << ": " << usage << "\n";
    }
    report << "\n";

    // Efficiencies
    report << "Efficiencies:\n";
    for (const auto &[operationName, duration] : operationDurations) {
      const auto &efficiencies = calculateAllEfficiencies(operationName);
      if (!efficiencies.empty()) {
        report << "  " << operationName << ":\n";
        for (const auto &[resourceName, efficiency] : efficiencies) {
          report << "    " << resourceName << ": " << efficiency << "\n";
        }
      }
    }
  }

  return report.str();
}

bool TemporalBenchmark::saveReport(const std::string &report,
                                   const std::string &filePath) const {
  std::ofstream file(filePath);
  if (!file) {
    return false;
  }

  file << report;
  return !file.bad();
}

} // namespace proving_grounds
} // namespace compiler
} // namespace chronovyan