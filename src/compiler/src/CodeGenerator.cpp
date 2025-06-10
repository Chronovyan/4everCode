#include <cassert>

#include "compiler/CodeGenerator.h"

namespace chronovyan {
namespace compiler {

CodeGenerator::CodeGenerator() : indentLevel(0) {}

std::string CodeGenerator::generate(ProgramNode *programNode) {
  if (!programNode) {
    return "";
  }

  // Clear state
  output.str("");
  output.clear();
  indentLevel = 0;
  symbolTable = SymbolTable();
  exprCode.clear();

  // Add boilerplate
  output << "#include <stdio.h>\n";
  output << "#include <stdlib.h>\n";
  output << "#include <stdbool.h>\n";
  output << "#include <string.h>\n";
  output << "\n";
  output << "// Chronovyan runtime\n";
  output << "#include \"chronovyan_runtime.h\"\n";
  output << "\n";

  // Generate code
  visitProgramNode(programNode);

  return output.str();
}

void CodeGenerator::indent() { indentLevel++; }

void CodeGenerator::dedent() {
  if (indentLevel > 0) {
    indentLevel--;
  }
}

void CodeGenerator::printIndent() {
  for (int i = 0; i < indentLevel; i++) {
    output << "    ";
  }
}

void CodeGenerator::printLine(const std::string &line) {
  printIndent();
  output << line << "\n";
}

std::string CodeGenerator::getNodeCode(const ExprNode *node) {
  auto it = exprCode.find(node);
  if (it != exprCode.end()) {
    return it->second;
  }

  return "";
}

void CodeGenerator::setNodeCode(const ExprNode *node, const std::string &code) {
  exprCode[node] = code;
}

std::string
CodeGenerator::getCTypeForBasicType(const std::string &chronovyanType) {
  if (chronovyanType == "int" || chronovyanType == "integer") {
    return "int";
  } else if (chronovyanType == "float") {
    return "double";
  } else if (chronovyanType == "boolean") {
    return "bool";
  } else if (chronovyanType == "string") {
    return "char*";
  } else if (chronovyanType == "chronon") {
    return "ChronType";
  } else if (chronovyanType == "aethel") {
    return "AethelType";
  } else {
    return "void*"; // Default to void* for unknown types
  }
}

void CodeGenerator::visitProgramNode(ProgramNode *node) {
  // Add main function
  output << "int main(int argc, char** argv) {\n";
  indent();

  // Add Chronovyan runtime initialization
  printLine("// Initialize Chronovyan runtime");
  printLine("ChronovyanRuntime* runtime = chronovyan_init();");
  printLine("");

  // Visit all declarations
  for (const auto &decl : node->getDeclarations()) {
    if (decl) {
      decl->accept(*this);
    }
  }

  // Add runtime cleanup and return
  printLine("");
  printLine("// Cleanup Chronovyan runtime");
  printLine("chronovyan_cleanup(runtime);");
  printLine("return 0;");

  dedent();
  output << "}\n";
}

void CodeGenerator::visitBasicTypeNode(BasicTypeNode *node) {
  // Basic types are handled in the context where they're used
  // This method is typically not called directly
}

void CodeGenerator::visitLiteralExprNode(LiteralExprNode *node) {
  if (!node->getToken().hasValue()) {
    setNodeCode(node, "NULL");
    return;
  }

  const auto &value = node->getValue();

  if (std::holds_alternative<int>(value)) {
    setNodeCode(node, std::to_string(std::get<int>(value)));
  } else if (std::holds_alternative<double>(value)) {
    setNodeCode(node, std::to_string(std::get<double>(value)));
  } else if (std::holds_alternative<std::string>(value)) {
    setNodeCode(node, "\"" + std::get<std::string>(value) + "\"");
  } else if (std::holds_alternative<bool>(value)) {
    setNodeCode(node, std::get<bool>(value) ? "true" : "false");
  } else {
    setNodeCode(node, "NULL");
  }
}

void CodeGenerator::visitVariableExprNode(VariableExprNode *node) {
  setNodeCode(node, node->getName().getLexeme());
}

void CodeGenerator::visitUnaryExprNode(UnaryExprNode *node) {
  // Generate code for the operand
  node->getRight().accept(*this);
  std::string rightCode = getNodeCode(&node->getRight());

  // Generate code for the operator
  std::string opStr;
  switch (node->getOperator().getType()) {
  case TokenType::MINUS:
    opStr = "-";
    break;
  case TokenType::BANG:
    opStr = "!";
    break;
  default:
    opStr = node->getOperator().getLexeme();
    break;
  }

  setNodeCode(node, opStr + "(" + rightCode + ")");
}

void CodeGenerator::visitBinaryExprNode(BinaryExprNode *node) {
  // Generate code for the operands
  node->getLeft().accept(*this);
  node->getRight().accept(*this);

  std::string leftCode = getNodeCode(&node->getLeft());
  std::string rightCode = getNodeCode(&node->getRight());

  // Generate code for the operator
  std::string opStr;
  switch (node->getOperator().getType()) {
  case TokenType::PLUS:
    opStr = "+";
    break;
  case TokenType::MINUS:
    opStr = "-";
    break;
  case TokenType::STAR:
    opStr = "*";
    break;
  case TokenType::SLASH:
    opStr = "/";
    break;
  case TokenType::PERCENT:
    opStr = "%";
    break;
  case TokenType::EQUAL_EQUAL:
    opStr = "==";
    break;
  case TokenType::BANG_EQUAL:
    opStr = "!=";
    break;
  case TokenType::LESS:
    opStr = "<";
    break;
  case TokenType::LESS_EQUAL:
    opStr = "<=";
    break;
  case TokenType::GREATER:
    opStr = ">";
    break;
  case TokenType::GREATER_EQUAL:
    opStr = ">=";
    break;
  case TokenType::AND:
    opStr = "&&";
    break;
  case TokenType::OR:
    opStr = "||";
    break;
  default:
    opStr = node->getOperator().getLexeme();
    break;
  }

  setNodeCode(node, "(" + leftCode + " " + opStr + " " + rightCode + ")");
}

void CodeGenerator::visitCallExprNode(CallExprNode *node) {
  // Generate code for the callee
  node->getCallee().accept(*this);
  std::string calleeCode = getNodeCode(&node->getCallee());

  // Generate code for the arguments
  std::string argsCode;
  bool first = true;
  for (const auto &arg : node->getArguments()) {
    if (!first) {
      argsCode += ", ";
    }
    first = false;

    arg->accept(*this);
    argsCode += getNodeCode(arg.get());
  }

  setNodeCode(node, calleeCode + "(" + argsCode + ")");
}

void CodeGenerator::visitAssignExprNode(AssignExprNode *node) {
  // Generate code for the value
  node->getValue().accept(*this);
  std::string valueCode = getNodeCode(&node->getValue());

  // Generate the assignment code
  std::string variableName = node->getName().getLexeme();
  setNodeCode(node, variableName + " = " + valueCode);
}

void CodeGenerator::visitExprStmtNode(ExprStmtNode *node) {
  // Generate code for the expression
  node->getExpression().accept(*this);
  std::string exprCode = getNodeCode(&node->getExpression());

  // Generate the statement
  printLine(exprCode + ";");
}

void CodeGenerator::visitBlockStmtNode(BlockStmtNode *node) {
  // Enter a new scope
  symbolTable.enterScope();

  // Generate code for the block
  printLine("{");
  indent();

  // Generate code for all statements in the block
  for (const auto &stmt : node->getStatements()) {
    stmt->accept(*this);
  }

  dedent();
  printLine("}");

  // Exit the scope
  symbolTable.exitScope();
}

void CodeGenerator::visitIfStmtNode(IfStmtNode *node) {
  // Generate code for the condition
  node->getCondition().accept(*this);
  std::string conditionCode = getNodeCode(&node->getCondition());

  // Generate the if statement
  printLine("if (" + conditionCode + ")");

  // Generate code for the then branch
  node->getThenBranch().accept(*this);

  // Generate code for the else branch if it exists
  if (node->getElseBranch()) {
    printLine("else");
    node->getElseBranch()->accept(*this);
  }
}

void CodeGenerator::visitWhileStmtNode(WhileStmtNode *node) {
  // Generate code for the condition
  node->getCondition().accept(*this);
  std::string conditionCode = getNodeCode(&node->getCondition());

  // Generate the while statement
  printLine("while (" + conditionCode + ")");

  // Generate code for the body
  node->getBody().accept(*this);
}

void CodeGenerator::visitVarDeclNode(VarDeclNode *node) {
  // Generate type
  std::string typeStr = "auto"; // Default to auto for type inference
  if (node->getType()) {
    if (auto basicType = dynamic_cast<const BasicTypeNode *>(node->getType())) {
      const std::string &typeName = basicType->getToken().getLexeme();
      typeStr = getCTypeForBasicType(typeName);
    }
  }

  // Generate initializer
  std::string initStr = "";
  if (node->getInitializer()) {
    node->getInitializer()->accept(*this);
    initStr = " = " + getNodeCode(node->getInitializer());
  }

  // Generate variable declaration
  std::string name = node->getName().getLexeme();

  // Add the variable to the symbol table
  symbolTable.addSymbol(name, Symbol::Kind::VARIABLE, node->getType(),
                        node->getInitializer() != nullptr);

  // Generate the declaration
  printLine(typeStr + " " + name + initStr + ";");
}

void CodeGenerator::visitResourceStmtNode(ResourceStmtNode *node) {
  // Resource statements are translated to chronovyan_allocate_resources calls

  // Generate code for chronons and aethel
  std::string chrononsCode = "0";
  std::string aethelCode = "0";

  if (node->getChronons()) {
    node->getChronons()->accept(*this);
    chrononsCode = getNodeCode(node->getChronons());
  }

  if (node->getAethel()) {
    node->getAethel()->accept(*this);
    aethelCode = getNodeCode(node->getAethel());
  }

  // Generate the resource allocation code
  std::string operation;
  switch (node->getOperation()) {
  case ResourceStmtNode::ResourceOperation::ALLOCATE:
    operation = "ALLOCATE";
    break;
  case ResourceStmtNode::ResourceOperation::EXPEND:
    operation = "EXPEND";
    break;
  case ResourceStmtNode::ResourceOperation::BORROW:
    operation = "BORROW";
    break;
  case ResourceStmtNode::ResourceOperation::REPAY:
    operation = "REPAY";
    break;
  }

  printLine("// " + operation + " resources");
  printLine("chronovyan_" + operation + "_resources(runtime, " + chrononsCode +
            ", " + aethelCode + ");");

  // Generate code for the body
  node->getBody().accept(*this);
}

void CodeGenerator::visitStmtDeclNode(StmtDeclNode *node) {
  // Just visit the statement
  node->getStmt().accept(*this);
}

void CodeGenerator::visitParallelExecutionStmtNode(
    ParallelExecutionStmtNode *node) {
  printLine("// Parallel execution");
  printLine("{");
  indent();

  // Add Chronovyan runtime parallel execution initialization
  printLine("// Create parallel execution context");
  printLine("ChronovyanParallelExecution* parallel_ctx = "
            "chronovyan_create_parallel_execution(runtime);");

  // Generate code for thread count
  node->getThreadCount().accept(*this);
  std::string threadCountCode = getNodeCode(&node->getThreadCount());
  printLine("// Set thread count");
  printLine("chronovyan_set_thread_count(parallel_ctx, " + threadCountCode +
            ");");

  // Generate code for the body
  printLine("");
  printLine("// Parallel body");
  node->getBody().accept(*this);

  // Execute all branches
  printLine("");
  printLine("// Execute parallel branches");
  printLine("chronovyan_execute_parallel(parallel_ctx);");
  printLine("chronovyan_cleanup_parallel(parallel_ctx);");

  dedent();
  printLine("}");
}

void CodeGenerator::visitBranchTimelineStmtNode(BranchTimelineStmtNode *node) {
  printLine("// Branch timeline");
  printLine("{");
  indent();

  // Generate code for branch count expression
  node->getBranchCount().accept(*this);
  std::string branchCountCode = getNodeCode(&node->getBranchCount());

  printLine("// Create timeline branch");
  printLine("ChronovyanTimeline* timeline_branch = "
            "chronovyan_branch_timeline(runtime, " +
            branchCountCode + ");");

  // Generate code for the branch body
  printLine("");
  printLine("// Branch body");
  node->getBody().accept(*this);

  // Merge the timeline back (for now we just do a simple merge)
  printLine("");
  printLine("// Merge timeline branch back");
  printLine("chronovyan_merge_timeline(runtime, timeline_branch);");

  dedent();
  printLine("}");
}

} // namespace compiler
} // namespace chronovyan