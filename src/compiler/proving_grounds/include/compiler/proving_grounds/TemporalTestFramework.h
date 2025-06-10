#ifndef CHRONOVYAN_COMPILER_PROVING_GROUNDS_TEMPORAL_TEST_FRAMEWORK_H
#define CHRONOVYAN_COMPILER_PROVING_GROUNDS_TEMPORAL_TEST_FRAMEWORK_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "compiler/DiagnosticReporter.h"
#include "compiler/bytecode/BytecodeModule.h"
#include "compiler/debugger/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {
namespace proving_grounds {

// Forward declarations
class ParadoxDetector;
class TemporalBenchmark;
class TemporalAssertion;
class TemporalTestCase;
class TemporalTestSuite;
class TemporalTestResult;

/**
 * @class TemporalTestFramework
 * @brief Main framework class for testing temporal correctness in Chronovyan
 * programs.
 *
 * The Temporal Proving Grounds provides a comprehensive testing environment for
 * temporal operations, including correctness testing, paradox detection, and
 * benchmarking.
 */
class TemporalTestFramework {
public:
  /**
   * @brief Constructor
   * @param diagnostics Diagnostic reporter for reporting errors
   */
  explicit TemporalTestFramework(DiagnosticReporter &diagnostics);

  /**
   * @brief Destructor
   */
  ~TemporalTestFramework();

  /**
   * @brief Register a test case to be run by the framework
   * @param testCase The test case to register
   */
  void registerTestCase(std::shared_ptr<TemporalTestCase> testCase);

  /**
   * @brief Register a test suite to be run by the framework
   * @param testSuite The test suite to register
   */
  void registerTestSuite(std::shared_ptr<TemporalTestSuite> testSuite);

  /**
   * @brief Run all registered test cases and suites
   * @return A vector of test results
   */
  std::vector<TemporalTestResult> runAllTests();

  /**
   * @brief Run a specific test case
   * @param testCaseName The name of the test case to run
   * @return The test result
   */
  TemporalTestResult runTest(const std::string &testCaseName);

  /**
   * @brief Run a specific test suite
   * @param testSuiteName The name of the test suite to run
   * @return A vector of test results
   */
  std::vector<TemporalTestResult>
  runTestSuite(const std::string &testSuiteName);

  /**
   * @brief Get the paradox detector used by the framework
   * @return The paradox detector
   */
  std::shared_ptr<ParadoxDetector> getParadoxDetector() const;

  /**
   * @brief Get the temporal benchmark used by the framework
   * @return The temporal benchmark
   */
  std::shared_ptr<TemporalBenchmark> getTemporalBenchmark() const;

  /**
   * @brief Generate a report of test results
   * @param results The test results to include in the report
   * @param format The format of the report (e.g., "text", "html", "json")
   * @return The report as a string
   */
  std::string generateReport(const std::vector<TemporalTestResult> &results,
                             const std::string &format = "text") const;

  /**
   * @brief Save a report to a file
   * @param report The report to save
   * @param filePath The path to save the report to
   * @return True if the report was saved successfully, false otherwise
   */
  bool saveReport(const std::string &report, const std::string &filePath) const;

  /**
   * @brief Create a test case from a bytecode module
   * @param module The bytecode module to create a test case for
   * @param testCaseName The name of the test case
   * @return A test case
   */
  std::shared_ptr<TemporalTestCase>
  createTestCaseFromBytecode(const bytecode::BytecodeModule &module,
                             const std::string &testCaseName);

  /**
   * @brief Create a test case from a source file
   * @param sourceFilePath The path to the source file
   * @param testCaseName The name of the test case
   * @return A test case
   */
  std::shared_ptr<TemporalTestCase>
  createTestCaseFromSource(const std::string &sourceFilePath,
                           const std::string &testCaseName);

private:
  DiagnosticReporter &diagnostics;
  std::vector<std::shared_ptr<TemporalTestCase>> testCases;
  std::vector<std::shared_ptr<TemporalTestSuite>> testSuites;
  std::shared_ptr<ParadoxDetector> paradoxDetector;
  std::shared_ptr<TemporalBenchmark> benchmark;
  std::shared_ptr<debugger::DejaVuDebugger> debugger;

  /**
   * @brief Run a single test case
   * @param testCase The test case to run
   * @return The test result
   */
  TemporalTestResult
  runTestCase(const std::shared_ptr<TemporalTestCase> &testCase);
};

/**
 * @class TemporalTestResult
 * @brief Represents the result of a temporal test case.
 */
class TemporalTestResult {
public:
  /**
   * @brief Constructor
   * @param testName The name of the test
   * @param passed Whether the test passed or failed
   * @param message A message describing the test result
   */
  TemporalTestResult(const std::string &testName, bool passed,
                     const std::string &message = "");

  /**
   * @brief Get the name of the test
   * @return The name of the test
   */
  std::string getTestName() const;

  /**
   * @brief Check if the test passed
   * @return True if the test passed, false otherwise
   */
  bool isPassed() const;

  /**
   * @brief Get the message describing the test result
   * @return The message describing the test result
   */
  std::string getMessage() const;

  /**
   * @brief Add a paradox to the test result
   * @param paradoxDescription A description of the paradox
   * @param severity The severity of the paradox
   * @param sourceLocation The source location where the paradox occurred
   */
  void addParadox(const std::string &paradoxDescription, int severity,
                  const SourceLocation &sourceLocation);

  /**
   * @brief Get the paradoxes detected during the test
   * @return A vector of paradoxes detected during the test
   */
  struct Paradox {
    std::string description;
    int severity;
    SourceLocation location;
  };

  std::vector<Paradox> getParadoxes() const;

  /**
   * @brief Add a benchmark result to the test result
   * @param operationName The name of the operation
   * @param duration The duration of the operation in milliseconds
   * @param resourceUsage A map of resource names to their usage values
   */
  void addBenchmarkResult(const std::string &operationName, double duration,
                          const std::map<std::string, double> &resourceUsage);

  /**
   * @brief Get the benchmark results
   * @return A map of operation names to their benchmark results
   */
  struct BenchmarkResult {
    double duration;
    std::map<std::string, double> resourceUsage;
  };

  std::map<std::string, BenchmarkResult> getBenchmarkResults() const;

private:
  std::string testName;
  bool passed;
  std::string message;
  std::vector<Paradox> paradoxes;
  std::map<std::string, BenchmarkResult> benchmarkResults;
};

/**
 * @class TemporalTestCase
 * @brief Represents a test case for temporal correctness.
 */
class TemporalTestCase {
public:
  /**
   * @brief Constructor
   * @param name The name of the test case
   * @param module The bytecode module to test
   */
  TemporalTestCase(const std::string &name,
                   const bytecode::BytecodeModule &module);

  /**
   * @brief Get the name of the test case
   * @return The name of the test case
   */
  std::string getName() const;

  /**
   * @brief Get the bytecode module being tested
   * @return The bytecode module being tested
   */
  const bytecode::BytecodeModule &getModule() const;

  /**
   * @brief Add an assertion to the test case
   * @param assertion The assertion to add
   */
  void addAssertion(std::shared_ptr<TemporalAssertion> assertion);

  /**
   * @brief Get the assertions in the test case
   * @return The assertions in the test case
   */
  const std::vector<std::shared_ptr<TemporalAssertion>> &getAssertions() const;

  /**
   * @brief Set the maximum allowed paradox severity
   * @param severity The maximum allowed paradox severity
   */
  void setMaxParadoxSeverity(int severity);

  /**
   * @brief Get the maximum allowed paradox severity
   * @return The maximum allowed paradox severity
   */
  int getMaxParadoxSeverity() const;

  /**
   * @brief Enable or disable benchmarking for this test case
   * @param enable Whether to enable benchmarking
   */
  void enableBenchmarking(bool enable);

  /**
   * @brief Check if benchmarking is enabled for this test case
   * @return True if benchmarking is enabled, false otherwise
   */
  bool isBenchmarkingEnabled() const;

private:
  std::string name;
  bytecode::BytecodeModule module;
  std::vector<std::shared_ptr<TemporalAssertion>> assertions;
  int maxParadoxSeverity;
  bool benchmarkingEnabled;
};

/**
 * @class TemporalTestSuite
 * @brief Represents a suite of test cases for temporal correctness.
 */
class TemporalTestSuite {
public:
  /**
   * @brief Constructor
   * @param name The name of the test suite
   */
  explicit TemporalTestSuite(const std::string &name);

  /**
   * @brief Get the name of the test suite
   * @return The name of the test suite
   */
  std::string getName() const;

  /**
   * @brief Add a test case to the suite
   * @param testCase The test case to add
   */
  void addTestCase(std::shared_ptr<TemporalTestCase> testCase);

  /**
   * @brief Get the test cases in the suite
   * @return The test cases in the suite
   */
  const std::vector<std::shared_ptr<TemporalTestCase>> &getTestCases() const;

private:
  std::string name;
  std::vector<std::shared_ptr<TemporalTestCase>> testCases;
};

/**
 * @class TemporalAssertion
 * @brief Represents an assertion for temporal correctness.
 */
class TemporalAssertion {
public:
  /**
   * @brief Constructor
   * @param description A description of the assertion
   */
  explicit TemporalAssertion(const std::string &description);

  /**
   * @brief Get the description of the assertion
   * @return The description of the assertion
   */
  std::string getDescription() const;

  /**
   * @brief Check if the assertion passes
   * @param debugger The debugger to use for checking the assertion
   * @return True if the assertion passes, false otherwise
   */
  virtual bool check(debugger::DejaVuDebugger &debugger) = 0;

  /**
   * @brief Get a message describing why the assertion failed
   * @return A message describing why the assertion failed
   */
  std::string getFailureMessage() const;

protected:
  std::string description;
  std::string failureMessage;
};

/**
 * @class StateAssertion
 * @brief Represents an assertion about the state of a program.
 */
class StateAssertion : public TemporalAssertion {
public:
  /**
   * @brief Constructor
   * @param description A description of the assertion
   * @param variableName The name of the variable to check
   * @param expectedValue The expected value of the variable
   */
  StateAssertion(const std::string &description,
                 const std::string &variableName,
                 const std::string &expectedValue);

  /**
   * @brief Check if the assertion passes
   * @param debugger The debugger to use for checking the assertion
   * @return True if the assertion passes, false otherwise
   */
  bool check(debugger::DejaVuDebugger &debugger) override;

private:
  std::string variableName;
  std::string expectedValue;
};

/**
 * @class ResourceAssertion
 * @brief Represents an assertion about resource usage.
 */
class ResourceAssertion : public TemporalAssertion {
public:
  /**
   * @brief Constructor
   * @param description A description of the assertion
   * @param resourceName The name of the resource to check
   * @param expectedUsage The expected usage of the resource
   * @param tolerance The tolerance for the resource usage
   */
  ResourceAssertion(const std::string &description,
                    const std::string &resourceName, double expectedUsage,
                    double tolerance = 0.0);

  /**
   * @brief Check if the assertion passes
   * @param debugger The debugger to use for checking the assertion
   * @return True if the assertion passes, false otherwise
   */
  bool check(debugger::DejaVuDebugger &debugger) override;

private:
  std::string resourceName;
  double expectedUsage;
  double tolerance;
};

/**
 * @class TimelineAssertion
 * @brief Represents an assertion about timeline branches.
 */
class TimelineAssertion : public TemporalAssertion {
public:
  /**
   * @brief Constructor
   * @param description A description of the assertion
   * @param expectedTimelineCount The expected number of timeline branches
   */
  TimelineAssertion(const std::string &description, int expectedTimelineCount);

  /**
   * @brief Check if the assertion passes
   * @param debugger The debugger to use for checking the assertion
   * @return True if the assertion passes, false otherwise
   */
  bool check(debugger::DejaVuDebugger &debugger) override;

private:
  int expectedTimelineCount;
};

/**
 * @class CustomAssertion
 * @brief Represents a custom assertion with a user-defined check function.
 */
class CustomAssertion : public TemporalAssertion {
public:
  /**
   * @brief Constructor
   * @param description A description of the assertion
   * @param checkFunction The function to use for checking the assertion
   */
  CustomAssertion(const std::string &description,
                  std::function<bool(debugger::DejaVuDebugger &, std::string &)>
                      checkFunction);

  /**
   * @brief Check if the assertion passes
   * @param debugger The debugger to use for checking the assertion
   * @return True if the assertion passes, false otherwise
   */
  bool check(debugger::DejaVuDebugger &debugger) override;

private:
  std::function<bool(debugger::DejaVuDebugger &, std::string &)> checkFunction;
};

} // namespace proving_grounds
} // namespace compiler
} // namespace chronovyan

#endif // CHRONOVYAN_COMPILER_PROVING_GROUNDS_TEMPORAL_TEST_FRAMEWORK_H