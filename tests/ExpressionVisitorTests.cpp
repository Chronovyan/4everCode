#include <memory>
#include <string>

#include "ast_nodes.h"
#include "environment.h"
#include "gtest/gtest.h"
#include "interpreter.h"
#include "interpreter/ExpressionVisitor.h"
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

// Helper function to create binary expressions
std::unique_ptr<BinaryExprNode> createBinaryExpr(std::unique_ptr<ExprNode> left, TokenType op,
                                                 std::unique_ptr<ExprNode> right) {
    Token opToken(op, "", SourceLocation());
    return std::make_unique<BinaryExprNode>(SourceLocation(), std::move(left), opToken,
                                            std::move(right));
}

// Helper function to create unary expressions
std::unique_ptr<UnaryExprNode> createUnaryExpr(TokenType op, std::unique_ptr<ExprNode> right) {
    Token opToken(op, "", SourceLocation());
    return std::make_unique<UnaryExprNode>(SourceLocation(), opToken, std::move(right));
}

// Mock interpreter for testing
class MockInterpreter : public Interpreter {
public:
    MockInterpreter() {
        // Initialize with a test environment
        m_env = std::make_shared<Environment>(nullptr);
        setEnvironment(m_env);
    }

    void defineVariable(const std::string& name, const Value& value) { m_env->define(name, value); }

    std::shared_ptr<Environment> m_env;
};

class ExpressionVisitorTest : public ::testing::Test {
protected:
    MockInterpreter interpreter;
    ExpressionVisitor visitor;

    ExpressionVisitorTest() : visitor(interpreter) {}
};

TEST_F(ExpressionVisitorTest, EvaluatesIntegerLiterals) {
    auto expr = createLiteralExpr(LiteralValue(static_cast<int64_t>(42)));
    Value result = visitor.evaluate(*expr);

    EXPECT_TRUE(result.isInteger());
    EXPECT_EQ(result.asInteger(), 42);
}

TEST_F(ExpressionVisitorTest, EvaluatesFloatLiterals) {
    auto expr = createLiteralExpr(LiteralValue(3.14));
    Value result = visitor.evaluate(*expr);

    EXPECT_TRUE(result.isNumber());
    EXPECT_DOUBLE_EQ(result.asNumber(), 3.14);
}

TEST_F(ExpressionVisitorTest, EvaluatesStringLiterals) {
    auto expr = createLiteralExpr(LiteralValue("hello"));
    Value result = visitor.evaluate(*expr);

    EXPECT_TRUE(result.isString());
    EXPECT_EQ(result.asString(), "hello");
}

TEST_F(ExpressionVisitorTest, EvaluatesBooleanLiterals) {
    auto exprTrue = createLiteralExpr(LiteralValue(true));
    Value resultTrue = visitor.evaluate(*exprTrue);

    EXPECT_TRUE(resultTrue.isBoolean());
    EXPECT_TRUE(resultTrue.asBoolean());

    auto exprFalse = createLiteralExpr(LiteralValue(false));
    Value resultFalse = visitor.evaluate(*exprFalse);

    EXPECT_TRUE(resultFalse.isBoolean());
    EXPECT_FALSE(resultFalse.asBoolean());
}

TEST_F(ExpressionVisitorTest, EvaluatesNilLiterals) {
    auto expr = createLiteralExpr(LiteralValue());
    Value result = visitor.evaluate(*expr);

    EXPECT_TRUE(result.isNil());
}

TEST_F(ExpressionVisitorTest, EvaluatesExistingVariables) {
    interpreter.defineVariable("x", Value(static_cast<int64_t>(10)));
    interpreter.defineVariable("y", Value(20.5));
    interpreter.defineVariable("name", Value("test"));

    auto exprX = createVariableExpr("x");
    Value resultX = visitor.evaluate(*exprX);

    EXPECT_TRUE(resultX.isInteger());
    EXPECT_EQ(resultX.asInteger(), 10);

    auto exprY = createVariableExpr("y");
    Value resultY = visitor.evaluate(*exprY);

    EXPECT_TRUE(resultY.isNumber());
    EXPECT_DOUBLE_EQ(resultY.asNumber(), 20.5);

    auto exprName = createVariableExpr("name");
    Value resultName = visitor.evaluate(*exprName);

    EXPECT_TRUE(resultName.isString());
    EXPECT_EQ(resultName.asString(), "test");
}

TEST_F(ExpressionVisitorTest, ThrowsOnUndefinedVariables) {
    auto expr = createVariableExpr("undefined");

    EXPECT_THROW(visitor.evaluate(*expr), RuntimeError);
}

TEST_F(ExpressionVisitorTest, EvaluatesArithmeticOperations) {
    // 2 + 3
    auto addExpr =
        createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(2))), TokenType::PLUS,
                         createLiteralExpr(LiteralValue(static_cast<int64_t>(3))));

    Value addResult = visitor.evaluate(*addExpr);
    EXPECT_TRUE(addResult.isInteger());
    EXPECT_EQ(addResult.asInteger(), 5);

    // 5 - 2
    auto subExpr =
        createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(5))), TokenType::MINUS,
                         createLiteralExpr(LiteralValue(static_cast<int64_t>(2))));

    Value subResult = visitor.evaluate(*subExpr);
    EXPECT_TRUE(subResult.isInteger());
    EXPECT_EQ(subResult.asInteger(), 3);

    // 4 * 3
    auto mulExpr =
        createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(4))), TokenType::STAR,
                         createLiteralExpr(LiteralValue(static_cast<int64_t>(3))));

    Value mulResult = visitor.evaluate(*mulExpr);
    EXPECT_TRUE(mulResult.isInteger());
    EXPECT_EQ(mulResult.asInteger(), 12);

    // 10 / 2
    auto divExpr = createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(10))),
                                    TokenType::SLASH,
                                    createLiteralExpr(LiteralValue(static_cast<int64_t>(2))));

    Value divResult = visitor.evaluate(*divExpr);
    EXPECT_TRUE(divResult.isNumber());
    EXPECT_DOUBLE_EQ(divResult.asNumber(), 5.0);
}

TEST_F(ExpressionVisitorTest, EvaluatesStringConcatenation) {
    // "hello" + " world"
    auto concatExpr = createBinaryExpr(createLiteralExpr(LiteralValue("hello")), TokenType::PLUS,
                                       createLiteralExpr(LiteralValue(" world")));

    Value result = visitor.evaluate(*concatExpr);
    EXPECT_TRUE(result.isString());
    EXPECT_EQ(result.asString(), "hello world");
}

TEST_F(ExpressionVisitorTest, EvaluatesComparisonOperations) {
    // 5 > 3
    auto gtExpr = createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(5))),
                                   TokenType::GREATER,
                                   createLiteralExpr(LiteralValue(static_cast<int64_t>(3))));

    Value gtResult = visitor.evaluate(*gtExpr);
    EXPECT_TRUE(gtResult.isBoolean());
    EXPECT_TRUE(gtResult.asBoolean());

    // 2 < 3
    auto ltExpr =
        createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(2))), TokenType::LESS,
                         createLiteralExpr(LiteralValue(static_cast<int64_t>(3))));

    Value ltResult = visitor.evaluate(*ltExpr);
    EXPECT_TRUE(ltResult.isBoolean());
    EXPECT_TRUE(ltResult.asBoolean());

    // 5 >= 5
    auto geExpr = createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(5))),
                                   TokenType::GREATER_EQUAL,
                                   createLiteralExpr(LiteralValue(static_cast<int64_t>(5))));

    Value geResult = visitor.evaluate(*geExpr);
    EXPECT_TRUE(geResult.isBoolean());
    EXPECT_TRUE(geResult.asBoolean());

    // 5 <= 3
    auto leExpr = createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(5))),
                                   TokenType::LESS_EQUAL,
                                   createLiteralExpr(LiteralValue(static_cast<int64_t>(3))));

    Value leResult = visitor.evaluate(*leExpr);
    EXPECT_TRUE(leResult.isBoolean());
    EXPECT_FALSE(leResult.asBoolean());
}

TEST_F(ExpressionVisitorTest, EvaluatesEqualityOperations) {
    // 5 == 5
    auto eqExpr = createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(5))),
                                   TokenType::EQUAL_EQUAL,
                                   createLiteralExpr(LiteralValue(static_cast<int64_t>(5))));

    Value eqResult = visitor.evaluate(*eqExpr);
    EXPECT_TRUE(eqResult.isBoolean());
    EXPECT_TRUE(eqResult.asBoolean());

    // 5 != 3
    auto neExpr = createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(5))),
                                   TokenType::BANG_EQUAL,
                                   createLiteralExpr(LiteralValue(static_cast<int64_t>(3))));

    Value neResult = visitor.evaluate(*neExpr);
    EXPECT_TRUE(neResult.isBoolean());
    EXPECT_TRUE(neResult.asBoolean());
}

TEST_F(ExpressionVisitorTest, EvaluatesLogicalOperations) {
    // true && true
    auto andExpr = createBinaryExpr(createLiteralExpr(LiteralValue(true)), TokenType::AND,
                                    createLiteralExpr(LiteralValue(true)));

    Value andResult = visitor.evaluate(*andExpr);
    EXPECT_TRUE(andResult.isBoolean());
    EXPECT_TRUE(andResult.asBoolean());

    // false || true
    auto orExpr = createBinaryExpr(createLiteralExpr(LiteralValue(false)), TokenType::OR,
                                   createLiteralExpr(LiteralValue(true)));

    Value orResult = visitor.evaluate(*orExpr);
    EXPECT_TRUE(orResult.isBoolean());
    EXPECT_TRUE(orResult.asBoolean());

    // Short-circuit evaluation: false && (error operation)
    auto shortCircuitAndExpr = createBinaryExpr(
        createLiteralExpr(LiteralValue(false)), TokenType::AND,
        createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(1))), TokenType::SLASH,
                         createLiteralExpr(LiteralValue(static_cast<int64_t>(0)))));

    Value shortCircuitAndResult = visitor.evaluate(*shortCircuitAndExpr);
    EXPECT_TRUE(shortCircuitAndResult.isBoolean());
    EXPECT_FALSE(shortCircuitAndResult.asBoolean());

    // Short-circuit evaluation: true || (error operation)
    auto shortCircuitOrExpr = createBinaryExpr(
        createLiteralExpr(LiteralValue(true)), TokenType::OR,
        createBinaryExpr(createLiteralExpr(LiteralValue(static_cast<int64_t>(1))), TokenType::SLASH,
                         createLiteralExpr(LiteralValue(static_cast<int64_t>(0)))));

    Value shortCircuitOrResult = visitor.evaluate(*shortCircuitOrExpr);
    EXPECT_TRUE(shortCircuitOrResult.isBoolean());
    EXPECT_TRUE(shortCircuitOrResult.asBoolean());
}

TEST_F(ExpressionVisitorTest, EvaluatesUnaryMinus) {
    // -5
    auto expr =
        createUnaryExpr(TokenType::MINUS, createLiteralExpr(LiteralValue(static_cast<int64_t>(5))));

    Value result = visitor.evaluate(*expr);
    EXPECT_TRUE(result.isInteger());
    EXPECT_EQ(result.asInteger(), -5);
}

TEST_F(ExpressionVisitorTest, EvaluatesLogicalNot) {
    // !true
    auto exprTrue = createUnaryExpr(TokenType::BANG, createLiteralExpr(LiteralValue(true)));

    Value resultTrue = visitor.evaluate(*exprTrue);
    EXPECT_TRUE(resultTrue.isBoolean());
    EXPECT_FALSE(resultTrue.asBoolean());

    // !false
    auto exprFalse = createUnaryExpr(TokenType::BANG, createLiteralExpr(LiteralValue(false)));

    Value resultFalse = visitor.evaluate(*exprFalse);
    EXPECT_TRUE(resultFalse.isBoolean());
    EXPECT_TRUE(resultFalse.asBoolean());
}

// More tests would be added for other expression types and complex scenarios