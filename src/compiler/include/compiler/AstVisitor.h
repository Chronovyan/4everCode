#ifndef CHRONOVYAN_AST_VISITOR_H
#define CHRONOVYAN_AST_VISITOR_H

namespace chronovyan {
namespace compiler {

// Forward declarations for all AST node types
class ProgramNode;
class BasicTypeNode;
class LiteralExprNode;
class VariableExprNode;
class UnaryExprNode;
class BinaryExprNode;
class CallExprNode;
class AssignExprNode;
class ExprStmtNode;
class BlockStmtNode;
class IfStmtNode;
class WhileStmtNode;
class VarDeclNode;
class ResourceStmtNode;
class StmtDeclNode;
class ParallelExecutionStmtNode;
class BranchTimelineStmtNode;

/**
 * AstVisitor defines the interface for visitors that traverse the AST.
 * This is the base class for all visitors that need to process AST nodes.
 */
class AstVisitor {
public:
  virtual ~AstVisitor() = default;

  // Visit methods for each node type
  virtual void visitProgramNode(ProgramNode *node) = 0;
  virtual void visitBasicTypeNode(BasicTypeNode *node) = 0;
  virtual void visitLiteralExprNode(LiteralExprNode *node) = 0;
  virtual void visitVariableExprNode(VariableExprNode *node) = 0;
  virtual void visitUnaryExprNode(UnaryExprNode *node) = 0;
  virtual void visitBinaryExprNode(BinaryExprNode *node) = 0;
  virtual void visitCallExprNode(CallExprNode *node) = 0;
  virtual void visitAssignExprNode(AssignExprNode *node) = 0;
  virtual void visitExprStmtNode(ExprStmtNode *node) = 0;
  virtual void visitBlockStmtNode(BlockStmtNode *node) = 0;
  virtual void visitIfStmtNode(IfStmtNode *node) = 0;
  virtual void visitWhileStmtNode(WhileStmtNode *node) = 0;
  virtual void visitVarDeclNode(VarDeclNode *node) = 0;
  virtual void visitResourceStmtNode(ResourceStmtNode *node) = 0;
  virtual void visitStmtDeclNode(StmtDeclNode *node) = 0;
  virtual void
  visitParallelExecutionStmtNode(ParallelExecutionStmtNode *node) = 0;
  virtual void visitBranchTimelineStmtNode(BranchTimelineStmtNode *node) = 0;
};

} // namespace compiler
} // namespace chronovyan

#endif // CHRONOVYAN_AST_VISITOR_H