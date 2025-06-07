#include <memory>
#include <string>
#include <vector>

#include "ast_nodes.h"
#include "environment.h"
#include "gtest/gtest.h"
#include "interpreter.h"
#include "interpreter/StatementVisitor.h"
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

// Helper function to create a block statement
std::unique_ptr<BlockStmtNode> createBlockStmt(std::vector<std::unique_ptr<StmtNode>> statements) {
    return std::make_unique<BlockStmtNode>(SourceLocation(), std::move(statements));
}

// Helper function to create an if statement
std::unique_ptr<IfStmtNode> createIfStmt(std::unique_ptr<ExprNode> condition,
                                         std::unique_ptr<StmtNode> thenBranch,
                                         std::unique_ptr<StmtNode> elseBranch = nullptr) {
    return std::make_unique<IfStmtNode>(SourceLocation(), std::move(condition),
                                        std::move(thenBranch), std::move(elseBranch));
}

// Mock interpreter for testing
class MockInterpreter : public Interpreter {
public:
    MockInterpreter() {
        // Initialize with a test environment
        m_env = std::make_shared<Environment>(nullptr);
        setEnvironment(m_env);
    }

    Value getVariable(const std::string& name) { return m_env->get(name); }

    bool hasVariable(const std::string& name) {
        try {
            m_env->get(name);
            return true;
        } catch (const RuntimeError&) {
            return false;
        }
    }

    std::shared_ptr<Environment> m_env;
};

class StatementVisitorTest : public ::testing::Test {
protected:
    MockInterpreter interpreter;
    StatementVisitor visitor;

    StatementVisitorTest() : visitor(interpreter) {}
};

TEST_F(StatementVisitorTest, ExecutesVariableDeclarationsWithInitializer) {
    // var x: Any = 42;
    auto stmt = createVarDeclStmt("x", VariableModifier::CONF,
                                  createLiteralExpr(LiteralValue(static_cast<int64_t>(42))));

    visitor.execute(*stmt);

    // Check if variable was defined
    EXPECT_TRUE(interpreter.hasVariable("x"));

    // Check value
    Value value = interpreter.getVariable("x");
    EXPECT_TRUE(value.isInteger());
    EXPECT_EQ(value.asInteger(), 42);

    // Check metadata
    EXPECT_TRUE(value.hasFlaggedMetadata("CONF"));
    EXPECT_TRUE(value.getFlaggedMetadata("CONF").asBoolean());
}

TEST_F(StatementVisitorTest, ExecutesVariableDeclarationsWithoutInitializer) {
    // var y: Any;
    auto stmt = createVarDeclStmt("y", VariableModifier::REB);

    visitor.execute(*stmt);

    // Check if variable was defined
    EXPECT_TRUE(interpreter.hasVariable("y"));

    // Check value (should be nil)
    Value value = interpreter.getVariable("y");
    EXPECT_TRUE(value.isNil());

    // Check metadata
    EXPECT_TRUE(value.hasFlaggedMetadata("REB"));
    EXPECT_TRUE(value.getFlaggedMetadata("REB").asBoolean());
}

TEST_F(StatementVisitorTest, ExecutesExpressionStatements) {
    // Create a variable to update
    auto varDecl = createVarDeclStmt("counter", VariableModifier::CONF,
                                     createLiteralExpr(LiteralValue(static_cast<int64_t>(0))));

    visitor.execute(*varDecl);

    // Create an expression statement that updates the variable
    // counter = counter + 1;
    auto assignExpr = std::make_unique<AssignExprNode>(
        SourceLocation(), "counter", Token(TokenType::EQUAL, "=", SourceLocation()),
        std::make_unique<BinaryExprNode>(SourceLocation(), createVariableExpr("counter"),
                                         Token(TokenType::PLUS, "+", SourceLocation()),
                                         createLiteralExpr(LiteralValue(static_cast<int64_t>(1)))));

    auto exprStmt = createExprStmt(std::move(assignExpr));
    visitor.execute(*exprStmt);

    // Check if the variable was updated
    Value value = interpreter.getVariable("counter");
    EXPECT_TRUE(value.isInteger());
    EXPECT_EQ(value.asInteger(), 1);
}

TEST_F(StatementVisitorTest, ExecutesBlocks) {
    // Create a block with multiple statements
    std::vector<std::unique_ptr<StmtNode>> statements;

    // var x: Any = 10;
    statements.push_back(createVarDeclStmt(
        "x", VariableModifier::CONF, createLiteralExpr(LiteralValue(static_cast<int64_t>(10)))));

    // var y: Any = 20;
    statements.push_back(createVarDeclStmt(
        "y", VariableModifier::CONF, createLiteralExpr(LiteralValue(static_cast<int64_t>(20)))));

    // var z: Any = x + y;
    auto addExpr = std::make_unique<BinaryExprNode>(SourceLocation(), createVariableExpr("x"),
                                                    Token(TokenType::PLUS, "+", SourceLocation()),
                                                    createVariableExpr("y"));

    statements.push_back(createVarDeclStmt("z", VariableModifier::CONF, std::move(addExpr)));

    auto blockStmt = createBlockStmt(std::move(statements));
    visitor.execute(*blockStmt);

    // Check if variables were defined with correct values
    EXPECT_TRUE(interpreter.hasVariable("x"));
    EXPECT_TRUE(interpreter.hasVariable("y"));
    EXPECT_TRUE(interpreter.hasVariable("z"));

    Value z = interpreter.getVariable("z");
    EXPECT_TRUE(z.isInteger());
    EXPECT_EQ(z.asInteger(), 30);
}

TEST_F(StatementVisitorTest, ExecutesIfStatementsWithTrueCondition) {
    // Initialize a variable
    auto varDecl = createVarDeclStmt("result", VariableModifier::CONF,
                                     createLiteralExpr(LiteralValue(static_cast<int64_t>(0))));

    visitor.execute(*varDecl);

    // if (true) { result = 1; }
    auto condition = createLiteralExpr(LiteralValue(true));

    auto thenBranch = createExprStmt(std::make_unique<AssignExprNode>(
        SourceLocation(), "result", Token(TokenType::EQUAL, "=", SourceLocation()),
        createLiteralExpr(LiteralValue(static_cast<int64_t>(1)))));

    auto ifStmt = createIfStmt(std::move(condition), std::move(thenBranch));
    visitor.execute(*ifStmt);

    // Check if result was updated
    Value result = interpreter.getVariable("result");
    EXPECT_TRUE(result.isInteger());
    EXPECT_EQ(result.asInteger(), 1);
}

TEST_F(StatementVisitorTest, ExecutesIfStatementsWithFalseCondition) {
    // Initialize a variable
    auto varDecl = createVarDeclStmt("result", VariableModifier::CONF,
                                     createLiteralExpr(LiteralValue(static_cast<int64_t>(0))));

    visitor.execute(*varDecl);

    // if (false) { result = 1; }
    auto condition = createLiteralExpr(LiteralValue(false));

    auto thenBranch = createExprStmt(std::make_unique<AssignExprNode>(
        SourceLocation(), "result", Token(TokenType::EQUAL, "=", SourceLocation()),
        createLiteralExpr(LiteralValue(static_cast<int64_t>(1)))));

    auto ifStmt = createIfStmt(std::move(condition), std::move(thenBranch));
    visitor.execute(*ifStmt);

    // Check if result was not updated
    Value result = interpreter.getVariable("result");
    EXPECT_TRUE(result.isInteger());
    EXPECT_EQ(result.asInteger(), 0);
}

TEST_F(StatementVisitorTest, ExecutesIfStatementsWithElseBranch) {
    // Initialize a variable
    auto varDecl = createVarDeclStmt("result", VariableModifier::CONF,
                                     createLiteralExpr(LiteralValue(static_cast<int64_t>(0))));

    visitor.execute(*varDecl);

    // if (false) { result = 1; } else { result = 2; }
    auto condition = createLiteralExpr(LiteralValue(false));

    auto thenBranch = createExprStmt(std::make_unique<AssignExprNode>(
        SourceLocation(), "result", Token(TokenType::EQUAL, "=", SourceLocation()),
        createLiteralExpr(LiteralValue(static_cast<int64_t>(1)))));

    auto elseBranch = createExprStmt(std::make_unique<AssignExprNode>(
        SourceLocation(), "result", Token(TokenType::EQUAL, "=", SourceLocation()),
        createLiteralExpr(LiteralValue(static_cast<int64_t>(2)))));

    auto ifStmt = createIfStmt(std::move(condition), std::move(thenBranch), std::move(elseBranch));

    visitor.execute(*ifStmt);

    // Check if result was updated by the else branch
    Value result = interpreter.getVariable("result");
    EXPECT_TRUE(result.isInteger());
    EXPECT_EQ(result.asInteger(), 2);
}

TEST_F(StatementVisitorTest, HandlesReturnStatementSetsReturnFlagAndValue) {
    // return 42;
    auto returnStmt = std::make_unique<ReturnStmtNode>(
        SourceLocation(), createLiteralExpr(LiteralValue(static_cast<int64_t>(42))));

    visitor.execute(*returnStmt);

    // Check return flag and value
    EXPECT_TRUE(visitor.isReturning());
    EXPECT_TRUE(visitor.getReturnValue().isInteger());
    EXPECT_EQ(visitor.getReturnValue().asInteger(), 42);

    // Clear the flags and check
    visitor.clearControlFlowFlags();
    EXPECT_FALSE(visitor.isReturning());
}

TEST_F(StatementVisitorTest, StopsExecutingBlockAfterReturn) {
    // Create a block with multiple statements, including a return
    std::vector<std::unique_ptr<StmtNode>> statements;

    // var x: Any = 0;
    statements.push_back(createVarDeclStmt(
        "x", VariableModifier::CONF, createLiteralExpr(LiteralValue(static_cast<int64_t>(0)))));

    // x = 1;
    statements.push_back(createExprStmt(std::make_unique<AssignExprNode>(
        SourceLocation(), "x", Token(TokenType::EQUAL, "=", SourceLocation()),
        createLiteralExpr(LiteralValue(static_cast<int64_t>(1))))));

    // return 42;
    statements.push_back(std::make_unique<ReturnStmtNode>(
        SourceLocation(), createLiteralExpr(LiteralValue(static_cast<int64_t>(42)))));

    // var y: Any = 0;
    statements.push_back(createVarDeclStmt(
        "y", VariableModifier::CONF, createLiteralExpr(LiteralValue(static_cast<int64_t>(0)))));

    // y = 1; (should not execute)
    statements.push_back(createExprStmt(std::make_unique<AssignExprNode>(
        SourceLocation(), "y", Token(TokenType::EQUAL, "=", SourceLocation()),
        createLiteralExpr(LiteralValue(static_cast<int64_t>(1))))));

    auto blockStmt = createBlockStmt(std::move(statements));
    visitor.execute(*blockStmt);

    // Check variable values
    Value x = interpreter.getVariable("x");
    Value y = interpreter.getVariable("y");

    EXPECT_TRUE(x.isInteger());
    EXPECT_EQ(x.asInteger(), 1);

    EXPECT_TRUE(y.isInteger());
    EXPECT_EQ(y.asInteger(), 0);  // Still 0 because execution stopped at return

    // Check return flag and value
    EXPECT_TRUE(visitor.isReturning());
    EXPECT_TRUE(visitor.getReturnValue().isInteger());
    EXPECT_EQ(visitor.getReturnValue().asInteger(), 42);
}

// More tests would be added for other statement types and complex scenarios