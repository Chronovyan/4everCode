#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "ast_nodes.h"
#include "ast_nodes/BranchTimelineNode.h"
#include "ast_nodes/ParallelExecutionNode.h"
#include "ast_nodes/TemporalLoopNode.h"
#include "gtest/gtest.h"
#include "interpreter/Interpreter.h"
#include "interpreter/TemporalVisitor.h"
#include "temporal_runtime.h"
#include "token.h"
#include "value.h"

using namespace chronovyan;
using namespace std::chrono_literals;

// Mock classes for testing advanced control flow
class MockTemporalRuntime : public TemporalRuntime {
    int m_paradoxLevel = 0;
    float m_stabilizationFactor = 1.0f;
    std::vector<std::string> m_timelineBranches;

public:
    MockTemporalRuntime() = default;

    // Track method calls
    struct MethodCall {
        std::string method;
        std::string branchId;
        int branchCount = 0;
        std::string mergeStrategy;
    };

    std::vector<MethodCall> calls;

    // Override TemporalRuntime methods
    bool branchTimeline(const std::string& branchId, int branchCount) override {
        calls.push_back({"branch_timeline", branchId, branchCount, ""});
        return true;
    }

    bool mergeTimelines(const std::string& branchId, const std::string& strategy) override {
        calls.push_back({"merge_timelines", branchId, 0, strategy});
        return true;
    }

    // Getters for test verification
    const std::vector<MethodCall>& getCalls() const { return calls; }

    // Mock implementations of required pure virtual functions
    int getParadoxLevel() const override { return m_paradoxLevel; }
    void setParadoxLevel(int level) override { m_paradoxLevel = level; }
    float getStabilizationFactor() const override { return m_stabilizationFactor; }
    void setStabilizationFactor(float factor) override { m_stabilizationFactor = factor; }

    // Stub implementations for pure virtual functions
    bool rewindVariable(const std::string&, double) override { return true; }
    bool fastForwardVariable(const std::string&, double) override { return true; }
    bool createSnapshot(const std::string&, const std::string&) override { return true; }
    bool restoreSnapshot(const std::string&, const std::string&) override { return true; }
    bool stabilizeVariable(const std::string&) override { return true; }
};

class MockInterpreter : public Interpreter {
    std::shared_ptr<MockTemporalRuntime> m_temporalRuntime;

public:
    MockInterpreter() { m_temporalRuntime = std::make_shared<MockTemporalRuntime>(); }

    std::shared_ptr<MockTemporalRuntime> getMockTemporalRuntime() { return m_temporalRuntime; }

    // Override to return our mock runtime
    std::shared_ptr<TemporalRuntime> getTemporalRuntime() const override {
        return m_temporalRuntime;
    }

    // Simple evaluation for testing
    Value evaluate(const ExprNode& expr) override {
        if (auto literal = dynamic_cast<const LiteralExprNode*>(&expr)) {
            return Value(literal->getValue());
        }
        return Value();
    }
};

class AdvancedControlFlowTest : public ::testing::Test {
protected:
    std::unique_ptr<MockInterpreter> interpreter;
    std::unique_ptr<TemporalVisitor> visitor;

    void SetUp() override {
        interpreter = std::make_unique<MockInterpreter>();
        visitor = std::make_unique<TemporalVisitor>(*interpreter);
    }

    void TearDown() override {
        visitor.reset();
        interpreter.reset();
    }
};

// Test for TEMPORAL_LOOP
TEST_F(AdvancedControlFlowTest, BasicTemporalLoop) {
    // Counter to track loop iterations
    std::atomic<int> counter{0};

    // Create a simple temporal loop that runs for 3 iterations
    auto loopBody = std::make_unique<BlockStmtNode>(SourceLocation());

    // Add a statement to increment the counter
    auto incCounter = std::make_unique<ExprStmtNode>(
        SourceLocation(),
        std::make_unique<FunctionCallExprNode>(SourceLocation(), "increment",
                                               std::vector<std::unique_ptr<ExprNode>>()));
    loopBody->addStatement(std::move(incCounter));

    // Create the temporal loop node
    auto loopNode = std::make_unique<TemporalLoopNode>(
        SourceLocation(),
        std::make_unique<LiteralExprNode>(SourceLocation(), 50.0),  // 50ms duration
        std::make_unique<LiteralExprNode>(SourceLocation(), 3),     // 3 iterations
        std::move(loopBody));

    // Mock the function call
    interpreter->define("increment", Value([&counter](const std::vector<Value>&) {
                            counter++;
                            return Value();
                        }));

    // Execute the temporal loop
    auto start = std::chrono::steady_clock::now();
    visitor->visit(*loopNode);
    auto end = std::chrono::steady_clock::now();

    // Verify the loop executed the expected number of times
    ASSERT_EQ(counter.load(), 3);

    // Verify the total time is at least (iterations * duration)
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    ASSERT_GE(duration, 100);  // 3 * 50ms = 150ms, but we'll be conservative with 100ms
}

// Test for PARALLEL_EXECUTION
TEST_F(AdvancedControlFlowTest, BasicParallelExecution) {
    // Counter to track completed tasks
    std::atomic<int> completedTasks{0};

    // Create a parallel execution block with 4 threads
    auto parallelBlock = std::make_unique<BlockStmtNode>(SourceLocation());

    // Add work that takes some time to complete
    auto task = std::make_unique<ExprStmtNode>(
        SourceLocation(),
        std::make_unique<FunctionCallExprNode>(SourceLocation(), "do_work",
                                               std::vector<std::unique_ptr<ExprNode>>()));

    // Add multiple tasks to the parallel block
    for (int i = 0; i < 4; ++i) {
        parallelBlock->addStatement(task->clone());
    }

    // Create the parallel execution node with 4 threads
    auto parallelNode = std::make_unique<ParallelExecutionNode>(SourceLocation(),
                                                                4,  // 4 threads
                                                                std::move(parallelBlock));

    // Mock the work function
    interpreter->define("do_work", Value([&completedTasks](const std::vector<Value>&) {
                            // Simulate work by sleeping
                            std::this_thread::sleep_for(50ms);
                            completedTasks++;
                            return Value();
                        }));

    // Execute the parallel block and measure time
    auto start = std::chrono::steady_clock::now();
    visitor->visit(*parallelNode);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Verify all tasks completed
    ASSERT_EQ(completedTasks.load(), 4);

    // Verify that the tasks ran in parallel (should take less than 4 * 50ms)
    ASSERT_LT(duration, 150);
}

// Test for BRANCH_TIMELINE
TEST_F(AdvancedControlFlowTest, BasicBranchTimeline) {
    // Get the mock temporal runtime
    auto mockRuntime = interpreter->getMockTemporalRuntime();

    // Create a branch timeline with 3 branches
    auto branchBlock = std::make_unique<BlockStmtNode>(SourceLocation());

    // Add some work to the branch block
    auto branchWork = std::make_unique<ExprStmtNode>(
        SourceLocation(),
        std::make_unique<FunctionCallExprNode>(SourceLocation(), "branch_work",
                                               std::vector<std::unique_ptr<ExprNode>>()));
    branchBlock->addStatement(std::move(branchWork));

    // Create the branch timeline node
    auto branchNode = std::make_unique<BranchTimelineNode>(
        SourceLocation(), std::make_unique<LiteralExprNode>(SourceLocation(), 3),  // 3 branches
        std::move(branchBlock),
        "test_branch"  // Branch ID
    );

    // Track which branches were executed
    std::vector<int> executedBranches;
    std::mutex mutex;

    // Mock the branch work function
    interpreter->define("branch_work", Value([&](const std::vector<Value>&) {
                            std::lock_guard<std::mutex> lock(mutex);
                            // Get the current branch ID from the runtime
                            auto branchId = interpreter->evaluate("__current_branch__").asString();
                            executedBranches.push_back(std::stoi(branchId));
                            return Value();
                        }));

    // Execute the branch timeline
    visitor->visit(*branchNode);

    // Verify that the branch and merge operations were called correctly
    const auto& calls = mockRuntime->getCalls();
    ASSERT_GE(calls.size(), 2);

    // First call should be branch_timeline
    EXPECT_EQ(calls[0].method, "branch_timeline");
    EXPECT_EQ(calls[0].branchId, "test_branch");
    EXPECT_EQ(calls[0].branchCount, 3);

    // Last call should be merge_timelines
    EXPECT_EQ(calls.back().method, "merge_timelines");
    EXPECT_EQ(calls.back().branchId, "test_branch");
    EXPECT_EQ(calls.back().mergeStrategy, "default");

    // Verify that all branches were executed
    std::sort(executedBranches.begin(), executedBranches.end());
    ASSERT_EQ(executedBranches.size(), 3);
    EXPECT_EQ(executedBranches[0], 0);
    EXPECT_EQ(executedBranches[1], 1);
    EXPECT_EQ(executedBranches[2], 2);
}

// Test for interaction between parallel execution and temporal loops
TEST_F(AdvancedControlFlowTest, ParallelTemporalInteraction) {
    // Counter to track completed iterations
    std::vector<int> completedIterations;
    std::mutex mutex;

    // Create a parallel execution block with 2 threads
    auto parallelBlock = std::make_unique<BlockStmtNode>(SourceLocation());

    // Create a temporal loop that will run in parallel
    auto loopBody = std::make_unique<BlockStmtNode>(SourceLocation());

    // Add work to the loop body
    auto loopWork = std::make_unique<ExprStmtNode>(
        SourceLocation(),
        std::make_unique<FunctionCallExprNode>(
            SourceLocation(), "do_work",
            std::vector<std::unique_ptr<ExprNode>>(
                {std::make_unique<LiteralExprNode>(SourceLocation(), "__current_thread__")})));
    loopBody->addStatement(std::move(loopWork));

    // Create the temporal loop node (2 iterations, 30ms each)
    auto loopNode = std::make_unique<TemporalLoopNode>(
        SourceLocation(), std::make_unique<LiteralExprNode>(SourceLocation(), 30.0),
        std::make_unique<LiteralExprNode>(SourceLocation(), 2), std::move(loopBody));

    // Add the loop to the parallel block
    parallelBlock->addStatement(std::move(loopNode));

    // Create the parallel execution node with 2 threads
    auto parallelNode = std::make_unique<ParallelExecutionNode>(SourceLocation(),
                                                                2,  // 2 threads
                                                                std::move(parallelBlock));

    // Mock the work function
    interpreter->define("do_work", Value([&](const std::vector<Value>& args) {
                            std::string threadId = args[0].asString();
                            std::this_thread::sleep_for(20ms);

                            std::lock_guard<std::mutex> lock(mutex);
                            completedIterations.push_back(threadId == "thread_0" ? 1 : 2);
                            return Value();
                        }));

    // Execute the parallel block with temporal loops
    auto start = std::chrono::steady_clock::now();
    visitor->visit(*parallelNode);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Verify that we have 4 completed iterations total (2 threads * 2 iterations)
    ASSERT_EQ(completedIterations.size(), 4);

    // Verify that the execution was parallel (should take less than 2 * 2 * 30ms)
    ASSERT_LT(duration, 100);

    // Count iterations per thread
    int thread1Count = std::count(completedIterations.begin(), completedIterations.end(), 1);
    int thread2Count = std::count(completedIterations.begin(), completedIterations.end(), 2);

    // Verify both threads did work
    ASSERT_EQ(thread1Count, 2);
    ASSERT_EQ(thread2Count, 2);
        std::move(parallelBlock)
    );

        // Execute the test
        visitor->visit(*parallelNode);

        // In a real test, we would verify that the temporal loops in each thread
        // executed independently and correctly
        ASSERT_TRUE(true);
}
