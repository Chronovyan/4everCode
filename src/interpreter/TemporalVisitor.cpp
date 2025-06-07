#include <cmath>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#include "environment.h"
#include "error_handler.h"
#include "interpreter.h"
#include "interpreter/TemporalVisitor.h"
#include "resource_optimizer.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

// AST Node includes
#include "ast_nodes/BranchTimelineNode.h"
#include "ast_nodes/ParallelExecutionNode.h"
#include "ast_nodes/TemporalLoopNode.h"

namespace chronovyan {

TemporalVisitor::TemporalVisitor(Interpreter& interpreter) : m_interpreter(interpreter) {}

// Control flow state accessors
bool TemporalVisitor::isBreakChronon() const { return m_breakChronon_flag; }

bool TemporalVisitor::isContinueWeave() const { return m_continueWeave_flag; }

bool TemporalVisitor::isReverseFlow() const { return m_reverseFlow_flag; }

bool TemporalVisitor::isHaltRewind() const { return m_haltRewind_flag; }

bool TemporalVisitor::isPreventModification() const { return m_preventModification_flag; }

bool TemporalVisitor::isReadOnlyMode() const { return m_readOnlyMode; }

double TemporalVisitor::getStabilizationFactor() const { return m_stabilizationFactor; }

int TemporalVisitor::getFlowDirection() const { return m_flowDirection; }

// Control flow state modifiers
void TemporalVisitor::clearTemporalFlags() {
    m_breakChronon_flag = false;
    m_continueWeave_flag = false;
    m_reverseFlow_flag = false;
    m_haltRewind_flag = false;
    m_preventModification_flag = false;
    m_readOnlyMode = false;
    m_stabilizationFactor = 0.0;
    m_flowDirection = 1;
}

void TemporalVisitor::setBreakChronon(bool flag) { m_breakChronon_flag = flag; }

void TemporalVisitor::setContinueWeave(bool flag) { m_continueWeave_flag = flag; }

void TemporalVisitor::setReverseFlow(bool flag) {
    m_reverseFlow_flag = flag;
    if (flag) {
        m_flowDirection = -1;
    } else {
        m_flowDirection = 1;
    }
}

void TemporalVisitor::setHaltRewind(bool flag) { m_haltRewind_flag = flag; }

void TemporalVisitor::setPreventModification(bool flag) {
    m_preventModification_flag = flag;
    m_readOnlyMode = flag;
}

void TemporalVisitor::setReadOnlyMode(bool flag) { m_readOnlyMode = flag; }

void TemporalVisitor::setStabilizationFactor(double factor) { m_stabilizationFactor = factor; }

void TemporalVisitor::setFlowDirection(int direction) { m_flowDirection = direction; }

double TemporalVisitor::calculateTemporalResourceCost(TemporalOperation op) {
    // Base costs for different operations
    double baseCost = 0.0;

    switch (op) {
        case TemporalOperation::REWIND_FLOW:
            baseCost = 25.0;
            break;
        case TemporalOperation::STABILIZE:
            baseCost = 15.0;
            break;
        case TemporalOperation::PREVENT_MODIFICATION:
            baseCost = 10.0;
            break;
        case TemporalOperation::TEMPORAL_LOOP:
            baseCost = 30.0;
            break;
    }

    // Apply stabilization factor (if any)
    if (m_stabilizationFactor > 0.0) {
        // Stabilization reduces cost
        baseCost *= (1.0 - m_stabilizationFactor * 0.5);
    }

    // Get current paradox level from runtime
    int paradoxLevel = m_interpreter.getRuntime()->getParadoxLevel();

    // Paradox level increases cost exponentially
    double paradoxFactor = 1.0 + (paradoxLevel / 10.0);

    return baseCost * paradoxFactor;
}

void TemporalVisitor::consumeResources(double amount) {
    // Get the runtime
    auto runtime = m_interpreter.getRuntime();

    // Distribute the cost between aethel and chronons
    double aethelCost = amount * 0.6;   // 60% of cost
    double chrononCost = amount * 0.4;  // 40% of cost

    // Consume the resources
    runtime->consumeAethel(aethelCost);
    runtime->consumeChronons(chrononCost);

    // Track the resource usage
    m_interpreter.trackResourceUsage(aethelCost, chrononCost, "Temporal operation");
}

void TemporalVisitor::visitBreakChrononStmt(const BreakChrononStmtNode& stmt) {
    // Consume a small amount of resources
    m_interpreter.getRuntime()->consumeChronons(1.0);

    // Set the break flag
    setBreakChronon(true);
}

void TemporalVisitor::visitContinueWeaveStmt(const ContinueWeaveStmtNode& stmt) {
    // Consume a small amount of resources
    m_interpreter.getRuntime()->consumeChronons(1.0);

    // Set the continue flag
    setContinueWeave(true);
}

void TemporalVisitor::visitHaltRewindStmt(const HaltRewindStmtNode& stmt) {
    // Calculate and consume resources
    double cost = calculateTemporalResourceCost(TemporalOperation::REWIND_FLOW);
    consumeResources(cost);

    // Set the halt rewind flag
    setHaltRewind(true);
}

void TemporalVisitor::visitStabilizeTimelineStmt(const StabilizeTimelineStmtNode& stmt) {
    // Calculate base resource cost
    double cost = calculateTemporalResourceCost(TemporalOperation::STABILIZE);

    // Get the intensity if specified
    double intensity = 0.5;  // Default intensity
    if (stmt.hasIntensity()) {
        Value intensityValue = m_interpreter.evaluate(stmt.getIntensity());

        // Ensure the intensity is a number
        if (intensityValue.isNumber()) {
            intensity = intensityValue.asNumber();
        } else if (intensityValue.isInteger()) {
            intensity = static_cast<double>(intensityValue.asInteger());
        } else {
            throw RuntimeError("Stabilization intensity must be a number.", stmt.getLocation());
        }

        // Clamp intensity to valid range [0.0, 1.0]
        intensity = std::max(0.0, std::min(1.0, intensity));
    }

    // Adjust cost based on intensity
    cost *= (0.5 + intensity);

    // Consume resources
    consumeResources(cost);

    // Set the stabilization factor
    setStabilizationFactor(intensity);

    // Reduce paradox level based on intensity
    int reduction = static_cast<int>(intensity * 5.0);
    m_interpreter.getRuntime()->decreaseParadoxLevel(reduction);
}

void TemporalVisitor::visitPreventModificationStmt(const PreventModificationStmtNode& stmt) {
    // Calculate and consume resources
    double cost = calculateTemporalResourceCost(TemporalOperation::PREVENT_MODIFICATION);
    consumeResources(cost);

    // Set the prevent modification flag
    setPreventModification(true);
}

void TemporalVisitor::visitReverseFlowStmt(const ReverseFlowStmtNode& stmt) {
    // Calculate and consume resources
    double cost = calculateTemporalResourceCost(TemporalOperation::REWIND_FLOW);
    consumeResources(cost);

    // Set the reverse flow flag
    setReverseFlow(true);
}

void TemporalVisitor::visit(const TemporalLoopNode& node) {
    // Evaluate the duration and iterations
    Value durationVal = m_interpreter.evaluate(*node.getDuration());
    Value iterationsVal = m_interpreter.evaluate(*node.getIterations());

    // Validate types
    if (!durationVal.isNumber() || !iterationsVal.isNumber()) {
        throw RuntimeError("Temporal loop duration and iterations must be numbers");
    }

    double duration = durationVal.asNumber();
    int iterations = static_cast<int>(iterationsVal.asNumber());

    // Execute the loop
    for (int i = 0; i < iterations; ++i) {
        // Check for early exit conditions
        if (isBreakChronon() || isHaltRewind()) {
            break;
        }
        if (isContinueWeave()) {
            setContinueWeave(false);
            continue;
        }

        // Execute the loop body
        try {
            node.getBody()->accept(*this);
        } catch (const std::exception& e) {
            // Handle any exceptions that occur during loop execution
            std::cerr << "Error in temporal loop iteration " << i << ": " << e.what() << std::endl;
            throw;
        }

        // Simulate the passage of time
        consumeResources(duration);

        // Sleep to simulate the duration (in a real implementation, this would be more
        // sophisticated)
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(duration)));
    }
}

void TemporalVisitor::visit(const ParallelExecutionNode& node) {
    // Get the number of threads to use
    int threadCount = node.getThreadCount();

    // Create a vector to store the threads
    std::vector<std::thread> threads;

    // Create a mutex for thread-safe operations
    std::mutex mtx;

    // Create a vector to store any exceptions that occur in the threads
    std::vector<std::exception_ptr> exceptions;

    // Launch the threads
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&, i]() {
            try {
                // Create a new environment for this thread
                auto env = std::make_shared<Environment>(m_interpreter.getEnvironment());

                // Set up the thread-specific environment
                m_interpreter.pushEnvironment(env);

                // Execute the parallel block
                node.getBody()->accept(*this);

                // Clean up the thread-specific environment
                m_interpreter.popEnvironment();
            } catch (...) {
                // Store any exceptions that occur in this thread
                std::lock_guard<std::mutex> lock(mtx);
                exceptions.push_back(std::current_exception());
            }
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Re-throw any exceptions that occurred in the threads
    if (!exceptions.empty()) {
        std::rethrow_exception(exceptions[0]);
    }
}

void TemporalVisitor::visit(const BranchTimelineNode& node) {
    // Evaluate the number of branches
    Value branchCountVal = m_interpreter.evaluate(*node.getBranchCount());

    // Validate the branch count
    if (!branchCountVal.isNumber() || branchCountVal.asNumber() < 1) {
        throw RuntimeError("Branch count must be a positive number");
    }

    int branchCount = static_cast<int>(branchCountVal.asNumber());

    // Execute the branch timeline operation
    auto runtime = m_interpreter.getTemporalRuntime();
    if (!runtime) {
        throw RuntimeError("No temporal runtime available for branch timeline operation");
    }

    // Notify the runtime about the branch operation
    if (!runtime->branchTimeline(node.getBranchId(), branchCount)) {
        throw RuntimeError("Failed to create timeline branches");
    }

    try {
        // Execute the branch body
        node.getBody()->accept(*this);
    } catch (const std::exception& e) {
        // Ensure we clean up the branch on error
        runtime->mergeTimelines(node.getBranchId(), "error");
        throw;
    }

    // Merge the timelines back together
    if (!runtime->mergeTimelines(node.getBranchId(), "default")) {
        throw RuntimeError("Failed to merge timeline branches");
    }
}

}  // namespace chronovyan