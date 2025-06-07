#ifndef CHRONOVYAN_INTERPRETER_H
#define CHRONOVYAN_INTERPRETER_H

#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "ast_nodes.h"
#include "custom_type_system.h"
#include "environment.h"
#include "resource_optimizer.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"
#include "user_type_system.h"
#include "value.h"

namespace chronovyan {

// Forward declarations
class ExpressionVisitor;
class StatementVisitor;
class TemporalVisitor;
class ResourceManager;

/**
 * @class Interpreter
 * @brief Interprets and executes Chronovyan AST nodes
 */
class Interpreter : public ASTVisitor {
public:
    /**
     * @brief Create a new interpreter
     */
    Interpreter();

    /**
     * @brief Destructor
     */
    ~Interpreter() override;

    /**
     * @brief Execute a program represented by an AST
     * @param statements The statements to execute
     * @return The result value
     */
    Value interpret(const ProgramNode& program);

    /**
     * @brief Execute a single expression
     * @param expr The expression to evaluate
     * @return The result value
     */
    void execute(const StmtNode& stmt);

    /**
     * @brief Execute a single statement
     * @param stmt The statement to execute
     */
    Value evaluate(const ExprNode& expr);

    /**
     * @brief Define a variable in the global environment
     * @param name The variable name
     * @param value The variable value
     */
    void defineGlobal(const std::string& name, const Value& value);

    /**
     * @brief Get the global environment
     * @return The global environment
     */
    std::shared_ptr<Environment> getGlobals() const;

    /**
     * @brief Get the current environment
     * @return The current environment
     */
    std::shared_ptr<Environment> getEnvironment() const;

    /**
     * @brief Set the current environment
     * @param environment The new environment
     */
    void setEnvironment(std::shared_ptr<Environment> environment);

    /**
     * @brief Get the temporal runtime
     * @return The temporal runtime
     */
    std::shared_ptr<TemporalRuntime> getRuntime() const;

    /**
     * @brief Get the resource optimizer
     * @return The resource optimizer
     */
    std::shared_ptr<ResourceOptimizer> getResourceOptimizer() const;

    /**
     * @brief Get the temporal debt tracker
     * @return The temporal debt tracker
     */
    std::shared_ptr<TemporalDebtTracker> getTemporalDebtTracker() const;

    /**
     * @brief Get the resource visualization component
     * @return The resource visualization component
     */
    std::shared_ptr<ResourceVisualization> getResourceVisualization() const;

    /**
     * @brief Reset the interpreter state
     */
    void reset();

    /**
     * @brief Get the custom type system
     */
    std::shared_ptr<CustomTypeSystem> getTypeSystem() const;

    /**
     * @brief Get the user type system
     */
    std::shared_ptr<UserTypeSystem> getUserTypeSystem() const;

    /**
     * @brief Get the last value evaluated
     */
    Value getLastValue() const;

    /**
     * @brief Track resource usage
     * @param aethel The amount of aethel used
     * @param chronons The amount of chronons used
     * @param operation The operation description
     */
    void trackResourceUsage(double aethel, double chronons, const std::string& operation);

    /**
     * @brief Consume resources
     * @param amount The amount to consume
     * @return True if sufficient resources were available
     */
    bool consumeResources(double amount);

    /**
     * @brief Handle variable interaction (CONF/REB)
     * @param left The left operand
     * @param right The right operand
     * @param operation The operation type
     * @return The result of the interaction
     */
    Value handleVariableInteraction(const Value& left, const Value& right, TokenType operation);

    /**
     * @brief Update paradox level based on variable interaction
     * @param left The left operand
     * @param right The right operand
     * @param operation The operation type
     */
    void updateParadoxLevel(const Value& left, const Value& right, TokenType operation);

    // Implementation of visitor methods for expressions
    void visitLiteralExpr(const LiteralExprNode& expr) override;
    void visitVariableExpr(const VariableExprNode& expr) override;
    void visitUnaryExpr(const UnaryExprNode& expr) override;
    void visitBinaryExpr(const BinaryExprNode& expr) override;
    void visitGroupingExpr(const GroupingExprNode& expr) override;
    void visitAssignExpr(const AssignExprNode& expr) override;
    void visitCallExpr(const CallExprNode& expr) override;

    // Implementation of visitor methods for statements
    void visitBlockStmt(const BlockStmtNode& stmt) override;
    void visitBreakChrononStmt(const BreakChrononStmtNode& stmt) override;
    void visitContinueWeaveStmt(const ContinueWeaveStmtNode& stmt) override;
    void visitExprStmt(const ExprStmtNode& stmt) override;
    void visitForChrononStmt(const ForChrononStmtNode& stmt) override;
    void visitFunctionDeclStmt(const FunctionDeclStmtNode& stmt) override;
    void visitIfStmt(const IfStmtNode& stmt) override;
    void visitReturnStmt(const ReturnStmtNode& stmt) override;
    void visitTemporalOpStmt(const TemporalOpStmtNode& stmt) override;
    void visitVariableDeclStmt(const VariableDeclStmtNode& stmt) override;
    void visitWhileEventStmt(const WhileEventStmtNode& stmt) override;
    void visitTemporalSection(const TemporalSectionNode& stmt) override;
    void visitTypeDefStmt(const TypeDefStmtNode& stmt) override;
    void visitRewindFlowStmt(const RewindFlowStmtNode& stmt) override;
    void visitReverseFlowStmt(const ReverseFlowStmtNode& stmt) override;
    void visitTemporalLoopStmt(const TemporalLoopStmtNode& stmt) override;
    void visitHaltRewindStmt(const HaltRewindStmtNode& stmt) override;
    void visitStabilizeTimelineStmt(const StabilizeTimelineStmtNode& stmt) override;
    void visitPreventModificationStmt(const PreventModificationStmtNode& stmt) override;

    // Implementation of visitor methods for other nodes
    void visitProgram(const ProgramNode& program) override;
    void visitTemporalProgram(const TemporalProgramNode& program) override;
    void visitExpendResourcesStmt(const ExpendResourcesStmtNode& stmt) override;
    void visitResourceInsufficiencyHandler(
        const ResourceInsufficiencyHandlerNode& handler) override;
    void visitType(const TypeNode& type) override;
    void visitAttemptWeaveStmt(const AttemptWeaveStmtNode& node) override;
    void visitDissonanceHandler(const DissonanceHandlerNode& node) override;

private:
    std::shared_ptr<Environment> m_globals;
    std::shared_ptr<Environment> m_environment;
    std::shared_ptr<TemporalRuntime> m_runtime;
    std::shared_ptr<ResourceOptimizer> m_optimizer;
    std::shared_ptr<TemporalDebtTracker> m_debt_tracker;
    std::shared_ptr<ResourceVisualization> m_visualization;
    std::shared_ptr<CustomTypeSystem> m_typeSystem;
    std::shared_ptr<UserTypeSystem> m_userTypeSystem;
    Value m_lastValue;  // Result of the last expression evaluated

    // New visitor components
    std::unique_ptr<ExpressionVisitor> m_expressionVisitor;
    std::unique_ptr<StatementVisitor> m_statementVisitor;
    std::unique_ptr<TemporalVisitor> m_temporalVisitor;
    std::unique_ptr<ResourceManager> m_resourceManager;

    // Native function definitions
    void defineNativeFunctions();
    void defineTemporalFunctions();
    void defineTypeNativeFunctions();
    void defineUserTypeNativeFunctions();
    void initializeGlobals();
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_INTERPRETER_H