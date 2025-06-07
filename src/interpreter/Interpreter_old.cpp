#include <iostream>

#include "error_handler.h"
#include "interpreter/ExpressionVisitor.h"
#include "interpreter/Interpreter.h"
#include "interpreter/ResourceManager.h"
#include "interpreter/StatementVisitor.h"
#include "interpreter/TemporalVisitor.h"
#include "native_function.h"

namespace chronovyan {

Interpreter::Interpreter()
    : m_globals(std::make_shared<Environment>(nullptr))
    , m_environment(m_globals)
    , m_runtime(std::make_shared<TemporalRuntime>())
    , m_optimizer(std::make_shared<ResourceOptimizer>(m_runtime, nullptr))
    , m_debt_tracker(std::make_shared<TemporalDebtTracker>())
    , m_visualization(std::make_shared<ResourceVisualization>())
    , m_typeSystem(std::make_shared<CustomTypeSystem>())
    , m_userTypeSystem(std::make_shared<UserTypeSystem>()) {
    // Initialize resource manager
    m_resourceManager = std::make_unique<ResourceManager>(m_runtime, m_optimizer, m_debt_tracker);

    // Initialize visitor components
    m_expressionVisitor = std::make_unique<ExpressionVisitor>(*this);
    m_statementVisitor = std::make_unique<StatementVisitor>(*this);
    m_temporalVisitor = std::make_unique<TemporalVisitor>(*this);

    // Set the debt tracker in the optimizer
    m_optimizer->setTemporalDebtTracker(m_debt_tracker);

    // Define native functions and initialize globals
    defineNativeFunctions();
    defineTemporalFunctions();
    defineTypeNativeFunctions();
    defineUserTypeNativeFunctions();
    initializeGlobals();
}

Interpreter::~Interpreter() = default;

Value Interpreter::interpret(const ProgramNode& program) {
    try {
        // Visit the program
        visitProgram(program);
        return m_lastValue;
    } catch (const RuntimeError& error) {
        // Report the error
        ErrorHandler::getInstance().reportError(error.getLocation(), error.what());
        return Value();
    }
}

void Interpreter::execute(const StmtNode& stmt) {
    // Delegate to statement visitor
    m_statementVisitor->execute(stmt);
}

Value Interpreter::evaluate(const ExprNode& expr) {
    // Delegate to expression visitor
    Value result = m_expressionVisitor->evaluate(expr);
    m_lastValue = result;
    return result;
}

void Interpreter::defineGlobal(const std::string& name, const Value& value) {
    m_globals->define(name, value);
}

std::shared_ptr<Environment> Interpreter::getGlobals() const { return m_globals; }

std::shared_ptr<Environment> Interpreter::getEnvironment() const { return m_environment; }

void Interpreter::setEnvironment(std::shared_ptr<Environment> environment) {
    m_environment = environment;
}

std::shared_ptr<TemporalRuntime> Interpreter::getRuntime() const { return m_runtime; }

std::shared_ptr<ResourceOptimizer> Interpreter::getResourceOptimizer() const { return m_optimizer; }

std::shared_ptr<TemporalDebtTracker> Interpreter::getTemporalDebtTracker() const {
    return m_debt_tracker;
}

std::shared_ptr<ResourceVisualization> Interpreter::getResourceVisualization() const {
    return m_visualization;
}

void Interpreter::reset() {
    // Reset all components
    m_environment = m_globals;
    m_runtime->reset();
    m_lastValue = Value();

    // Reset control flow state in visitors
    // No need to reset expression visitor as it has no persistent state
    m_statementVisitor = std::make_unique<StatementVisitor>(*this);
    m_temporalVisitor = std::make_unique<TemporalVisitor>(*this);
}

std::shared_ptr<CustomTypeSystem> Interpreter::getTypeSystem() const { return m_typeSystem; }

std::shared_ptr<UserTypeSystem> Interpreter::getUserTypeSystem() const { return m_userTypeSystem; }

Value Interpreter::getLastValue() const { return m_lastValue; }

void Interpreter::trackResourceUsage(double aethel, double chronons, const std::string& operation) {
    // Delegate to resource manager
    m_resourceManager->trackResourceUsage(aethel, chronons, operation);
}

bool Interpreter::consumeResources(double amount) {
    // Split the amount between aethel and chronons
    double aethelAmount = amount * 0.6;
    double chrononsAmount = amount * 0.4;

    // Delegate to resource manager
    return m_resourceManager->consumeResources(aethelAmount, chrononsAmount, "Generic operation");
}

Value Interpreter::handleVariableInteraction(const Value& left, const Value& right,
                                             TokenType operation) {
    // This is a complex behavior that might be specific to the interpreter
    // For now, implement it directly in the interpreter

    // Increase paradox level
    m_runtime->increaseParadoxLevel(1);

    // Different operations have different interactions
    switch (operation) {
        case TokenType::PLUS: {
            // CONF + REB: Amplification effect
            if (left.isNumber() && right.isNumber()) {
                double result = left.asNumber() * 1.5 + right.asNumber() * 1.5;
                return Value(result);
            } else if (left.isInteger() && right.isInteger()) {
                int64_t result = left.asInteger() * 2 + right.asInteger() * 2;
                return Value(result);
            }
            break;
        }

        case TokenType::MINUS: {
            // CONF - REB: Cancellation effect
            if (left.isNumber() && right.isNumber()) {
                double result = left.asNumber() - right.asNumber() * 0.5;
                return Value(result);
            } else if (left.isInteger() && right.isInteger()) {
                int64_t result = left.asInteger() - right.asInteger() / 2;
                return Value(result);
            }
            break;
        }

        case TokenType::STAR: {
            // CONF * REB: Unstable multiplication
            if (left.isNumber() && right.isNumber()) {
                double result = left.asNumber() * right.asNumber();
                // Add some instability based on paradox level
                double instability = m_runtime->getParadoxLevel() / 10.0;
                result *= (1.0 + instability);
                return Value(result);
            } else if (left.isInteger() && right.isInteger()) {
                int64_t result = left.asInteger() * right.asInteger();
                // Add some instability based on paradox level
                double instability = m_runtime->getParadoxLevel() / 10.0;
                result = static_cast<int64_t>(result * (1.0 + instability));
                return Value(result);
            }
            break;
        }

        case TokenType::SLASH: {
            // CONF / REB: Unpredictable division
            if (left.isNumber() && right.isNumber()) {
                double rightVal = right.asNumber();
                if (std::abs(rightVal) < 1e-10) {
                    throw RuntimeError("Division by zero in CONF/REB interaction.",
                                       left.getSourceLocation());
                }

                double result = left.asNumber() / rightVal;
                // Add some unpredictability based on paradox level
                double unpredictability = m_runtime->getParadoxLevel() / 20.0;
                result *= (1.0 + unpredictability * (std::rand() % 2 == 0 ? 1 : -1));
                return Value(result);
            }
            break;
        }

        default:
            // For other operations, just return the left value
            return left;
    }

    // Default case: return the left value
    return left;
}

void Interpreter::updateParadoxLevel(const Value& left, const Value& right, TokenType operation) {
    // Different operations affect paradox level differently
    int increase = 1;

    switch (operation) {
        case TokenType::PLUS:
            increase = 1;
            break;

        case TokenType::MINUS:
            increase = 2;
            break;

        case TokenType::STAR:
            increase = 3;
            break;

        case TokenType::SLASH:
            increase = 4;
            break;

        default:
            increase = 1;
            break;
    }

    // Increase paradox level
    m_runtime->increaseParadoxLevel(increase);
}

// Implementation of visitor methods for expressions
void Interpreter::visitLiteralExpr(const LiteralExprNode& expr) {
    m_expressionVisitor->visitLiteralExpr(expr);
    m_lastValue = m_expressionVisitor->getLastValue();
}

void Interpreter::visitVariableExpr(const VariableExprNode& expr) {
    m_expressionVisitor->visitVariableExpr(expr);
    m_lastValue = m_expressionVisitor->getLastValue();
}

void Interpreter::visitUnaryExpr(const UnaryExprNode& expr) {
    m_expressionVisitor->visitUnaryExpr(expr);
    m_lastValue = m_expressionVisitor->getLastValue();
}

void Interpreter::visitBinaryExpr(const BinaryExprNode& expr) {
    m_expressionVisitor->visitBinaryExpr(expr);
    m_lastValue = m_expressionVisitor->getLastValue();
}

void Interpreter::visitGroupingExpr(const GroupingExprNode& expr) {
    m_expressionVisitor->visitGroupingExpr(expr);
    m_lastValue = m_expressionVisitor->getLastValue();
}

void Interpreter::visitAssignExpr(const AssignExprNode& expr) {
    m_expressionVisitor->visitAssignExpr(expr);
    m_lastValue = m_expressionVisitor->getLastValue();
}

void Interpreter::visitCallExpr(const CallExprNode& expr) {
    m_expressionVisitor->visitCallExpr(expr);
    m_lastValue = m_expressionVisitor->getLastValue();
}

// Implementation of visitor methods for statements
void Interpreter::visitBlockStmt(const BlockStmtNode& stmt) {
    m_statementVisitor->visitBlockStmt(stmt);
}

void Interpreter::visitBreakChrononStmt(const BreakChrononStmtNode& stmt) {
    m_temporalVisitor->visitBreakChrononStmt(stmt);
}

void Interpreter::visitContinueWeaveStmt(const ContinueWeaveStmtNode& stmt) {
    m_temporalVisitor->visitContinueWeaveStmt(stmt);
}

void Interpreter::visitExprStmt(const ExprStmtNode& stmt) {
    m_statementVisitor->visitExprStmt(stmt);
}

void Interpreter::visitForChrononStmt(const ForChrononStmtNode& stmt) {
    m_temporalVisitor->visitForChrononStmt(stmt);
}

void Interpreter::visitFunctionDeclStmt(const FunctionDeclStmtNode& stmt) {
    m_statementVisitor->visitFunctionDeclStmt(stmt);
}

void Interpreter::visitIfStmt(const IfStmtNode& stmt) { m_statementVisitor->visitIfStmt(stmt); }

void Interpreter::visitReturnStmt(const ReturnStmtNode& stmt) {
    m_statementVisitor->visitReturnStmt(stmt);
}

void Interpreter::visitTemporalOpStmt(const TemporalOpStmtNode& stmt) {
    m_temporalVisitor->visitTemporalOpStmt(stmt);
}

void Interpreter::visitVariableDeclStmt(const VariableDeclStmtNode& stmt) {
    m_statementVisitor->visitVariableDeclStmt(stmt);
}

void Interpreter::visitWhileEventStmt(const WhileEventStmtNode& stmt) {
    m_temporalVisitor->visitWhileEventStmt(stmt);
}

void Interpreter::visitTemporalSection(const TemporalSectionNode& stmt) {
    m_temporalVisitor->visitTemporalSection(stmt);
}

void Interpreter::visitTypeDefStmt(const TypeDefStmtNode& stmt) {
    m_statementVisitor->visitTypeDefStmt(stmt);
}

void Interpreter::visitRewindFlowStmt(const RewindFlowStmtNode& stmt) {
    m_temporalVisitor->visitRewindFlowStmt(stmt);
}

void Interpreter::visitReverseFlowStmt(const ReverseFlowStmtNode& stmt) {
    m_temporalVisitor->visitReverseFlowStmt(stmt);
}

void Interpreter::visitTemporalLoopStmt(const TemporalLoopStmtNode& stmt) {
    m_temporalVisitor->visitTemporalLoopStmt(stmt);
}

void Interpreter::visitHaltRewindStmt(const HaltRewindStmtNode& stmt) {
    m_temporalVisitor->visitHaltRewindStmt(stmt);
}

void Interpreter::visitStabilizeTimelineStmt(const StabilizeTimelineStmtNode& stmt) {
    m_temporalVisitor->visitStabilizeTimelineStmt(stmt);
}

void Interpreter::visitPreventModificationStmt(const PreventModificationStmtNode& stmt) {
    m_temporalVisitor->visitPreventModificationStmt(stmt);
}

// Implementation of visitor methods for other nodes
void Interpreter::visitProgram(const ProgramNode& program) {
    // Execute each statement in the program
    for (const auto& stmt : program.getStatements()) {
        execute(*stmt);
    }
}

void Interpreter::visitTemporalProgram(const TemporalProgramNode& program) {
    m_temporalVisitor->visitTemporalProgram(program);
}

void Interpreter::visitExpendResourcesStmt(const ExpendResourcesStmtNode& stmt) {
    m_temporalVisitor->visitExpendResourcesStmt(stmt);
}

void Interpreter::visitResourceInsufficiencyHandler(
    const ResourceInsufficiencyHandlerNode& handler) {
    m_temporalVisitor->visitResourceInsufficiencyHandler(handler);
}

void Interpreter::visitType(const TypeNode& type) {
    // This is a simple pass-through that doesn't need delegation
    // Type nodes are typically evaluated in the context of other operations
}

void Interpreter::visitAttemptWeaveStmt(const AttemptWeaveStmtNode& node) {
    // TODO: Implement try-catch mechanism
}

void Interpreter::visitDissonanceHandler(const DissonanceHandlerNode& node) {
    // TODO: Implement exception handler
}

// Native function definitions - these would be implemented as needed,
// similar to the original Interpreter class

void Interpreter::defineNativeFunctions() {
    // TODO: Define native functions
}

void Interpreter::defineTemporalFunctions() {
    // TODO: Define temporal functions
}

void Interpreter::defineTypeNativeFunctions() {
    // TODO: Define type-related native functions
}

void Interpreter::defineUserTypeNativeFunctions() {
    // TODO: Define user type-related native functions
}

void Interpreter::initializeGlobals() {
    // TODO: Initialize global variables and constants
}

}  // namespace chronovyan