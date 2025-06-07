#include <memory>
#include <string>
#include <vector>

#include "ast_nodes.h"
#include "gtest/gtest.h"
#include "interpreter.h"
#include "interpreter/TemporalVisitor.h"
#include "resource_optimizer.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"
#include "token.h"
#include "value.h"

using namespace chronovyan;

// Helper function to create literal expressions
std::unique_ptr<LiteralExprNode> createLiteralExpr(const LiteralValue& value) {
    return std::make_unique<LiteralExprNode>(SourceLocation(), value);
}

// Helper function to create variable expressions
std::unique_ptr<VariableExprNode> createVariableExpr(const std::string& name) {
    return std::make_unique<VariableExprNode>(SourceLocation(), name);
}

// Helper function to create rewind statements
std::unique_ptr<RewindStmtNode> createRewindStmt(std::unique_ptr<ExprNode> target,
                                                 std::unique_ptr<ExprNode> duration = nullptr) {
    return std::make_unique<RewindStmtNode>(SourceLocation(),
                                            Token(TokenType::REWIND, "rewind", SourceLocation()),
                                            std::move(target), std::move(duration));
}

// Helper function to create fastforward statements
std::unique_ptr<FastForwardStmtNode> createFastForwardStmt(
    std::unique_ptr<ExprNode> target, std::unique_ptr<ExprNode> duration = nullptr) {
    return std::make_unique<FastForwardStmtNode>(
        SourceLocation(), Token(TokenType::FASTFORWARD, "fastforward", SourceLocation()),
        std::move(target), std::move(duration));
}

// Helper function to create snapshot statements
std::unique_ptr<SnapshotStmtNode> createSnapshotStmt(std::unique_ptr<ExprNode> target,
                                                     const std::string& label) {
    return std::make_unique<SnapshotStmtNode>(
        SourceLocation(), Token(TokenType::SNAPSHOT, "snapshot", SourceLocation()),
        std::move(target), label);
}

// Helper function to create restore statements
std::unique_ptr<RestoreStmtNode> createRestoreStmt(std::unique_ptr<ExprNode> target,
                                                   const std::string& label) {
    return std::make_unique<RestoreStmtNode>(SourceLocation(),
                                             Token(TokenType::RESTORE, "restore", SourceLocation()),
                                             std::move(target), label);
}

// Helper function to create stabilize statements
std::unique_ptr<StabilizeStmtNode> createStabilizeStmt(std::unique_ptr<ExprNode> target) {
    return std::make_unique<StabilizeStmtNode>(
        SourceLocation(), Token(TokenType::STABILIZE, "stabilize", SourceLocation()),
        std::move(target));
}

// Mock classes for testing
class MockTemporalRuntime : public TemporalRuntime {
public:
    MockTemporalRuntime() : TemporalRuntime() {
        m_paradoxLevel = 0;
        m_stabilizationFactor = 1.0f;
    }

    int getParadoxLevel() const { return m_paradoxLevel; }

    void setParadoxLevel(int level) { m_paradoxLevel = level; }

    float getStabilizationFactor() const { return m_stabilizationFactor; }

    void setStabilizationFactor(float factor) { m_stabilizationFactor = factor; }

    // Track method calls
    struct MethodCall {
        std::string method;
        std::string targetName;
        double duration;
        std::string label;
    };

    std::vector<MethodCall> calls;

    // Override methods and track calls
    bool rewindVariable(const std::string& name, double duration) override {
        calls.push_back({"rewind", name, duration, ""});
        return true;
    }

    bool fastForwardVariable(const std::string& name, double duration) override {
        calls.push_back({"fastforward", name, duration, ""});
        return true;
    }

    bool createSnapshot(const std::string& name, const std::string& label) override {
        calls.push_back({"snapshot", name, 0.0, label});
        return true;
    }

    bool restoreSnapshot(const std::string& name, const std::string& label) override {
        calls.push_back({"restore", name, 0.0, label});
        return true;
    }

    bool stabilizeVariable(const std::string& name) override {
        calls.push_back({"stabilize", name, 0.0, ""});
        return true;
    }

private:
    int m_paradoxLevel;
    float m_stabilizationFactor;
};

class MockResourceManager {
public:
    MockResourceManager() {}

    // Track resource consumption calls
    struct ResourceCall {
        double aethel;
        double chronons;
        std::string operation;
    };

    std::vector<ResourceCall> calls;

    bool consumeResources(double aethel, double chronons, const std::string& operation) {
        calls.push_back({aethel, chronons, operation});
        return true;  // Always succeed for testing
    }
};

class MockInterpreter : public Interpreter {
public:
    MockInterpreter() {
        m_temporalRuntime = std::make_shared<MockTemporalRuntime>();
        m_resourceManager = std::make_shared<MockResourceManager>();
    }

    std::shared_ptr<MockTemporalRuntime> getTemporalRuntime() {
        return std::static_pointer_cast<MockTemporalRuntime>(m_temporalRuntime);
    }

    std::shared_ptr<MockResourceManager> getResourceManager() { return m_resourceManager; }

    Value evaluate(const ExprNode& expr) override {
        // For testing, just return predefined values based on node type
        if (auto literal = dynamic_cast<const LiteralExprNode*>(&expr)) {
            return Value(literal->getValue());
        } else if (auto var = dynamic_cast<const VariableExprNode*>(&expr)) {
            if (var->getName() == "target") {
                return Value("target_var");
            } else if (var->getName() == "duration") {
                return Value(static_cast<double>(5.0));
            } else if (var->getName() == "label") {
                return Value("test_label");
            }
        }

        // Default
        return Value();
    }

private:
    std::shared_ptr<MockResourceManager> m_resourceManager;
};

class TemporalVisitorTest : public ::testing::Test {
protected:
    MockInterpreter interpreter;
    TemporalVisitor visitor;

    TemporalVisitorTest() : visitor(interpreter) {}
};

TEST_F(TemporalVisitorTest, ExecutesRewindOperation) {
    // rewind target by 5.0;
    auto target = createVariableExpr("target");
    auto duration = createLiteralExpr(LiteralValue(static_cast<double>(5.0)));
    auto rewindStmt = createRewindStmt(std::move(target), std::move(duration));

    visitor.execute(*rewindStmt);

    // Check if rewind was called with correct parameters
    auto& calls = interpreter.getTemporalRuntime()->calls;
    EXPECT_EQ(calls.size(), 1);
    EXPECT_EQ(calls[0].method, "rewind");
    EXPECT_EQ(calls[0].targetName, "target_var");
    EXPECT_EQ(calls[0].duration, 5.0);

    // Check if resources were consumed
    auto& resourceCalls = interpreter.getResourceManager()->calls;
    EXPECT_EQ(resourceCalls.size(), 1);
    EXPECT_EQ(resourceCalls[0].operation, "rewind");
    EXPECT_GT(resourceCalls[0].aethel, 0);
    EXPECT_GT(resourceCalls[0].chronons, 0);
}

TEST_F(TemporalVisitorTest, ExecutesFastForwardOperation) {
    // fastforward target by 5.0;
    auto target = createVariableExpr("target");
    auto duration = createLiteralExpr(LiteralValue(static_cast<double>(5.0)));
    auto ffStmt = createFastForwardStmt(std::move(target), std::move(duration));

    visitor.execute(*ffStmt);

    // Check if fastforward was called with correct parameters
    auto& calls = interpreter.getTemporalRuntime()->calls;
    EXPECT_EQ(calls.size(), 1);
    EXPECT_EQ(calls[0].method, "fastforward");
    EXPECT_EQ(calls[0].targetName, "target_var");
    EXPECT_EQ(calls[0].duration, 5.0);

    // Check if resources were consumed
    auto& resourceCalls = interpreter.getResourceManager()->calls;
    EXPECT_EQ(resourceCalls.size(), 1);
    EXPECT_EQ(resourceCalls[0].operation, "fastforward");
    EXPECT_GT(resourceCalls[0].aethel, 0);
    EXPECT_GT(resourceCalls[0].chronons, 0);
}

TEST_F(TemporalVisitorTest, ExecutesSnapshotOperation) {
    // snapshot target as "test_label";
    auto target = createVariableExpr("target");
    auto snapshotStmt = createSnapshotStmt(std::move(target), "test_label");

    visitor.execute(*snapshotStmt);

    // Check if snapshot was called with correct parameters
    auto& calls = interpreter.getTemporalRuntime()->calls;
    EXPECT_EQ(calls.size(), 1);
    EXPECT_EQ(calls[0].method, "snapshot");
    EXPECT_EQ(calls[0].targetName, "target_var");
    EXPECT_EQ(calls[0].label, "test_label");

    // Check if resources were consumed
    auto& resourceCalls = interpreter.getResourceManager()->calls;
    EXPECT_EQ(resourceCalls.size(), 1);
    EXPECT_EQ(resourceCalls[0].operation, "snapshot");
    EXPECT_GT(resourceCalls[0].aethel, 0);
    EXPECT_GT(resourceCalls[0].chronons, 0);
}

TEST_F(TemporalVisitorTest, ExecutesRestoreOperation) {
    // restore target from "test_label";
    auto target = createVariableExpr("target");
    auto restoreStmt = createRestoreStmt(std::move(target), "test_label");

    visitor.execute(*restoreStmt);

    // Check if restore was called with correct parameters
    auto& calls = interpreter.getTemporalRuntime()->calls;
    EXPECT_EQ(calls.size(), 1);
    EXPECT_EQ(calls[0].method, "restore");
    EXPECT_EQ(calls[0].targetName, "target_var");
    EXPECT_EQ(calls[0].label, "test_label");

    // Check if resources were consumed
    auto& resourceCalls = interpreter.getResourceManager()->calls;
    EXPECT_EQ(resourceCalls.size(), 1);
    EXPECT_EQ(resourceCalls[0].operation, "restore");
    EXPECT_GT(resourceCalls[0].aethel, 0);
    EXPECT_GT(resourceCalls[0].chronons, 0);
}

TEST_F(TemporalVisitorTest, ExecutesStabilizeOperation) {
    // stabilize target;
    auto target = createVariableExpr("target");
    auto stabilizeStmt = createStabilizeStmt(std::move(target));

    visitor.execute(*stabilizeStmt);

    // Check if stabilize was called with correct parameters
    auto& calls = interpreter.getTemporalRuntime()->calls;
    EXPECT_EQ(calls.size(), 1);
    EXPECT_EQ(calls[0].method, "stabilize");
    EXPECT_EQ(calls[0].targetName, "target_var");

    // Check if resources were consumed
    auto& resourceCalls = interpreter.getResourceManager()->calls;
    EXPECT_EQ(resourceCalls.size(), 1);
    EXPECT_EQ(resourceCalls[0].operation, "stabilize");
    EXPECT_GT(resourceCalls[0].aethel, 0);
    EXPECT_GT(resourceCalls[0].chronons, 0);
}

TEST_F(TemporalVisitorTest, ParadoxLevelAffectsResourceConsumption) {
    // Set different paradox levels and verify resource consumption changes
    auto target = createVariableExpr("target");
    auto duration = createLiteralExpr(LiteralValue(static_cast<double>(5.0)));

    // Check at paradox level 0 (default)
    auto rewindStmt1 = createRewindStmt(createVariableExpr("target"),
                                        createLiteralExpr(LiteralValue(static_cast<double>(5.0))));
    visitor.execute(*rewindStmt1);

    auto& resourceCalls = interpreter.getResourceManager()->calls;
    double baseAethel = resourceCalls[0].aethel;
    double baseChronons = resourceCalls[0].chronons;

    // Increase paradox level
    interpreter.getTemporalRuntime()->setParadoxLevel(2);

    auto rewindStmt2 = createRewindStmt(createVariableExpr("target"),
                                        createLiteralExpr(LiteralValue(static_cast<double>(5.0))));
    visitor.execute(*rewindStmt2);

    // Resource consumption should be higher with higher paradox level
    EXPECT_GT(resourceCalls[1].aethel, baseAethel);
    EXPECT_GT(resourceCalls[1].chronons, baseChronons);
}

TEST_F(TemporalVisitorTest, StabilizationFactorAffectsResourceConsumption) {
    // Set different stabilization factors and verify resource consumption changes
    auto target = createVariableExpr("target");
    auto duration = createLiteralExpr(LiteralValue(static_cast<double>(5.0)));

    // Check at stabilization factor 1.0 (default)
    auto rewindStmt1 = createRewindStmt(createVariableExpr("target"),
                                        createLiteralExpr(LiteralValue(static_cast<double>(5.0))));
    visitor.execute(*rewindStmt1);

    auto& resourceCalls = interpreter.getResourceManager()->calls;
    double baseAethel = resourceCalls[0].aethel;
    double baseChronons = resourceCalls[0].chronons;

    // Decrease stabilization factor (makes operations more expensive)
    interpreter.getTemporalRuntime()->setStabilizationFactor(0.5f);

    auto rewindStmt2 = createRewindStmt(createVariableExpr("target"),
                                        createLiteralExpr(LiteralValue(static_cast<double>(5.0))));
    visitor.execute(*rewindStmt2);

    // Resource consumption should be higher with lower stabilization factor
    EXPECT_GT(resourceCalls[1].aethel, baseAethel);
    EXPECT_GT(resourceCalls[1].chronons, baseChronons);
}

TEST_F(TemporalVisitorTest, HandlesInvalidDurationType) {
    // rewind target by "invalid"; // String instead of number
    auto target = createVariableExpr("target");
    auto invalidDuration = createLiteralExpr(LiteralValue(std::string("invalid")));
    auto rewindStmt = createRewindStmt(std::move(target), std::move(invalidDuration));

    // This should not throw but log an error and skip the operation
    EXPECT_NO_THROW(visitor.execute(*rewindStmt));

    // Check that no operation was performed
    auto& calls = interpreter.getTemporalRuntime()->calls;
    EXPECT_TRUE(calls.empty());
}

// More tests would be added for other temporal operation scenarios