#include <memory>
#include <string>
#include <vector>

#include "ast_nodes.h"
#include "catch2/catch.hpp"
#include "gtest/gtest.h"
#include "interpreter.h"
#include "interpreter/ExpressionVisitor.h"
#include "interpreter/ResourceManager.h"
#include "interpreter/StatementVisitor.h"
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

// Helper function to create expression statements
std::unique_ptr<ExprStmtNode> createExprStmt(std::unique_ptr<ExprNode> expr) {
    return std::make_unique<ExprStmtNode>(SourceLocation(), std::move(expr));
}

// Helper function to create variable declaration statements
std::unique_ptr<VariableDeclStmtNode> createVarDeclStmt(
    const std::string& name, VariableModifier modifier,
    std::unique_ptr<ExprNode> initializer = nullptr) {
    auto typeNode = std::make_unique<BasicTypeNode>(SourceLocation(), TokenType::IDENTIFIER, "Any");
    std::vector<VariableFlag> flags;

    return std::make_unique<VariableDeclStmtNode>(SourceLocation(), name, std::move(typeNode),
                                                  modifier, flags, std::move(initializer));
}

// Helper function to create binary expressions
std::unique_ptr<BinaryExprNode> createBinaryExpr(std::unique_ptr<ExprNode> left, TokenType op,
                                                 std::unique_ptr<ExprNode> right) {
    return std::make_unique<BinaryExprNode>(SourceLocation(), std::move(left),
                                            Token(op, tokenTypeToString(op), SourceLocation()),
                                            std::move(right));
}

// Helper function to create assign expressions
std::unique_ptr<AssignExprNode> createAssignExpr(const std::string& name,
                                                 std::unique_ptr<ExprNode> value) {
    return std::make_unique<AssignExprNode>(
        SourceLocation(), name, Token(TokenType::EQUAL, "=", SourceLocation()), std::move(value));
}

// Helper function to create rewind statements
std::unique_ptr<RewindStmtNode> createRewindStmt(std::unique_ptr<ExprNode> target,
                                                 std::unique_ptr<ExprNode> duration = nullptr) {
    return std::make_unique<RewindStmtNode>(SourceLocation(),
                                            Token(TokenType::REWIND, "rewind", SourceLocation()),
                                            std::move(target), std::move(duration));
}

class InterpreterComponentsIntegrationTest : public ::testing::Test {
protected:
    Interpreter interpreter;
    ExpressionVisitor& expressionVisitor;
    StatementVisitor& statementVisitor;
    TemporalVisitor& temporalVisitor;
    ResourceManager& resourceManager;

    InterpreterComponentsIntegrationTest()
        : expressionVisitor(interpreter.getExpressionVisitor())
        , statementVisitor(interpreter.getStatementVisitor())
        , temporalVisitor(interpreter.getTemporalVisitor())
        , resourceManager(interpreter.getResourceManager()) {}
};

TEST_F(InterpreterComponentsIntegrationTest, ExecutesVariableDeclarationsAndExpressions) {
    // var x: Any = 10;
    auto declX = createVarDeclStmt("x", VariableModifier::CONF,
                                   createLiteralExpr(LiteralValue(static_cast<int64_t>(10))));

    // var y: Any = 20;
    auto declY = createVarDeclStmt("y", VariableModifier::CONF,
                                   createLiteralExpr(LiteralValue(static_cast<int64_t>(20))));

    // Execute the declarations
    statementVisitor.execute(*declX);
    statementVisitor.execute(*declY);

    // Check that variables were declared
    EXPECT_EQ(interpreter.getEnvironment()->get("x").asInteger(), 10);
    EXPECT_EQ(interpreter.getEnvironment()->get("y").asInteger(), 20);

    // Create an expression: x + y
    auto addExpr =
        createBinaryExpr(createVariableExpr("x"), TokenType::PLUS, createVariableExpr("y"));

    // Evaluate the expression
    Value result = expressionVisitor.evaluate(*addExpr);

    // Check the result
    EXPECT_TRUE(result.isInteger());
    EXPECT_EQ(result.asInteger(), 30);

    // Create an assignment: z = x + y
    auto assignExpr = createAssignExpr(
        "z", createBinaryExpr(createVariableExpr("x"), TokenType::PLUS, createVariableExpr("y")));

    // Evaluate the assignment
    expressionVisitor.evaluate(*assignExpr);

    // Check that z was assigned correctly
    EXPECT_EQ(interpreter.getEnvironment()->get("z").asInteger(), 30);
}

TEST_F(InterpreterComponentsIntegrationTest, ExecutesTemporalOperationsWithResourceTracking) {
    // Initialize a counter variable
    auto declCounter = createVarDeclStmt("counter", VariableModifier::CONF,
                                         createLiteralExpr(LiteralValue(static_cast<int64_t>(0))));

    statementVisitor.execute(*declCounter);

    // Record initial resource levels
    double initialAethel = resourceManager.getAethelLevel();
    double initialChronons = resourceManager.getChrononsLevel();

    // Execute a temporal operation: rewind counter by 5.0
    auto rewindStmt = createRewindStmt(createVariableExpr("counter"),
                                       createLiteralExpr(LiteralValue(static_cast<double>(5.0))));

    temporalVisitor.execute(*rewindStmt);

    // Check that resources were consumed
    EXPECT_LT(resourceManager.getAethelLevel(), initialAethel);
    EXPECT_LT(resourceManager.getChrononsLevel(), initialChronons);

    // Check the resource usage history
    auto usageHistory = resourceManager.getResourceUsageHistory();
    EXPECT_NE(usageHistory.find("rewind"), usageHistory.end());
    EXPECT_EQ(usageHistory.at("rewind").count, 1);
}

TEST_F(InterpreterComponentsIntegrationTest, ExecutesComprehensiveWorkflow) {
    // 1. Declare variables
    auto declX = createVarDeclStmt("x", VariableModifier::CONF,
                                   createLiteralExpr(LiteralValue(static_cast<int64_t>(10))));

    auto declY = createVarDeclStmt("y", VariableModifier::REB,
                                   createLiteralExpr(LiteralValue(static_cast<int64_t>(20))));

    // Execute declarations
    statementVisitor.execute(*declX);
    statementVisitor.execute(*declY);

    // 2. Perform arithmetic operation
    auto addExpr =
        createBinaryExpr(createVariableExpr("x"), TokenType::PLUS, createVariableExpr("y"));

    // Assign result to z
    auto assignZ = createAssignExpr("z", std::move(addExpr));
    auto assignStmt = createExprStmt(std::move(assignZ));

    statementVisitor.execute(*assignStmt);

    // 3. Perform a temporal operation on z
    auto rewindStmt = createRewindStmt(createVariableExpr("z"),
                                       createLiteralExpr(LiteralValue(static_cast<double>(3.0))));

    // Record resource levels before temporal op
    double beforeAethel = resourceManager.getAethelLevel();
    double beforeChronons = resourceManager.getChrononsLevel();

    temporalVisitor.execute(*rewindStmt);

    // 4. Verify results
    // a) Variables should have correct values
    EXPECT_EQ(interpreter.getEnvironment()->get("x").asInteger(), 10);
    EXPECT_EQ(interpreter.getEnvironment()->get("y").asInteger(), 20);
    EXPECT_EQ(interpreter.getEnvironment()->get("z").asInteger(), 30);

    // b) Resources should be consumed
    EXPECT_LT(resourceManager.getAethelLevel(), beforeAethel);
    EXPECT_LT(resourceManager.getChrononsLevel(), beforeChronons);

    // c) Operation should be in resource history
    auto usageHistory = resourceManager.getResourceUsageHistory();
    EXPECT_NE(usageHistory.find("rewind"), usageHistory.end());

    // d) Check for any temporal debt tracked
    // This would require access to the debt tracker, which might not be directly available
    // For integration test purposes, we just verify that the system works as a whole
}