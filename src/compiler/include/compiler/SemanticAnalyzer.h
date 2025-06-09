#ifndef CHRONOVYAN_COMPILER_SEMANTIC_ANALYZER_H
#define CHRONOVYAN_COMPILER_SEMANTIC_ANALYZER_H

#include <unordered_map>

#include "compiler/AstNode.h"
#include "compiler/AstVisitor.h"
#include "compiler/Diagnostic.h"
#include "compiler/SymbolTable.h"
#include "compiler/TypeChecker.h"

namespace chronovyan {
namespace compiler {

/**
 * SemanticAnalyzer is responsible for performing semantic analysis on the AST.
 * It checks for type correctness, scope validity, and other semantic
 * constraints.
 */
class SemanticAnalyzer : public AstVisitor {
public:
  /**
   * Constructs a semantic analyzer.
   *
   * @param diagnostics The diagnostic reporter to use for reporting errors
   */
  SemanticAnalyzer(DiagnosticReporter &diagnostics);

  /**
   * Analyzes the AST starting from the program node.
   *
   * @param programNode The root node of the AST
   */
  void analyze(ProgramNode *programNode);

  /**
   * Checks if there were any errors during the analysis.
   */
  bool hasErrors() const;

  // Visit methods for each node type
  void visitProgramNode(ProgramNode *node) override;
  void visitBasicTypeNode(BasicTypeNode *node) override;
  void visitLiteralExprNode(LiteralExprNode *node) override;
  void visitVariableExprNode(VariableExprNode *node) override;
  void visitUnaryExprNode(UnaryExprNode *node) override;
  void visitBinaryExprNode(BinaryExprNode *node) override;
  void visitCallExprNode(CallExprNode *node) override;
  void visitAssignExprNode(AssignExprNode *node) override;
  void visitExprStmtNode(ExprStmtNode *node) override;
  void visitBlockStmtNode(BlockStmtNode *node) override;
  void visitIfStmtNode(IfStmtNode *node) override;
  void visitWhileStmtNode(WhileStmtNode *node) override;
  void visitVarDeclNode(VarDeclNode *node) override;
  void visitResourceStmtNode(ResourceStmtNode *node) override;
  void visitStmtDeclNode(StmtDeclNode *node) override;
  void visitParallelExecutionStmtNode(ParallelExecutionStmtNode *node) override;
  void visitBranchTimelineStmtNode(BranchTimelineStmtNode *node) override;

  // Expression type management
  Type getExprType(const ExprNode *expr);
  void setExprType(const ExprNode *expr, const Type &type);

private:
  DiagnosticReporter &diagnostics;

  // Symbol table for managing scopes and symbols
  SymbolTable symbolTable;

  // Type checker for type checking
  TypeChecker typeChecker;

  // Type information for expressions
  std::unordered_map<const ExprNode *, Type> exprTypes;

  // Type checking related methods
  bool checkBinaryOperatorTypes(BinaryExprNode *node);
  bool checkUnaryOperatorTypes(UnaryExprNode *node);
  bool checkAssignmentTypes(AssignExprNode *node);
  bool checkCallArgumentTypes(CallExprNode *node);

  // Helper methods
  void reportError(const std::string &message, const Token &token);
  void reportWarning(const std::string &message, const Token &token);
  void reportError(const std::string &message, const SourceLocation &location);
  void reportWarning(const std::string &message,
                     const SourceLocation &location);
};

} // namespace compiler
} // namespace chronovyan

#endif // CHRONOVYAN_COMPILER_SEMANTIC_ANALYZER_H