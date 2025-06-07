#ifndef CHRONOVYAN_EXPRESSION_VISITOR_H
#define CHRONOVYAN_EXPRESSION_VISITOR_H

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

#include "ast_nodes.h"
#include "value.h"

namespace chronovyan {

// Forward declarations
class Interpreter;
class Environment;
class TemporalRuntime;
class ResourceOptimizer;

/**
 * @class ExpressionVisitor
 * @brief Handles the evaluation of expressions in the AST
 */
class ExpressionVisitor {
public:
    /**
     * @brief Create a new expression visitor
     * @param interpreter Reference to the parent interpreter
     */
    explicit ExpressionVisitor(Interpreter& interpreter);

    /**
     * @brief Evaluate an expression
     * @param expr The expression to evaluate
     * @return The resulting value
     */
    Value evaluate(const ExprNode& expr);

    // Expression visitor methods
    void visitLiteralExpr(const LiteralExprNode& expr);
    void visitVariableExpr(const VariableExprNode& expr);
    void visitUnaryExpr(const UnaryExprNode& expr);
    void visitBinaryExpr(const BinaryExprNode& expr);
    void visitGroupingExpr(const GroupingExprNode& expr);
    void visitAssignExpr(const AssignExprNode& expr);
    void visitCallExpr(const CallExprNode& expr);

    /**
     * @brief Get the last evaluated value
     * @return The last value calculated by the visitor
     */
    Value getLastValue() const;

private:
    Interpreter& m_interpreter;
    Value m_lastValue;

    // Helper methods
    Value lookUpVariable(const std::string& name, const SourceLocation& location);
    Value handleVariableInteraction(const Value& left, const Value& right, TokenType operation);
    void updateParadoxLevel(const Value& left, const Value& right, TokenType operation);
    double getNumericValue(const Value& value);

    // Helper for binary operations
    using BinaryOperation = std::function<Value(const Value&, const Value&)>;
    std::unordered_map<TokenType, BinaryOperation> m_binaryOps;

    // Initialize binary operations map
    void initializeBinaryOps();
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_EXPRESSION_VISITOR_H