#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "ast/AstVisitor.h"
#include "compiler/AstVisitor.h"
#include "compiler/OptimizationPass.h"
#include "diagnostics/DiagnosticReporter.h"

namespace chronovyan {
namespace compiler {

/**
 * @class ConstantFoldingPass
 * @brief Optimization pass that performs constant folding.
 *
 * This pass evaluates constant expressions at compile time and replaces
 * them with their computed values, reducing runtime computation.
 */
class ConstantFoldingPass : public OptimizationPass, private AstVisitor {
public:
  /**
   * @brief Default constructor.
   */
  ConstantFoldingPass(DiagnosticReporter *diagnostics = nullptr);

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
  bool run(AstNode *node, DiagnosticReporter &diagnostics) override;

  /**
   * @brief Create a deep clone of this optimization pass.
   *
   * @return A unique pointer to a new instance of this pass.
   */
  std::unique_ptr<OptimizationPass> clone() const override;

  // Visitors for expressions
  ExprNode *visitLiteralExprNode(LiteralExprNode &node) override;
  ExprNode *visitVariableExprNode(VariableExprNode &node) override;
  ExprNode *visitUnaryExprNode(UnaryExprNode &node) override;
  ExprNode *visitBinaryExprNode(BinaryExprNode &node) override;
  ExprNode *visitCallExprNode(CallExprNode &node) override;
  ExprNode *visitAssignExprNode(AssignExprNode &node) override;

  // Visitors for statements
  StmtNode *visitExprStmtNode(ExprStmtNode &node) override;
  StmtNode *visitBlockStmtNode(BlockStmtNode &node) override;
  StmtNode *visitIfStmtNode(IfStmtNode &node) override;
  StmtNode *visitWhileStmtNode(WhileStmtNode &node) override;
  StmtNode *visitResourceStmtNode(ResourceStmtNode &node) override;
  StmtNode *visitVarDeclNode(VarDeclNode &node) override;
  StmtNode *visitStmtDeclNode(StmtDeclNode &node) override;
  StmtNode *
  visitParallelExecutionStmtNode(ParallelExecutionStmtNode &node) override;
  StmtNode *visitBranchTimelineStmtNode(BranchTimelineStmtNode &node) override;

  // Helper methods for folding expressions
  bool tryFoldBinaryExpr(BinaryExprNode *node);
  bool tryFoldUnaryExpr(UnaryExprNode *node);

  // Helper methods for node replacement
  void replaceExpressionNode(ExprNode *oldNode,
                             std::unique_ptr<ExprNode> newNode);
  void replaceStatementNode(StmtNode *oldNode,
                            std::unique_ptr<StmtNode> newNode);

private:
  DiagnosticReporter *diagnostics;
  bool modified;

  // Maps to track node replacements during traversal
  std::unordered_map<ExprNode *, std::unique_ptr<ExprNode>>
      expressionReplacements;
  std::unordered_map<StmtNode *, std::unique_ptr<StmtNode>>
      statementReplacements;

  // Helper methods for evaluating literals
  std::variant<int64_t, double, std::string, bool>
  evaluateLiteral(const LiteralExprNode *node);
};

} // namespace compiler
} // namespace chronovyan