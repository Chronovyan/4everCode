#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

#include "compiler/proving_grounds/ParadoxDetector.h"
#include "compiler/proving_grounds/TemporalBenchmark.h"
#include "compiler/proving_grounds/TemporalTestFramework.h"

namespace chronovyan {
namespace compiler {
namespace proving_grounds {

// TemporalTestFramework implementation
TemporalTestFramework::TemporalTestFramework(DiagnosticReporter &diag)
    : diagnostics(diag) {
  paradoxDetector = std::make_shared<ParadoxDetector>(diag);
  benchmark = std::make_shared<TemporalBenchmark>();
  debugger = std::make_shared<debugger::DejaVuDebugger>(diag);
}

TemporalTestFramework::~TemporalTestFramework() = default;

void TemporalTestFramework::registerTestCase(
    std::shared_ptr<TemporalTestCase> testCase) {
  testCases.push_back(testCase);
}

void TemporalTestFramework::registerTestSuite(
    std::shared_ptr<TemporalTestSuite> testSuite) {
  testSuites.push_back(testSuite);
}

std::vector<TemporalTestResult> TemporalTestFramework::runAllTests() {
  std::vector<TemporalTestResult> results;

  // Run individual test cases
  for (const auto &testCase : testCases) {
    results.push_back(runTestCase(testCase));
  }

  // Run test suites
  for (const auto &testSuite : testSuites) {
    const auto &suiteTestCases = testSuite->getTestCases();
    for (const auto &testCase : suiteTestCases) {
      results.push_back(runTestCase(testCase));
    }
  }

  return results;
}

TemporalTestResult
TemporalTestFramework::runTest(const std::string &testCaseName) {
  // Look for the test case in the individual test cases
  for (const auto &testCase : testCases) {
    if (testCase->getName() == testCaseName) {
      return runTestCase(testCase);
    }
  }

  // Look for the test case in the test suites
  for (const auto &testSuite : testSuites) {
    const auto &suiteTestCases = testSuite->getTestCases();
    for (const auto &testCase : suiteTestCases) {
      if (testCase->getName() == testCaseName) {
        return runTestCase(testCase);
      }
    }
  }

  // Test case not found
  return TemporalTestResult(testCaseName, false, "Test case not found");
}

std::vector<TemporalTestResult>
TemporalTestFramework::runTestSuite(const std::string &testSuiteName) {
  std::vector<TemporalTestResult> results;

  // Look for the test suite
  for (const auto &testSuite : testSuites) {
    if (testSuite->getName() == testSuiteName) {
      const auto &suiteTestCases = testSuite->getTestCases();
      for (const auto &testCase : suiteTestCases) {
        results.push_back(runTestCase(testCase));
      }
      return results;
    }
  }

  // Test suite not found
  results.push_back(
      TemporalTestResult(testSuiteName, false, "Test suite not found"));
  return results;
}

std::shared_ptr<ParadoxDetector>
TemporalTestFramework::getParadoxDetector() const {
  return paradoxDetector;
}

std::shared_ptr<TemporalBenchmark>
TemporalTestFramework::getTemporalBenchmark() const {
  return benchmark;
}

std::string TemporalTestFramework::generateReport(
    const std::vector<TemporalTestResult> &results,
    const std::string &format) const {
  std::ostringstream report;

  if (format == "json") {
    // Generate JSON report
    report << "{\n";
    report << "  \"tests\": [\n";

    for (size_t i = 0; i < results.size(); ++i) {
      const auto &result = results[i];

      report << "    {\n";
      report << "      \"name\": \"" << result.getTestName() << "\",\n";
      report << "      \"passed\": " << (result.isPassed() ? "true" : "false")
             << ",\n";
      report << "      \"message\": \"" << result.getMessage() << "\",\n";

      // Paradoxes
      report << "      \"paradoxes\": [\n";
      const auto &paradoxes = result.getParadoxes();
      for (size_t j = 0; j < paradoxes.size(); ++j) {
        const auto &paradox = paradoxes[j];
        report << "        {\n";
        report << "          \"description\": \"" << paradox.description
               << "\",\n";
        report << "          \"severity\": " << paradox.severity << ",\n";
        report << "          \"location\": {\n";
        report << "            \"file\": \"" << paradox.location.file
               << "\",\n";
        report << "            \"line\": " << paradox.location.line << "\n";
        report << "          }\n";
        report << "        }";
        if (j < paradoxes.size() - 1) {
          report << ",";
        }
        report << "\n";
      }
      report << "      ],\n";

      // Benchmark results
      report << "      \"benchmarks\": {\n";
      const auto &benchmarks = result.getBenchmarkResults();
      size_t benchmarkCount = 0;
      for (const auto &[opName, benchResult] : benchmarks) {
        report << "        \"" << opName << "\": {\n";
        report << "          \"duration\": " << benchResult.duration << ",\n";
        report << "          \"resources\": {\n";

        size_t resourceCount = 0;
        for (const auto &[resourceName, usage] : benchResult.resourceUsage) {
          report << "            \"" << resourceName << "\": " << usage;
          if (++resourceCount < benchResult.resourceUsage.size()) {
            report << ",";
          }
          report << "\n";
        }

        report << "          }\n";
        report << "        }";
        if (++benchmarkCount < benchmarks.size()) {
          report << ",";
        }
        report << "\n";
      }
      report << "      }\n";

      report << "    }";
      if (i < results.size() - 1) {
        report << ",";
      }
      report << "\n";
    }

    report << "  ],\n";

    // Summary
    int passCount = 0;
    for (const auto &result : results) {
      if (result.isPassed()) {
        ++passCount;
      }
    }

    report << "  \"summary\": {\n";
    report << "    \"total\": " << results.size() << ",\n";
    report << "    \"passed\": " << passCount << ",\n";
    report << "    \"failed\": " << (results.size() - passCount) << ",\n";
    report << "    \"passPercentage\": "
           << (results.empty() ? 0 : (passCount * 100.0 / results.size()))
           << "\n";
    report << "  }\n";
    report << "}\n";
  } else if (format == "html") {
    // Generate HTML report
    report << "<!DOCTYPE html>\n";
    report << "<html>\n";
    report << "<head>\n";
    report << "  <title>Chronovyan Temporal Testing Report</title>\n";
    report << "  <style>\n";
    report << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    report << "    h1 { color: #336699; }\n";
    report << "    .summary { margin-bottom: 20px; }\n";
    report << "    .test { margin-bottom: 10px; padding: 10px; border: 1px "
              "solid #ccc; }\n";
    report << "    .test-passed { background-color: #dff0d8; }\n";
    report << "    .test-failed { background-color: #f2dede; }\n";
    report << "    .test-name { font-weight: bold; }\n";
    report << "    .test-message { margin-top: 5px; }\n";
    report << "    .paradoxes { margin-top: 10px; }\n";
    report << "    .paradox { margin-left: 20px; margin-bottom: 5px; }\n";
    report << "    .benchmarks { margin-top: 10px; }\n";
    report << "    .benchmark { margin-left: 20px; margin-bottom: 5px; }\n";
    report << "  </style>\n";
    report << "</head>\n";
    report << "<body>\n";

    report << "  <h1>Chronovyan Temporal Testing Report</h1>\n";

    // Summary
    int passCount = 0;
    for (const auto &result : results) {
      if (result.isPassed()) {
        ++passCount;
      }
    }

    report << "  <div class=\"summary\">\n";
    report << "    <h2>Summary</h2>\n";
    report << "    <p>Total tests: " << results.size() << "</p>\n";
    report << "    <p>Passed: " << passCount << "</p>\n";
    report << "    <p>Failed: " << (results.size() - passCount) << "</p>\n";
    report << "    <p>Pass percentage: "
           << (results.empty() ? 0 : (passCount * 100.0 / results.size()))
           << "%</p>\n";
    report << "  </div>\n";

    // Tests
    report << "  <h2>Test Results</h2>\n";

    for (const auto &result : results) {
      report << "  <div class=\"test "
             << (result.isPassed() ? "test-passed" : "test-failed") << "\">\n";
      report << "    <div class=\"test-name\">" << result.getTestName()
             << "</div>\n";
      report << "    <div class=\"test-message\">" << result.getMessage()
             << "</div>\n";

      // Paradoxes
      const auto &paradoxes = result.getParadoxes();
      if (!paradoxes.empty()) {
        report << "    <div class=\"paradoxes\">\n";
        report << "      <h3>Paradoxes</h3>\n";
        for (const auto &paradox : paradoxes) {
          report << "      <div class=\"paradox\">\n";
          report << "        <div>Description: " << paradox.description
                 << "</div>\n";
          report << "        <div>Severity: " << paradox.severity << "</div>\n";
          report << "        <div>Location: " << paradox.location.file << ":"
                 << paradox.location.line << "</div>\n";
          report << "      </div>\n";
        }
        report << "    </div>\n";
      }

      // Benchmark results
      const auto &benchmarks = result.getBenchmarkResults();
      if (!benchmarks.empty()) {
        report << "    <div class=\"benchmarks\">\n";
        report << "      <h3>Benchmarks</h3>\n";
        for (const auto &[opName, benchResult] : benchmarks) {
          report << "      <div class=\"benchmark\">\n";
          report << "        <div>" << opName << "</div>\n";
          report << "        <div>Duration: " << benchResult.duration
                 << " ms</div>\n";
          report << "        <div>Resources:</div>\n";
          report << "        <ul>\n";
          for (const auto &[resourceName, usage] : benchResult.resourceUsage) {
            report << "          <li>" << resourceName << ": " << usage
                   << "</li>\n";
          }
          report << "        </ul>\n";
          report << "      </div>\n";
        }
        report << "    </div>\n";
      }

      report << "  </div>\n";
    }

    report << "</body>\n";
    report << "</html>\n";
  } else {
    // Default to text format
    report << "Chronovyan Temporal Testing Report\n";
    report << "=================================\n\n";

    // Summary
    int passCount = 0;
    for (const auto &result : results) {
      if (result.isPassed()) {
        ++passCount;
      }
    }

    report << "Summary:\n";
    report << "  Total tests: " << results.size() << "\n";
    report << "  Passed: " << passCount << "\n";
    report << "  Failed: " << (results.size() - passCount) << "\n";
    report << "  Pass percentage: "
           << (results.empty() ? 0 : (passCount * 100.0 / results.size()))
           << "%\n\n";

    // Tests
    report << "Test Results:\n";

    for (const auto &result : results) {
      report << "  " << result.getTestName() << ": "
             << (result.isPassed() ? "PASSED" : "FAILED") << "\n";
      if (!result.getMessage().empty()) {
        report << "    Message: " << result.getMessage() << "\n";
      }

      // Paradoxes
      const auto &paradoxes = result.getParadoxes();
      if (!paradoxes.empty()) {
        report << "    Paradoxes:\n";
        for (const auto &paradox : paradoxes) {
          report << "      Description: " << paradox.description << "\n";
          report << "      Severity: " << paradox.severity << "\n";
          report << "      Location: " << paradox.location.file << ":"
                 << paradox.location.line << "\n";
        }
      }

      // Benchmark results
      const auto &benchmarks = result.getBenchmarkResults();
      if (!benchmarks.empty()) {
        report << "    Benchmarks:\n";
        for (const auto &[opName, benchResult] : benchmarks) {
          report << "      " << opName << ":\n";
          report << "        Duration: " << benchResult.duration << " ms\n";
          report << "        Resources:\n";
          for (const auto &[resourceName, usage] : benchResult.resourceUsage) {
            report << "          " << resourceName << ": " << usage << "\n";
          }
        }
      }

      report << "\n";
    }
  }

  return report.str();
}

bool TemporalTestFramework::saveReport(const std::string &report,
                                       const std::string &filePath) const {
  std::ofstream file(filePath);
  if (!file) {
    return false;
  }

  file << report;
  return !file.bad();
}

std::shared_ptr<TemporalTestCase>
TemporalTestFramework::createTestCaseFromBytecode(
    const bytecode::BytecodeModule &module, const std::string &testCaseName) {
  return std::make_shared<TemporalTestCase>(testCaseName, module);
}

std::shared_ptr<TemporalTestCase>
TemporalTestFramework::createTestCaseFromSource(
    const std::string &sourceFilePath, const std::string &testCaseName) {
  // In a real implementation, we would compile the source file to bytecode
  // For now, just create an empty bytecode module
  bytecode::BytecodeModule module;

  // TODO: Implement source file compilation
  diagnostics.addWarning(SourceLocation(),
                         "Source file compilation not implemented yet. Using "
                         "empty bytecode module.");

  return std::make_shared<TemporalTestCase>(testCaseName, module);
}

TemporalTestResult TemporalTestFramework::runTestCase(
    const std::shared_ptr<TemporalTestCase> &testCase) {
  TemporalTestResult result(testCase->getName(), true);

  // Initialize the debugger with the bytecode module
  if (!debugger->initialize(testCase->getModule())) {
    result = TemporalTestResult(testCase->getName(), false,
                                "Failed to initialize debugger");
    return result;
  }

  // Start the debugger
  if (!debugger->start()) {
    result = TemporalTestResult(testCase->getName(), false,
                                "Failed to start debugger");
    return result;
  }

  // Start benchmarking if enabled
  std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
  if (testCase->isBenchmarkingEnabled()) {
    startTime = std::chrono::high_resolution_clock::now();
    benchmark->startMeasuring();
  }

  // Run the program until completion or paradox detection
  bool completed = true;
  while (completed) {
    // Check for paradoxes
    const auto &paradoxes = paradoxDetector->detectParadoxes(*debugger);
    if (!paradoxes.empty()) {
      for (const auto &paradox : paradoxes) {
        result.addParadox(paradox.description, paradox.severity,
                          paradox.location);

        // If the paradox severity exceeds the maximum allowed, fail the test
        if (paradox.severity > testCase->getMaxParadoxSeverity()) {
          result = TemporalTestResult(
              testCase->getName(), false,
              "Paradox detected with severity " +
                  std::to_string(paradox.severity) + " (max allowed: " +
                  std::to_string(testCase->getMaxParadoxSeverity()) + ")");
          completed = false;
          break;
        }
      }
    }

    // Break the loop if a paradox was detected
    if (!completed) {
      break;
    }

    // Step the debugger
    if (!debugger->stepInto()) {
      completed = false;
      break;
    }
  }

  // Stop benchmarking if enabled
  if (testCase->isBenchmarkingEnabled()) {
    benchmark->stopMeasuring();
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate duration in milliseconds
    double duration =
        std::chrono::duration<double, std::milli>(endTime - startTime).count();

    // Add benchmark results to the test result
    result.addBenchmarkResult("execution", duration,
                              debugger->getCurrentResourceUsage());
  }

  // Check assertions
  for (const auto &assertion : testCase->getAssertions()) {
    if (!assertion->check(*debugger)) {
      result = TemporalTestResult(
          testCase->getName(), false,
          "Assertion failed: " + assertion->getDescription() + " - " +
              assertion->getFailureMessage());
      break;
    }
  }

  // Stop the debugger
  debugger->stop();

  return result;
}

// TemporalTestResult implementation
TemporalTestResult::TemporalTestResult(const std::string &name, bool testPassed,
                                       const std::string &testMessage)
    : testName(name), passed(testPassed), message(testMessage) {}

std::string TemporalTestResult::getTestName() const { return testName; }

bool TemporalTestResult::isPassed() const { return passed; }

std::string TemporalTestResult::getMessage() const { return message; }

void TemporalTestResult::addParadox(const std::string &paradoxDescription,
                                    int severity,
                                    const SourceLocation &sourceLocation) {
  paradoxes.push_back({paradoxDescription, severity, sourceLocation});
}

std::vector<TemporalTestResult::Paradox>
TemporalTestResult::getParadoxes() const {
  return paradoxes;
}

void TemporalTestResult::addBenchmarkResult(
    const std::string &operationName, double duration,
    const std::map<std::string, double> &resourceUsage) {
  benchmarkResults[operationName] = {duration, resourceUsage};
}

std::map<std::string, TemporalTestResult::BenchmarkResult>
TemporalTestResult::getBenchmarkResults() const {
  return benchmarkResults;
}

// TemporalTestCase implementation
TemporalTestCase::TemporalTestCase(
    const std::string &name, const bytecode::BytecodeModule &bytecodeModule)
    : name(name), module(bytecodeModule), maxParadoxSeverity(5),
      benchmarkingEnabled(false) {}

std::string TemporalTestCase::getName() const { return name; }

const bytecode::BytecodeModule &TemporalTestCase::getModule() const {
  return module;
}

void TemporalTestCase::addAssertion(
    std::shared_ptr<TemporalAssertion> assertion) {
  assertions.push_back(assertion);
}

const std::vector<std::shared_ptr<TemporalAssertion>> &
TemporalTestCase::getAssertions() const {
  return assertions;
}

void TemporalTestCase::setMaxParadoxSeverity(int severity) {
  maxParadoxSeverity = severity;
}

int TemporalTestCase::getMaxParadoxSeverity() const {
  return maxParadoxSeverity;
}

void TemporalTestCase::enableBenchmarking(bool enable) {
  benchmarkingEnabled = enable;
}

bool TemporalTestCase::isBenchmarkingEnabled() const {
  return benchmarkingEnabled;
}

// TemporalTestSuite implementation
TemporalTestSuite::TemporalTestSuite(const std::string &name) : name(name) {}

std::string TemporalTestSuite::getName() const { return name; }

void TemporalTestSuite::addTestCase(
    std::shared_ptr<TemporalTestCase> testCase) {
  testCases.push_back(testCase);
}

const std::vector<std::shared_ptr<TemporalTestCase>> &
TemporalTestSuite::getTestCases() const {
  return testCases;
}

// TemporalAssertion implementation
TemporalAssertion::TemporalAssertion(const std::string &desc)
    : description(desc) {}

std::string TemporalAssertion::getDescription() const { return description; }

std::string TemporalAssertion::getFailureMessage() const {
  return failureMessage;
}

// StateAssertion implementation
StateAssertion::StateAssertion(const std::string &description,
                               const std::string &varName,
                               const std::string &expectedVal)
    : TemporalAssertion(description), variableName(varName),
      expectedValue(expectedVal) {}

bool StateAssertion::check(debugger::DejaVuDebugger &debugger) {
  std::string actualValue = debugger.getVariableValue(variableName);

  if (actualValue != expectedValue) {
    failureMessage = "Expected " + variableName + " to be '" + expectedValue +
                     "', but got '" + actualValue + "'";
    return false;
  }

  return true;
}

// ResourceAssertion implementation
ResourceAssertion::ResourceAssertion(const std::string &description,
                                     const std::string &resName,
                                     double expectedUsage, double tolerance)
    : TemporalAssertion(description), resourceName(resName),
      expectedUsage(expectedUsage), tolerance(tolerance) {}

bool ResourceAssertion::check(debugger::DejaVuDebugger &debugger) {
  const auto &resourceUsage = debugger.getCurrentResourceUsage();

  auto it = resourceUsage.find(resourceName);
  if (it == resourceUsage.end()) {
    failureMessage = "Resource '" + resourceName + "' not found";
    return false;
  }

  double actualUsage = it->second;
  double lowerBound = expectedUsage - tolerance;
  double upperBound = expectedUsage + tolerance;

  if (actualUsage < lowerBound || actualUsage > upperBound) {
    std::ostringstream oss;
    oss << "Expected " << resourceName << " usage to be " << expectedUsage;
    if (tolerance > 0) {
      oss << " (±" << tolerance << ")";
    }
    oss << ", but got " << actualUsage;

    failureMessage = oss.str();
    return false;
  }

  return true;
}

// TimelineAssertion implementation
TimelineAssertion::TimelineAssertion(const std::string &description,
                                     int expectedCount)
    : TemporalAssertion(description), expectedTimelineCount(expectedCount) {}

bool TimelineAssertion::check(debugger::DejaVuDebugger &debugger) {
  // In a real implementation, we would get the timeline branches from the
  // debugger For now, just check if the current timeline ID is within the
  // expected range
  int currentTimelineId = debugger.getCurrentTimelineId();

  if (currentTimelineId >= expectedTimelineCount) {
    failureMessage = "Expected timeline count to be " +
                     std::to_string(expectedTimelineCount) +
                     ", but timeline ID " + std::to_string(currentTimelineId) +
                     " was detected";
    return false;
  }

  return true;
}

// CustomAssertion implementation
CustomAssertion::CustomAssertion(
    const std::string &description,
    std::function<bool(debugger::DejaVuDebugger &, std::string &)> checkFunc)
    : TemporalAssertion(description), checkFunction(checkFunc) {}

bool CustomAssertion::check(debugger::DejaVuDebugger &debugger) {
  return checkFunction(debugger, failureMessage);
}

} // namespace proving_grounds
} // namespace compiler
} // namespace chronovyan