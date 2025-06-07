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
#include "loot_system.h"
#include "resource_optimizer.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"
#include "user_type_system.h"
#include "value.h"

namespace chronovyan {

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
     * @brief Get the loot system
     */
    std::shared_ptr<LootSystem> getLootSystem() const;

    /**
     * @brief Get the last value evaluated
     */
    Value getLastValue() const;

    // Implementation of visitor methods for expressions
    void visitAssignExpr(const AssignExprNode& expr) override;
    void visitBinaryExpr(const BinaryExprNode& expr) override;
    void visitCallExpr(const CallExprNode& expr) override;
    void visitGroupingExpr(const GroupingExprNode& expr) override;
    void visitLiteralExpr(const LiteralExprNode& expr) override;
    void visitUnaryExpr(const UnaryExprNode& expr) override;
    void visitVariableExpr(const VariableExprNode& expr) override;

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

    // Visitor methods for TEMPORAL_LOOP and PARALLEL_EXECUTION
    void visitParallelExecutionStmt(const ParallelExecutionStmtNode& node) override;

    // Visitor method for BRANCH_TIMELINE
    void visitBranchTimelineStmt(const BranchTimelineStmtNode& node) override;

    // Helper methods for temporal program sections
    void executeTemporalLoop(const TemporalSectionNode& section);
    void registerEventHandler(const TemporalSectionNode& section);

private:
    std::shared_ptr<Environment> m_globals;
    std::shared_ptr<Environment> m_environment;
    std::shared_ptr<TemporalRuntime> m_runtime;
    std::shared_ptr<ResourceOptimizer> m_optimizer;
    std::shared_ptr<TemporalDebtTracker> m_debt_tracker;
    std::shared_ptr<ResourceVisualization> m_visualization;
    std::shared_ptr<CustomTypeSystem> m_typeSystem;
    std::shared_ptr<UserTypeSystem> m_userTypeSystem;
    std::shared_ptr<LootSystem> m_lootSystem;
    Value m_lastValue;  // Result of the last expression evaluated

    // For handling return statements, breaks, and continues
    std::stack<Value> m_returnValues;
    bool m_isReturning = false;
    bool m_isBreaking = false;
    bool m_isContinuing = false;

    // For handling temporal control flow
    bool m_breakChronon_flag = false;
    bool m_continueWeave_flag = false;
    bool m_reverseFlow_flag = false;
    bool m_haltRewind_flag = false;
    bool m_preventModification_flag = false;
    bool m_readOnlyMode = false;
    double m_stabilizationFactor = 0.0;
    int m_flowDirection = 1;

    // Resource management
    double m_temporalResources;
    double m_computationalResources;
    double m_aethelAvailable = 1000.0;    // Added for resource tracking
    double m_chrononsAvailable = 1000.0;  // Added for resource tracking
    bool m_debugLogging = false;          // Added for debugging control mechanisms
    bool m_debugMode = false;             // Added for compatibility with existing code

    // Event handlers for temporal sections
    std::map<std::string, const TemporalSectionNode*> m_eventHandlers;

    // Parallel execution state
    bool m_inParallelExecution;
    int m_currentThreadId;
    int m_totalThreads;
    std::vector<std::shared_ptr<Environment>> m_threadEnvironments;

    // Resource tracking
    double m_aethel;
    double m_chronons;

    // Enumeration for temporal operations
    enum class TemporalOperation {
        FOR_CHRONON,
        WHILE_EVENT,
        REWIND_FLOW,
        BRANCH_TIMELINE,
        MERGE_TIMELINES,
        TEMPORAL_ECHO_LOOP,
        TEMPORAL_LOOP,
        PARALLEL_EXECUTION
    };

    // Helper methods
    double calculateTemporalResourceCost(TemporalOperation op);
    void consumeResources(double amount);

    // Helper methods for executing blocks and evaluating variables
    void executeBlock(const BlockStmtNode& block, std::shared_ptr<Environment> environment);
    Value lookUpVariable(const std::string& name, const SourceLocation& location);

    // Helper methods for handling CONF/REB interactions
    Value handleVariableInteraction(const Value& left, const Value& right, TokenType operation);
    void updateParadoxLevel(const Value& left, const Value& right, TokenType operation);

    // Helper methods for handling temporal operations
    void executeForChronon(const TemporalOpStmtNode& stmt);
    void executeWhileEvent(const TemporalOpStmtNode& stmt);
    void executeRewindFlow(const TemporalOpStmtNode& stmt);
    void executeBranchTimeline(const TemporalOpStmtNode& stmt);
    void executeMergeTimelines(const TemporalOpStmtNode& stmt);
    void executeTemporalEchoLoop(const TemporalOpStmtNode& stmt);
    void executeTemporalLoopStmt(const TemporalLoopStmtNode& stmt);

    // Helper method for numeric conversions
    double getNumericValue(const Value& value);

    // Native function definitions
    void defineNativeFunctions();

    // Define temporal-related native functions
    void defineTemporalFunctions();

    // Define type-related native functions
    void defineTypeNativeFunctions();

    void defineUserTypeNativeFunctions();

    void initializeGlobals();

    // Resource tracking for REWIND_FLOW control mechanisms
    void trackResourceUsage(double aethel, double chronons, const std::string& operation);

    // Arithmetic and logical operation helper methods
    Value negate(const Value& operand);
    Value logicalNot(const Value& operand);
    Value add(const Value& left, const Value& right);
    Value subtract(const Value& left, const Value& right);
    Value multiply(const Value& left, const Value& right);
    Value divide(const Value& left, const Value& right);
    Value modulo(const Value& left, const Value& right);
    bool areEqual(const Value& left, const Value& right);

    // Helper method for parallel execution
    void executeParallelBlock(const BlockStmtNode& block, int threadId, int totalThreads);
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_INTERPRETER_H