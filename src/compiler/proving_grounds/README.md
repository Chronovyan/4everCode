---
title: Readme
description: Documentation for README
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Temporal Proving Grounds

## Overview

The Temporal Proving Grounds is a comprehensive testing, validation, and benchmarking framework for Chronovyan programs. It provides specialized tools for ensuring temporal correctness, detecting paradoxes, and measuring performance of temporal operations.

As temporal programming introduces unique challenges not found in traditional programming paradigms, the Proving Grounds offers tailored solutions for verifying that programs correctly handle timeline branching, resource consumption, and causality without introducing paradoxes or inefficiencies.

## Key Components

### TemporalTestFramework

The main entry point for creating, managing, and running temporal tests. This class provides:

- Creation and management of test cases and test suites
- Running individual tests or entire suites
- Generating comprehensive test reports in multiple formats
- Integration with the Chronovyan compiler and debugger

```cpp
// Example: Creating and running a simple test
DiagnosticReporter diagnostics;
auto testFramework = std::make_unique<TemporalTestFramework>(diagnostics);

// Create a test case
auto testCase = testFramework->createTestCaseFromBytecode(bytecodeModule, "MyTest");
testCase->enableBenchmarking(true);

// Add assertions
testCase->addAssertion(std::make_shared<StateAssertion>("Variable check", "myVar", "expectedValue"));

// Register and run the test
testFramework->registerTestCase(testCase);
auto results = testFramework->runAllTests();

// Generate a report
std::string report = testFramework->generateReport(results, "html");
testFramework->saveReport(report, "report.html");
```

### ParadoxDetector

Identifies and reports temporal paradoxes in Chronovyan programs. The detector supports multiple types of paradoxes:

- Resource violations (excessive chronon usage, aethel depletion)
- Timeline inconsistencies (excessive branching, unresolved timelines)
- Causality violations (actions affecting their own causes)
- Variable state inconsistencies across timelines
- Thread synchronization paradoxes in parallel execution

The detector is configurable with customizable rule sets and severity thresholds:

```cpp
// Example: Configuring the paradox detector
auto paradoxDetector = testFramework->getParadoxDetector();

// Enable or disable specific rules
paradoxDetector->setRuleEnabled("ResourceViolation", true);
paradoxDetector->setRuleEnabled("TimelineInconsistency", false);

// Set severity threshold (paradoxes below this severity are ignored)
paradoxDetector->setSeverityThreshold(7); // Range: 1-10
```

### TemporalBenchmark

Measures and analyzes the performance of temporal operations:

- Duration tracking for operations
- Resource usage monitoring (chronons, aethel, temporal debt)
- Efficiency calculations based on resource utilization
- Performance reporting with visualizations

```cpp
// Example: Using the benchmarking system
auto benchmark = testFramework->getTemporalBenchmark();

// Start measuring
benchmark->startMeasuring();

// Mark the start of an operation
benchmark->markOperationStart("myOperation");

// ... operation code ...

// Mark the end of the operation
benchmark->markOperationEnd("myOperation");

// Record resource usage
benchmark->recordResourceUsage("chronons", 150.0);

// Generate a report
std::string report = benchmark->generateReport("json");
```

## Assertion Types

The framework provides various assertion types for validating temporal behavior:

### StateAssertion

Verifies the state of variables at the end of execution:

```cpp
auto assertion = std::make_shared<StateAssertion>(
    "Counter should be 10 after execution",
    "counter",
    "10"
);
```

### ResourceAssertion

Checks resource usage against expected values:

```cpp
auto assertion = std::make_shared<ResourceAssertion>(
    "Chronon usage should be less than 500",
    "chronons",
    500.0,
    ResourceAssertion::ComparisonType::LESS_THAN
);
```

### TimelineAssertion

Validates timeline branching and merging behavior:

```cpp
auto assertion = std::make_shared<TimelineAssertion>(
    "Program should branch exactly 3 times",
    3
);
```

### CustomAssertion

Create specialized assertions by extending the base Assertion class:

```cpp
class MyCustomAssertion : public Assertion {
public:
    // ... implementation ...
    
    bool evaluate(const debugger::DejaVuDebugger& debugger) const override {
        // Custom evaluation logic
        return true;
    }
};
```

## Integration with the Compiler Pipeline

The Temporal Proving Grounds is designed to integrate with the Chronovyan compiler:

- Can be used in standalone mode for testing specific programs
- Can be integrated into the compilation process for automatic validation
- Works with both source code and bytecode for flexibility
- Shares diagnostics with the compiler for unified error reporting

## Examples

See the `examples/proving_grounds` directory for complete examples showing how to use the Temporal Proving Grounds for various testing scenarios.

## Running Tests

To run the tests for the Temporal Proving Grounds itself:

```bash
cd build
cmake --build . --target proving_grounds_test
./bin/proving_grounds_test
``` 