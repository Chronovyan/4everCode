#include <cmath>
#include <functional>

#include "callable.h"
#include "environment.h"
#include "error_handler.h"
#include "interpreter.h"
#include "interpreter/ExpressionVisitor.h"
#include "native_function.h"
#include "temporal_runtime.h"

namespace chronovyan {

ExpressionVisitor::ExpressionVisitor(Interpreter& interpreter) : m_interpreter(interpreter) {
    initializeBinaryOps();
}

Value ExpressionVisitor::evaluate(const ExprNode& expr) {
    // Use the visitor pattern to evaluate the expression
    expr.accept(*this);
    return m_lastValue;
}

Value ExpressionVisitor::getLastValue() const { return m_lastValue; }

void ExpressionVisitor::initializeBinaryOps() {
    // Arithmetic operations
    m_binaryOps[TokenType::PLUS] = [this](const Value& left, const Value& right) -> Value {
        // Handle string concatenation
        if (left.isString() && right.isString()) {
            return Value(left.asString() + right.asString());
        }

        // Handle numeric addition
        if ((left.isNumber() || left.isInteger()) && (right.isNumber() || right.isInteger())) {
            // Check if either is a float
            if (left.isNumber() || right.isNumber()) {
                return Value(getNumericValue(left) + getNumericValue(right));
            } else {
                return Value(left.asInteger() + right.asInteger());
            }
        }

        // Handle temporal interactions (CONF + REB)
        if (left.hasFlaggedMetadata("CONF") && right.hasFlaggedMetadata("REB")) {
            return handleVariableInteraction(left, right, TokenType::PLUS);
        }

        // Type error
        throw RuntimeError("Operands must be numbers or strings for addition.",
                           left.getSourceLocation());
    };

    m_binaryOps[TokenType::MINUS] = [this](const Value& left, const Value& right) -> Value {
        if ((left.isNumber() || left.isInteger()) && (right.isNumber() || right.isInteger())) {
            // Check if either is a float
            if (left.isNumber() || right.isNumber()) {
                return Value(getNumericValue(left) - getNumericValue(right));
            } else {
                return Value(left.asInteger() - right.asInteger());
            }
        }

        // Handle temporal interactions (CONF - REB)
        if (left.hasFlaggedMetadata("CONF") && right.hasFlaggedMetadata("REB")) {
            return handleVariableInteraction(left, right, TokenType::MINUS);
        }

        throw RuntimeError("Operands must be numbers for subtraction.", left.getSourceLocation());
    };

    m_binaryOps[TokenType::STAR] = [this](const Value& left, const Value& right) -> Value {
        if ((left.isNumber() || left.isInteger()) && (right.isNumber() || right.isInteger())) {
            // Check if either is a float
            if (left.isNumber() || right.isNumber()) {
                return Value(getNumericValue(left) * getNumericValue(right));
            } else {
                return Value(left.asInteger() * right.asInteger());
            }
        }

        // Handle temporal interactions (CONF * REB)
        if (left.hasFlaggedMetadata("CONF") && right.hasFlaggedMetadata("REB")) {
            return handleVariableInteraction(left, right, TokenType::STAR);
        }

        throw RuntimeError("Operands must be numbers for multiplication.",
                           left.getSourceLocation());
    };

    m_binaryOps[TokenType::SLASH] = [this](const Value& left, const Value& right) -> Value {
        if ((left.isNumber() || left.isInteger()) && (right.isNumber() || right.isInteger())) {
            double rightValue = getNumericValue(right);

            if (std::abs(rightValue) < 1e-10) {
                throw RuntimeError("Division by zero.", right.getSourceLocation());
            }

            // Division always returns a float
            return Value(getNumericValue(left) / rightValue);
        }

        // Handle temporal interactions (CONF / REB)
        if (left.hasFlaggedMetadata("CONF") && right.hasFlaggedMetadata("REB")) {
            return handleVariableInteraction(left, right, TokenType::SLASH);
        }

        throw RuntimeError("Operands must be numbers for division.", left.getSourceLocation());
    };

    // Comparison operations
    m_binaryOps[TokenType::GREATER] = [this](const Value& left, const Value& right) -> Value {
        if ((left.isNumber() || left.isInteger()) && (right.isNumber() || right.isInteger())) {
            return Value(getNumericValue(left) > getNumericValue(right));
        }

        if (left.isString() && right.isString()) {
            return Value(left.asString() > right.asString());
        }

        throw RuntimeError("Operands must be comparable types for greater than.",
                           left.getSourceLocation());
    };

    m_binaryOps[TokenType::GREATER_EQUAL] = [this](const Value& left, const Value& right) -> Value {
        if ((left.isNumber() || left.isInteger()) && (right.isNumber() || right.isInteger())) {
            return Value(getNumericValue(left) >= getNumericValue(right));
        }

        if (left.isString() && right.isString()) {
            return Value(left.asString() >= right.asString());
        }

        throw RuntimeError("Operands must be comparable types for greater than or equal.",
                           left.getSourceLocation());
    };

    m_binaryOps[TokenType::LESS] = [this](const Value& left, const Value& right) -> Value {
        if ((left.isNumber() || left.isInteger()) && (right.isNumber() || right.isInteger())) {
            return Value(getNumericValue(left) < getNumericValue(right));
        }

        if (left.isString() && right.isString()) {
            return Value(left.asString() < right.asString());
        }

        throw RuntimeError("Operands must be comparable types for less than.",
                           left.getSourceLocation());
    };

    m_binaryOps[TokenType::LESS_EQUAL] = [this](const Value& left, const Value& right) -> Value {
        if ((left.isNumber() || left.isInteger()) && (right.isNumber() || right.isInteger())) {
            return Value(getNumericValue(left) <= getNumericValue(right));
        }

        if (left.isString() && right.isString()) {
            return Value(left.asString() <= right.asString());
        }

        throw RuntimeError("Operands must be comparable types for less than or equal.",
                           left.getSourceLocation());
    };

    // Equality operations
    m_binaryOps[TokenType::EQUAL_EQUAL] = [](const Value& left, const Value& right) -> Value {
        return Value(left == right);
    };

    m_binaryOps[TokenType::BANG_EQUAL] = [](const Value& left, const Value& right) -> Value {
        return Value(left != right);
    };

    // Logical operations (implemented in visitBinaryExpr for short-circuit behavior)
}

void ExpressionVisitor::visitLiteralExpr(const LiteralExprNode& expr) {
    m_lastValue = Value(expr.getValue());
}

void ExpressionVisitor::visitVariableExpr(const VariableExprNode& expr) {
    m_lastValue = lookUpVariable(expr.getName(), expr.getLocation());
}

void ExpressionVisitor::visitGroupingExpr(const GroupingExprNode& expr) {
    // Simply evaluate the contained expression
    m_lastValue = evaluate(expr.getExpression());
}

void ExpressionVisitor::visitUnaryExpr(const UnaryExprNode& expr) {
    Value right = evaluate(expr.getRight());

    switch (expr.getOperator().type) {
        case TokenType::MINUS:
            // Ensure operand is a number
            if (right.isNumber()) {
                m_lastValue = Value(-right.asNumber());
            } else if (right.isInteger()) {
                m_lastValue = Value(-right.asInteger());
            } else {
                throw RuntimeError("Operand must be a number for unary minus.", expr.getLocation());
            }
            break;

        case TokenType::BANG:
            // Logical not - invert the truthiness of the value
            m_lastValue = Value(!right.isTruthy());
            break;

        case TokenType::PLUS_PLUS:
            // Pre-increment
            if (right.isNumber()) {
                m_lastValue = Value(right.asNumber() + 1.0);
            } else if (right.isInteger()) {
                m_lastValue = Value(right.asInteger() + 1);
            } else {
                throw RuntimeError("Operand must be a number for increment.", expr.getLocation());
            }

            // If this is a variable, update its value
            if (expr.getRight().isVariable()) {
                const auto& varExpr = static_cast<const VariableExprNode&>(expr.getRight());
                m_interpreter.getEnvironment()->assign(varExpr.getName(), m_lastValue);
            }
            break;

        case TokenType::MINUS_MINUS:
            // Pre-decrement
            if (right.isNumber()) {
                m_lastValue = Value(right.asNumber() - 1.0);
            } else if (right.isInteger()) {
                m_lastValue = Value(right.asInteger() - 1);
            } else {
                throw RuntimeError("Operand must be a number for decrement.", expr.getLocation());
            }

            // If this is a variable, update its value
            if (expr.getRight().isVariable()) {
                const auto& varExpr = static_cast<const VariableExprNode&>(expr.getRight());
                m_interpreter.getEnvironment()->assign(varExpr.getName(), m_lastValue);
            }
            break;

        default:
            // Unrecognized operator
            throw RuntimeError("Unknown unary operator: " +
                                   tokenTypeToString(expr.getOperator().type),
                               expr.getLocation());
    }
}

void ExpressionVisitor::visitBinaryExpr(const BinaryExprNode& expr) {
    // Special handling for logical operators for short-circuit evaluation
    if (expr.getOperator().type == TokenType::AND) {
        Value left = evaluate(expr.getLeft());

        // Short circuit if left is false
        if (!left.isTruthy()) {
            m_lastValue = left;
            return;
        }

        // Otherwise, result is the right value
        m_lastValue = evaluate(expr.getRight());
        return;
    }

    if (expr.getOperator().type == TokenType::OR) {
        Value left = evaluate(expr.getLeft());

        // Short circuit if left is true
        if (left.isTruthy()) {
            m_lastValue = left;
            return;
        }

        // Otherwise, result is the right value
        m_lastValue = evaluate(expr.getRight());
        return;
    }

    // For other operators, evaluate both sides
    Value left = evaluate(expr.getLeft());
    Value right = evaluate(expr.getRight());

    // Find and apply the appropriate binary operation
    auto it = m_binaryOps.find(expr.getOperator().type);
    if (it != m_binaryOps.end()) {
        try {
            m_lastValue = it->second(left, right);

            // Update paradox level for operations between CONF and REB
            if (left.hasFlaggedMetadata("CONF") && right.hasFlaggedMetadata("REB")) {
                updateParadoxLevel(left, right, expr.getOperator().type);
            }
        } catch (const RuntimeError& e) {
            // Add operation details to the error
            throw RuntimeError(e.what() + std::string(" Operator: ") +
                                   tokenTypeToString(expr.getOperator().type),
                               expr.getLocation());
        }
    } else {
        throw RuntimeError("Unknown binary operator: " + tokenTypeToString(expr.getOperator().type),
                           expr.getLocation());
    }
}

void ExpressionVisitor::visitAssignExpr(const AssignExprNode& expr) {
    Value value = evaluate(expr.getValue());

    // Get the current value if we need it for compound assignments
    Value currentValue;
    bool isCompound = false;

    switch (expr.getOperator().type) {
        case TokenType::PLUS_EQUAL:
        case TokenType::MINUS_EQUAL:
        case TokenType::STAR_EQUAL:
        case TokenType::SLASH_EQUAL:
            currentValue = lookUpVariable(expr.getName(), expr.getLocation());
            isCompound = true;
            break;

        default:
            // Simple assignment
            break;
    }

    // Handle compound assignments
    if (isCompound) {
        switch (expr.getOperator().type) {
            case TokenType::PLUS_EQUAL: {
                auto it = m_binaryOps.find(TokenType::PLUS);
                if (it != m_binaryOps.end()) {
                    value = it->second(currentValue, value);
                }
                break;
            }
            case TokenType::MINUS_EQUAL: {
                auto it = m_binaryOps.find(TokenType::MINUS);
                if (it != m_binaryOps.end()) {
                    value = it->second(currentValue, value);
                }
                break;
            }
            case TokenType::STAR_EQUAL: {
                auto it = m_binaryOps.find(TokenType::STAR);
                if (it != m_binaryOps.end()) {
                    value = it->second(currentValue, value);
                }
                break;
            }
            case TokenType::SLASH_EQUAL: {
                auto it = m_binaryOps.find(TokenType::SLASH);
                if (it != m_binaryOps.end()) {
                    value = it->second(currentValue, value);
                }
                break;
            }
            default:
                // Should never reach here due to the switch above
                break;
        }
    }

    // Assign the value to the variable
    m_interpreter.getEnvironment()->assign(expr.getName(), value);
    m_lastValue = value;
}

void ExpressionVisitor::visitCallExpr(const CallExprNode& expr) {
    // Evaluate the callee
    Value callee = evaluate(expr.getCallee());

    // Evaluate the arguments
    std::vector<Value> arguments;
    for (const auto& argument : expr.getArguments()) {
        arguments.push_back(evaluate(*argument));
    }

    // Ensure callee is callable
    if (!callee.isCallable()) {
        throw RuntimeError("Can only call functions and classes.", expr.getLocation());
    }

    // Get the callable object
    auto callable = callee.asCallable();

    // Check arity
    if (callable->arity() != -1 && arguments.size() != callable->arity()) {
        throw RuntimeError("Expected " + std::to_string(callable->arity()) + " arguments but got " +
                               std::to_string(arguments.size()) + ".",
                           expr.getLocation());
    }

    // Call the function
    m_lastValue = callable->call(m_interpreter, arguments);
}

Value ExpressionVisitor::lookUpVariable(const std::string& name, const SourceLocation& location) {
    try {
        return m_interpreter.getEnvironment()->get(name);
    } catch (const RuntimeError& e) {
        // Try globals if not found in current environment
        try {
            return m_interpreter.getGlobals()->get(name);
        } catch (const RuntimeError&) {
            // Re-throw the original error with better location information
            throw RuntimeError("Undefined variable '" + name + "'.", location);
        }
    }
}

Value ExpressionVisitor::handleVariableInteraction(const Value& left, const Value& right,
                                                   TokenType operation) {
    // Delegate to the interpreter's implementation
    return m_interpreter.handleVariableInteraction(left, right, operation);
}

void ExpressionVisitor::updateParadoxLevel(const Value& left, const Value& right,
                                           TokenType operation) {
    // Delegate to the interpreter's implementation
    m_interpreter.updateParadoxLevel(left, right, operation);
}

double ExpressionVisitor::getNumericValue(const Value& value) {
    if (value.isNumber()) {
        return value.asNumber();
    } else if (value.isInteger()) {
        return static_cast<double>(value.asInteger());
    }

    throw RuntimeError("Expected a number value.", value.getSourceLocation());
}

}  // namespace chronovyan