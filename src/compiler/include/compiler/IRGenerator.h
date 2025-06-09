#pragma once

#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "compiler/AstVisitor.h"
#include "compiler/DiagnosticReporter.h"
#include "compiler/IR.h"

namespace chronovyan {
namespace compiler {

/**
 * @brief Generates IR code from an AST
 *
 * This class implements the visitor pattern to traverse the AST
 * and generate corresponding IR instructions.
 */
class IRGenerator : public AstVisitor {
public:
  /**
   * @brief Constructor
   * @param diagnostics Diagnostic reporter for reporting errors
   */
  explicit IRGenerator(DiagnosticReporter &diagnostics);

  /**
   * @brief Generate IR from an AST
   * @param node The root node of the AST
   * @return The generated IR program
   */
  IRProgram generate(AstNode *node);

  // AstVisitor interface implementation
  void visitProgramNode(ProgramNode *node) override;
  void visitBasicTypeNode(BasicTypeNode *node) override;
  void visitLiteralExprNode(LiteralExprNode *node) override;
  void visitVariableExprNode(VariableExprNode *node) override;
  void visitUnaryExprNode(UnaryExprNode *node) override;
  void visitBinaryExprNode(BinaryExprNode *node) override;
  void visitCallExprNode(CallExprNode *node) override;
  void visitAssignExprNode(AssignExprNode *node) override;
  void visitVarDeclNode(VarDeclNode *node) override;
  void visitExprStmtNode(ExprStmtNode *node) override;
  void visitBlockStmtNode(BlockStmtNode *node) override;
  void visitIfStmtNode(IfStmtNode *node) override;
  void visitWhileStmtNode(WhileStmtNode *node) override;
  void visitResourceStmtNode(ResourceStmtNode *node) override;
  void visitParallelExecutionStmtNode(ParallelExecutionStmtNode *node) override;
  void visitBranchTimelineStmtNode(BranchTimelineStmtNode *node) override;
  void visitStmtDeclNode(StmtDeclNode *node) override;

private:
  // Helper methods
  /**
   * @brief Create a new basic block
   * @param label Label for the block
   * @return Reference to the created block
   */
  IRBasicBlock &createBasicBlock(const std::string &label = "");

  /**
   * @brief Generate a unique label
   * @param prefix Prefix for the label
   * @return A unique label string
   */
  std::string generateLabel(const std::string &prefix);

  /**
   * @brief Add an instruction to the current basic block
   * @param opcode Operation code
   * @param location Source location of the instruction
   */
  void addInstruction(IROpCode opcode,
                      const SourceLocation &location = SourceLocation());

  /**
   * @brief Add an instruction with one operand to the current basic block
   * @param opcode Operation code
   * @param operand1 First operand
   * @param location Source location of the instruction
   */
  void addInstruction(IROpCode opcode, const IRValue &operand1,
                      const SourceLocation &location = SourceLocation());

  /**
   * @brief Add an instruction with two operands to the current basic block
   * @param opcode Operation code
   * @param operand1 First operand
   * @param operand2 Second operand
   * @param location Source location of the instruction
   */
  void addInstruction(IROpCode opcode, const IRValue &operand1,
                      const IRValue &operand2,
                      const SourceLocation &location = SourceLocation());

  /**
   * @brief Add an instruction with three operands to the current basic block
   * @param opcode Operation code
   * @param operand1 First operand
   * @param operand2 Second operand
   * @param operand3 Third operand
   * @param location Source location of the instruction
   */
  void addInstruction(IROpCode opcode, const IRValue &operand1,
                      const IRValue &operand2, const IRValue &operand3,
                      const SourceLocation &location = SourceLocation());

  /**
   * @brief Start a new function
   * @param name Function name
   * @param parameters Function parameters
   */
  void startFunction(const std::string &name,
                     const std::vector<std::string> &parameters = {});

  /**
   * @brief End the current function
   */
  void endFunction();

  /**
   * @brief Generate code for a binary operation
   * @param node Binary expression node
   * @param opcode Operation code for the operation
   */
  void generateBinaryOp(BinaryExprNode *node, IROpCode opcode);

  /**
   * @brief Generate code for a comparison operation
   * @param node Binary expression node
   * @param opcode Operation code for the comparison
   */
  void generateComparisonOp(BinaryExprNode *node, IROpCode opcode);

  // Fields
  DiagnosticReporter &diagnostics; ///< Diagnostic reporter
  IRProgram program;               ///< Generated IR program
  IRFunction *currentFunction;     ///< Current function being processed
  IRBasicBlock *currentBlock;      ///< Current basic block being filled
  int labelCounter;                ///< Counter for generating unique labels
  bool hasError; ///< Whether an error occurred during generation

  // Symbol table for local variables
  std::unordered_map<std::string, int> localVariables;

  // Stack of break/continue targets for loops
  struct LoopLabels {
    std::string breakLabel;
    std::string continueLabel;
  };
  std::stack<LoopLabels> loopStack;
};

} // namespace compiler
} // namespace chronovyan