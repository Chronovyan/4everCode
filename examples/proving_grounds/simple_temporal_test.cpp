#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "bytecode/BytecodeModule.h"
#include "compiler/DiagnosticReporter.h"
#include "compiler/proving_grounds/TemporalTestFramework.h"

using namespace chronovyan;
using namespace chronovyan::compiler;
using namespace chronovyan::compiler::proving_grounds;

// Helper function to create a simple bytecode module for testing
bytecode::BytecodeModule createTestModule() {
    bytecode::BytecodeModule module;

    // Create a main function
    bytecode::Function mainFunc("main");

    // Add instructions to simulate a temporal operation with resource usage

    // Start resource usage block with 200 chronons
    bytecode::Instruction resourceBegin(bytecode::OpCode::RESOURCE_BEGIN);
    resourceBegin.addOperand(bytecode::Operand::createFloat(200.0));
    mainFunc.addInstruction(resourceBegin);

    // Store variable "counter" with initial value 0
    bytecode::Instruction storeVar(bytecode::OpCode::STORE_VAR);
    storeVar.addOperand(bytecode::Operand::createString("counter"));
    storeVar.addOperand(bytecode::Operand::createInteger(0));
    mainFunc.addInstruction(storeVar);

    // Start a loop that runs 10 times
    bytecode::Instruction loopBegin(bytecode::OpCode::LOOP_BEGIN);
    loopBegin.addOperand(bytecode::Operand::createInteger(10));
    mainFunc.addInstruction(loopBegin);

    // Create a timeline branch with 2 branches
    bytecode::Instruction timelineBegin(bytecode::OpCode::TIMELINE_BRANCH_BEGIN);
    timelineBegin.addOperand(bytecode::Operand::createInteger(2));
    mainFunc.addInstruction(timelineBegin);

    // Increment the counter in one timeline
    bytecode::Instruction loadVar(bytecode::OpCode::LOAD_VAR);
    loadVar.addOperand(bytecode::Operand::createString("counter"));
    mainFunc.addInstruction(loadVar);

    bytecode::Instruction increment(bytecode::OpCode::ADD);
    increment.addOperand(bytecode::Operand::createInteger(1));
    mainFunc.addInstruction(increment);

    bytecode::Instruction updateVar(bytecode::OpCode::STORE_VAR);
    updateVar.addOperand(bytecode::Operand::createString("counter"));
    mainFunc.addInstruction(updateVar);

    // End the timeline branch
    mainFunc.addInstruction(bytecode::Instruction(bytecode::OpCode::TIMELINE_BRANCH_END));

    // End the loop
    mainFunc.addInstruction(bytecode::Instruction(bytecode::OpCode::LOOP_END));

    // End resource usage
    mainFunc.addInstruction(bytecode::Instruction(bytecode::OpCode::RESOURCE_END));

    // Add a return instruction
    mainFunc.addInstruction(bytecode::Instruction(bytecode::OpCode::RETURN));

    // Add debug information to instructions
    for (size_t i = 0; i < mainFunc.getInstructions().size(); i++) {
        mainFunc.getInstructions()[i].setDebugInfo({"test_program.cvy", static_cast<int>(i + 1)});
    }

    // Add the function to the module
    module.addFunction(mainFunc);

    return module;
}

// Example of a custom assertion for timeline branching
class TimelineBranchAssertion : public StateAssertion {
public:
    TimelineBranchAssertion(const std::string& description, int expectedBranches)
        : StateAssertion(description, "timeline_branches", std::to_string(expectedBranches)) {}

    bool evaluate(const debugger::DejaVuDebugger& debugger) const override {
        // Check if the number of timeline branches matches the expected value
        int expectedBranches = std::stoi(getExpectedValue());
        int actualBranches = debugger.getTimelineBranchCount();
        return actualBranches == expectedBranches;
    }

    std::string getFailureMessage(const debugger::DejaVuDebugger& debugger) const override {
        int expectedBranches = std::stoi(getExpectedValue());
        int actualBranches = debugger.getTimelineBranchCount();
        return "Expected " + std::to_string(expectedBranches) + " timeline branches, but found " +
               std::to_string(actualBranches);
    }
};

int main() {
    // Create a diagnostic reporter for error reporting
    DiagnosticReporter diagnostics;

    // Create the temporal test framework
    auto testFramework = std::make_unique<TemporalTestFramework>(diagnostics);

    // Create a test module with our sample bytecode
    bytecode::BytecodeModule testModule = createTestModule();

    // Create a test case from the bytecode
    auto testCase = testFramework->createTestCaseFromBytecode(testModule, "SimpleTemporalTest");

    // Configure the test case
    testCase->setDescription("A simple test for temporal operations and resource usage");
    testCase->setMaxParadoxSeverity(5);  // Allow paradoxes with severity up to 5
    testCase->enableBenchmarking(true);  // Enable performance benchmarking

    // Add assertions to validate the test

    // 1. Resource assertion - check that chronon usage is within expected range
    auto resourceAssertion =
        std::make_shared<ResourceAssertion>("Chronon usage should be less than 300", "chronons",
                                            300.0, ResourceAssertion::ComparisonType::LESS_THAN);
    testCase->addAssertion(resourceAssertion);

    // 2. State assertion - check the final value of the counter variable
    auto stateAssertion = std::make_shared<StateAssertion>(
        "Counter should be 10 after loop execution", "counter", "10");
    testCase->addAssertion(stateAssertion);

    // 3. Custom timeline assertion - check that we have the expected number of timeline branches
    auto timelineAssertion = std::make_shared<TimelineBranchAssertion>(
        "Program should create exactly 20 timeline branches (2 per loop iteration × 10 iterations)",
        20);
    testCase->addAssertion(timelineAssertion);

    // Register the test case with the framework
    testFramework->registerTestCase(testCase);

    // Run all tests
    std::cout << "Running temporal tests..." << std::endl;
    auto results = testFramework->runAllTests();

    // Generate and print a report
    std::string report = testFramework->generateReport(results, "text");
    std::cout << "\nTest Results:\n" << report << std::endl;

    // Save the report to a file
    std::string reportPath = "temporal_test_report.html";
    std::string htmlReport = testFramework->generateReport(results, "html");
    if (testFramework->saveReport(htmlReport, reportPath)) {
        std::cout << "Detailed HTML report saved to: " << reportPath << std::endl;
    }

    // Return success if all tests passed, failure otherwise
    bool allPassed = true;
    for (const auto& result : results) {
        if (!result.isPassed()) {
            allPassed = false;
            break;
        }
    }

    if (allPassed) {
        std::cout << "All tests passed successfully!" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests failed. See the report for details." << std::endl;
        return 1;
    }
}