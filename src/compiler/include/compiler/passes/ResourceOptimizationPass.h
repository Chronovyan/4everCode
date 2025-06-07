#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "compiler/AstVisitor.h"
#include "compiler/OptimizationPass.h"

namespace chronovyan {
namespace compiler {

/**
 * @class ResourceOptimizationPass
 * @brief Optimization pass that optimizes resource usage in temporal operations.
 *
 * This pass analyzes temporal operations and optimizes their resource usage by:
 * - Coalescing multiple temporal operations where possible
 * - Reordering operations to minimize resource cost
 * - Identifying and eliminating redundant temporal operations
 * - Optimizing temporal operations based on their context and usage patterns
 */
class ResourceOptimizationPass : public OptimizationPass, private AstVisitor {
public:
    /**
     * @brief Default constructor.
     */
    ResourceOptimizationPass();

    /**
     * @brief Get the name of the optimization pass.
     *
     * @return The name of the pass.
     */
    std::string getName() const override;

    /**
     * @brief Get the description of what the optimization pass does.
     *
     * @return A brief description of the optimization performed.
     */
    std::string getDescription() const override;

    /**
     * @brief Run the optimization pass on an AST node.
     *
     * @param node The AST node to optimize.
     * @param diagnostics The diagnostic reporter for reporting issues.
     * @return True if any optimizations were applied, false otherwise.
     */
    bool run(AstNode* node, DiagnosticReporter& diagnostics) override;

    /**
     * @brief Create a deep clone of this optimization pass.
     *
     * @return A unique pointer to a new instance of this pass.
     */
    std::unique_ptr<OptimizationPass> clone() const override;

private:
    DiagnosticReporter* diagnostics;
    bool modified;

    // Resource usage tracking
    struct ResourceUsage {
        double aethelCost;
        double chrononCost;
        bool isConstant;

        ResourceUsage() : aethelCost(0.0), chrononCost(0.0), isConstant(false) {}
        ResourceUsage(double aethel, double chronon, bool constant = false)
            : aethelCost(aethel), chrononCost(chronon), isConstant(constant) {}
    };

    // Resource usage map for statements
    std::map<AstNode*, ResourceUsage> resourceUsageMap;

    // AstVisitor interface implementation
    void visitProgramNode(ProgramNode* node) override;
    void visitBasicTypeNode(BasicTypeNode* node) override;
    void visitLiteralExprNode(LiteralExprNode* node) override;
    void visitVariableExprNode(VariableExprNode* node) override;
    void visitUnaryExprNode(UnaryExprNode* node) override;
    void visitBinaryExprNode(BinaryExprNode* node) override;
    void visitCallExprNode(CallExprNode* node) override;
    void visitAssignExprNode(AssignExprNode* node) override;
    void visitVarDeclNode(VarDeclNode* node) override;
    void visitExprStmtNode(ExprStmtNode* node) override;
    void visitBlockStmtNode(BlockStmtNode* node) override;
    void visitIfStmtNode(IfStmtNode* node) override;
    void visitWhileStmtNode(WhileStmtNode* node) override;
    void visitResourceStmtNode(ResourceStmtNode* node) override;
    void visitStmtDeclNode(StmtDeclNode* node) override;

    /**
     * @brief Calculate the resource usage for a node.
     *
     * @param node The node to calculate resource usage for.
     * @return The resource usage.
     */
    ResourceUsage calculateResourceUsage(AstNode* node);

    /**
     * @brief Try to optimize a resource statement.
     *
     * @param node The resource statement to optimize.
     * @return True if the statement was optimized, false otherwise.
     */
    bool tryOptimizeResourceStmt(ResourceStmtNode* node);

    /**
     * @brief Optimize temporal operations in a block.
     *
     * @param node The block to optimize.
     * @return True if any optimizations were applied, false otherwise.
     */
    bool optimizeTemporalOperations(BlockStmtNode* node);

    /**
     * @brief Check if a node is a temporal operation.
     *
     * @param node The node to check.
     * @return True if the node is a temporal operation, false otherwise.
     */
    bool isTemporalOperation(AstNode* node);

    /**
     * @brief Get the cost of a temporal operation.
     *
     * @param node The temporal operation node.
     * @return The resource usage of the operation.
     */
    ResourceUsage getTemporalOperationCost(AstNode* node);

    /**
     * @brief Check if two temporal operations can be coalesced.
     *
     * @param first The first operation.
     * @param second The second operation.
     * @return True if the operations can be coalesced, false otherwise.
     */
    bool canCoalesceOperations(AstNode* first, AstNode* second);

    /**
     * @brief Coalesce two temporal operations.
     *
     * @param first The first operation.
     * @param second The second operation.
     * @return A new node that combines the two operations, or nullptr if coalescing failed.
     */
    AstNode* coalesceOperations(AstNode* first, AstNode* second);
};

}  // namespace compiler
}  // namespace chronovyan