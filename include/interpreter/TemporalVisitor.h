#ifndef CHRONOVYAN_TEMPORAL_VISITOR_H
#define CHRONOVYAN_TEMPORAL_VISITOR_H

#include <memory>
#include <stack>
#include <string>

#include "ast_nodes.h"
#include "value.h"

namespace chronovyan {

// Forward declarations
class Interpreter;
class Environment;
class TemporalRuntime;
class ResourceOptimizer;
class TemporalDebtTracker;
class TemporalLoopNode;
class ParallelExecutionNode;
class BranchTimelineNode;

/**
 * @class TemporalVisitor
 * @brief Handles the execution of temporal operations in the AST
 */
class TemporalVisitor {
public:
    /**
     * @brief Create a new temporal visitor
     * @param interpreter Reference to the parent interpreter
     */
    explicit TemporalVisitor(Interpreter& interpreter);

    /**
     * @brief Execute a temporal operation statement
     * @param stmt The temporal operation statement to execute
     */
    void visitTemporalOpStmt(const TemporalOpStmtNode& stmt);

    /**
     * @brief Execute a for-chronon loop
     * @param stmt The for-chronon statement to execute
     */
    void visitForChrononStmt(const ForChrononStmtNode& stmt);

    /**
     * @brief Execute a while-event loop
     * @param stmt The while-event statement to execute
     */
    void visitWhileEventStmt(const WhileEventStmtNode& stmt);

    /**
     * @brief Execute a rewind-flow operation
     * @param stmt The rewind-flow statement to execute
     */
    void visitRewindFlowStmt(const RewindFlowStmtNode& stmt);

    /**
     * @brief Execute a reverse-flow operation
     * @param stmt The reverse-flow statement to execute
     */
    void visitReverseFlowStmt(const ReverseFlowStmtNode& stmt);

    /**
     * @brief Execute a temporal loop operation
     * @param stmt The temporal-loop statement to execute
     */
    void visitTemporalLoopStmt(const TemporalLoopStmtNode& stmt);

    /**
     * @brief Execute a break-chronon statement
     * @param stmt The break-chronon statement to execute
     */
    void visitBreakChrononStmt(const BreakChrononStmtNode& stmt);

    /**
     * @brief Execute a continue-weave statement
     * @param stmt The continue-weave statement to execute
     */
    void visitContinueWeaveStmt(const ContinueWeaveStmtNode& stmt);

    /**
     * @brief Execute a halt-rewind statement
     * @param stmt The halt-rewind statement to execute
     */
    void visitHaltRewindStmt(const HaltRewindStmtNode& stmt);

    /**
     * @brief Execute a stabilize-timeline statement
     * @param stmt The stabilize-timeline statement to execute
     */
    void visitStabilizeTimelineStmt(const StabilizeTimelineStmtNode& stmt);

    /**
     * @brief Execute a prevent-modification statement
     * @param stmt The prevent-modification statement to execute
     */
    void visitPreventModificationStmt(const PreventModificationStmtNode& stmt);

    /**
     * @brief Execute an expend-resources statement
     * @param stmt The expend-resources statement to execute
     */
    void visitExpendResourcesStmt(const ExpendResourcesStmtNode& stmt);

    /**
     * @brief Execute a temporal loop
     * @param node The temporal loop node to execute
     */
    void visit(const TemporalLoopNode& node);

    /**
     * @brief Execute a parallel execution block
     * @param node The parallel execution node to execute
     */
    void visit(const ParallelExecutionNode& node);

    /**
     * @brief Execute a branch timeline operation
     * @param node The branch timeline node to execute
     */
    void visit(const BranchTimelineNode& node);

    /**
     * @brief Handle a resource insufficiency event
     * @param handler The resource insufficiency handler to execute
     */
    void visitResourceInsufficiencyHandler(const ResourceInsufficiencyHandlerNode& handler);

    /**
     * @brief Handle temporal section node
     * @param section The temporal section to process
     */
    void visitTemporalSection(const TemporalSectionNode& section);

    /**
     * @brief Process a temporal program
     * @param program The temporal program to execute
     */
    void visitTemporalProgram(const TemporalProgramNode& program);

    // Control flow state accessors
    bool isBreakChronon() const;
    bool isContinueWeave() const;
    bool isReverseFlow() const;
    bool isHaltRewind() const;
    bool isPreventModification() const;
    bool isReadOnlyMode() const;
    double getStabilizationFactor() const;
    int getFlowDirection() const;

    // Control flow state modifiers
    void clearTemporalFlags();
    void setBreakChronon(bool flag);
    void setContinueWeave(bool flag);
    void setReverseFlow(bool flag);
    void setHaltRewind(bool flag);
    void setPreventModification(bool flag);
    void setReadOnlyMode(bool flag);
    void setStabilizationFactor(double factor);
    void setFlowDirection(int direction);

private:
    Interpreter& m_interpreter;

    // Temporal control flow state
    bool m_breakChronon_flag = false;
    bool m_continueWeave_flag = false;
    bool m_reverseFlow_flag = false;
    bool m_haltRewind_flag = false;
    bool m_preventModification_flag = false;
    bool m_readOnlyMode = false;
    double m_stabilizationFactor = 0.0;
    int m_flowDirection = 1;

    // Helper methods
    double calculateTemporalResourceCost(TemporalOperation op);
    void consumeResources(double amount);

    // Helper methods for handling temporal operations
    void executeForChronon(const TemporalOpStmtNode& stmt);
    void executeWhileEvent(const TemporalOpStmtNode& stmt);
    void executeRewindFlow(const TemporalOpStmtNode& stmt);
    void executeBranchTimeline(const TemporalOpStmtNode& stmt);
    void executeMergeTimelines(const TemporalOpStmtNode& stmt);
    void executeTemporalEchoLoop(const TemporalOpStmtNode& stmt);

    // Enumeration for temporal operations
    enum class TemporalOperation { REWIND_FLOW, STABILIZE, PREVENT_MODIFICATION, TEMPORAL_LOOP };
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_TEMPORAL_VISITOR_H