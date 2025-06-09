#include <sstream>

#include "compiler/AstNode.h"
#include "compiler/AstPrinter.h"

namespace chronovyan {
namespace compiler {

AstPrinter::AstPrinter(std::ostream &os) : os(os), indentLevel(0) {}

void AstPrinter::visitProgramNode(ProgramNode *node) {
  printLine("Program");
  indent();
  for (const auto &decl : node->getDeclarations()) {
    if (decl) { // Check if the declaration is not null
      decl->accept(*this);
    } else {
      printLine("<nullptr>");
    }
  }
  indentLevel--;
}

void AstPrinter::visitBasicTypeNode(BasicTypeNode *node) {
  printLine("Type: " + node->getToken().getLexeme());
}

void AstPrinter::visitLiteralExprNode(LiteralExprNode *node) {
  if (node->getToken().hasValue()) {
    printLine("Literal: " + literalToString(node->getValue()));
  } else {
    printLine("Literal: <no value>");
  }
}

void AstPrinter::visitVariableExprNode(VariableExprNode *node) {
  printLine("Variable: " + node->getName().getLexeme());
}

void AstPrinter::visitUnaryExprNode(UnaryExprNode *node) {
  printLine("Unary: " + node->getOperator().getLexeme());
  indent();
  node->getRight().accept(*this);
  indentLevel--;
}

void AstPrinter::visitBinaryExprNode(BinaryExprNode *node) {
  printLine("Binary: " + node->getOperator().getLexeme());
  indent();

  printLine("Left:");
  indent();
  node->getLeft().accept(*this);
  indentLevel--;

  printLine("Right:");
  indent();
  node->getRight().accept(*this);
  indentLevel--;

  indentLevel--;
}

void AstPrinter::visitCallExprNode(CallExprNode *node) {
  printLine("Call");
  indent();

  printLine("Callee:");
  indent();
  node->getCallee().accept(*this);
  indentLevel--;

  printLine("Arguments:");
  indent();
  for (const auto &arg : node->getArguments()) {
    arg->accept(*this);
  }
  indentLevel--;

  indentLevel--;
}

void AstPrinter::visitAssignExprNode(AssignExprNode *node) {
  printLine("Assign: " + node->getName().getLexeme());
  indent();

  printLine("Value:");
  indent();
  node->getValue().accept(*this);
  indentLevel--;

  indentLevel--;
}

void AstPrinter::visitExprStmtNode(ExprStmtNode *node) {
  printLine("Expression Statement");
  indent();
  node->getExpression().accept(*this);
  indentLevel--;
}

void AstPrinter::visitBlockStmtNode(BlockStmtNode *node) {
  printLine("Block");
  indent();

  for (const auto &stmt : node->getStatements()) {
    stmt->accept(*this);
  }

  indentLevel--;
}

void AstPrinter::visitIfStmtNode(IfStmtNode *node) {
  printLine("If Statement");
  indent();

  printLine("Condition:");
  indent();
  node->getCondition().accept(*this);
  indentLevel--;

  printLine("Then Branch:");
  indent();
  node->getThenBranch().accept(*this);
  indentLevel--;

  if (node->getElseBranch()) {
    printLine("Else Branch:");
    indent();
    node->getElseBranch()->accept(*this);
    indentLevel--;
  }

  indentLevel--;
}

void AstPrinter::visitWhileStmtNode(WhileStmtNode *node) {
  printLine("While Statement");
  indent();

  printLine("Condition:");
  indent();
  node->getCondition().accept(*this);
  indentLevel--;

  printLine("Body:");
  indent();
  node->getBody().accept(*this);
  indentLevel--;

  indentLevel--;
}

void AstPrinter::visitVarDeclNode(VarDeclNode *node) {
  printLine("Variable Declaration: " + node->getName().getLexeme());
  indent();

  if (node->getType()) {
    printLine("Type:");
    indent();
    node->getType()->accept(*this);
    indentLevel--;
  }

  if (node->getInitializer()) {
    printLine("Initializer:");
    indent();
    node->getInitializer()->accept(*this);
    indentLevel--;
  }

  indentLevel--;
}

void AstPrinter::visitResourceStmtNode(ResourceStmtNode *node) {
  std::string opStr;
  switch (node->getOperation()) {
  case ResourceStmtNode::ResourceOperation::ALLOCATE:
    opStr = "ALLOCATE";
    break;
  case ResourceStmtNode::ResourceOperation::EXPEND:
    opStr = "EXPEND";
    break;
  case ResourceStmtNode::ResourceOperation::BORROW:
    opStr = "BORROW";
    break;
  case ResourceStmtNode::ResourceOperation::REPAY:
    opStr = "REPAY";
    break;
  }

  printLine("Resource Statement: " + opStr);
  indent();

  if (node->getChronons()) {
    printLine("Chronons:");
    indent();
    node->getChronons()->accept(*this);
    indentLevel--;
  }

  if (node->getAethel()) {
    printLine("Aethel:");
    indent();
    node->getAethel()->accept(*this);
    indentLevel--;
  }

  printLine("Body:");
  indent();
  node->getBody().accept(*this);
  indentLevel--;

  indentLevel--;
}

void AstPrinter::visitStmtDeclNode(StmtDeclNode *node) {
  printLine("Statement Declaration");
  indent();
  // Get the statement from the declaration and visit it
  if (auto stmt = &node->getStmt()) {
    stmt->accept(*this);
  } else {
    printLine("<null statement>");
  }
  indentLevel--;
}

void AstPrinter::visitParallelExecutionStmtNode(
    ParallelExecutionStmtNode *node) {
  printLine("Parallel Execution");
  indent();
  // Process the thread count expression
  printLine("Thread Count:");
  indent();
  node->getThreadCount().accept(*this);
  indentLevel--;

  // Process the body of the parallel execution
  printLine("Body:");
  indent();
  node->getBody().accept(*this);
  indentLevel--;
  indentLevel--;
}

void AstPrinter::visitBranchTimelineStmtNode(BranchTimelineStmtNode *node) {
  printLine("Branch Timeline");
  indent();

  // Process the branch count expression
  printLine("Branch Count:");
  indent();
  node->getBranchCount().accept(*this);
  indentLevel--;

  // Process the body of the branch timeline
  printLine("Body:");
  indent();
  node->getBody().accept(*this);
  indentLevel--;
  indentLevel--;
}

void AstPrinter::indent() { indentLevel++; }

void AstPrinter::printIndent() {
  for (int i = 0; i < indentLevel; i++) {
    os << "  ";
  }
}

void AstPrinter::printLine(const std::string &text) {
  printIndent();
  os << text << std::endl;
}

std::string AstPrinter::literalToString(const Token::ValueType &value) {
  if (std::holds_alternative<std::monostate>(value)) {
    return "nil";
  } else if (std::holds_alternative<bool>(value)) {
    return std::get<bool>(value) ? "true" : "false";
  } else if (std::holds_alternative<double>(value)) {
    return std::to_string(std::get<double>(value));
  } else if (std::holds_alternative<std::string>(value)) {
    return "\"" + std::get<std::string>(value) + "\"";
  }
  return "unknown";
}

} // namespace compiler
} // namespace chronovyan