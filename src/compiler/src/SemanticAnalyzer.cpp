#include <cassert>

#include "compiler/SemanticAnalyzer.h"

namespace chronovyan {
namespace compiler {

SemanticAnalyzer::SemanticAnalyzer(DiagnosticReporter &diagnostics)
    : diagnostics(diagnostics), typeChecker(diagnostics) {}

void SemanticAnalyzer::analyze(ProgramNode *programNode) {
  if (!programNode) {
    return;
  }

  // Reset state
  symbolTable = SymbolTable();
  exprTypes.clear();

  // Start analysis
  visitProgramNode(programNode);
}

bool SemanticAnalyzer::hasErrors() const { return diagnostics.hasErrors(); }

void SemanticAnalyzer::visitProgramNode(ProgramNode *node) {
  // Visit all declarations in the program
  for (const auto &decl : node->getDeclarations()) {
    if (decl) {
      decl->accept(*this);
    }
  }
}

void SemanticAnalyzer::visitBasicTypeNode(BasicTypeNode *node) {
  // Nothing to do for basic types
}

void SemanticAnalyzer::visitLiteralExprNode(LiteralExprNode *node) {
  // Get the type of the literal
  Type type = typeChecker.getLiteralType(node);
  setExprType(node, type);
}

void SemanticAnalyzer::visitVariableExprNode(VariableExprNode *node) {
  // Look up the variable in the symbol table
  const std::string &name = node->getName().getLexeme();
  Symbol *symbol = symbolTable.lookupSymbol(name);

  if (!symbol) {
    reportError("Undefined variable '" + name + "'", node->getName());
    setExprType(node, Type(DataType::UNKNOWN));
    return;
  }

  // Check if the variable is initialized
  if (!symbol->initialized) {
    reportWarning("Variable '" + name + "' may be used before initialization",
                  node->getName());
  }

  // Set the expression type
  if (symbol->type) {
    setExprType(node, typeChecker.getTypeFromTypeNode(symbol->type));
  } else {
    setExprType(node, Type(DataType::UNKNOWN));
  }
}

void SemanticAnalyzer::visitUnaryExprNode(UnaryExprNode *node) {
  // Visit the operand
  node->getRight().accept(*this);

  // Get the type of the operand
  Type rightType = getExprType(&node->getRight());

  // Check the operation
  Type resultType =
      typeChecker.checkUnaryOperation(node->getOperator().getType(), rightType);
  setExprType(node, resultType);
}

void SemanticAnalyzer::visitBinaryExprNode(BinaryExprNode *node) {
  // Visit the operands
  node->getLeft().accept(*this);
  node->getRight().accept(*this);

  // Get the types of the operands
  Type leftType = getExprType(&node->getLeft());
  Type rightType = getExprType(&node->getRight());

  // Check the operation
  Type resultType = typeChecker.checkBinaryOperation(
      node->getOperator().getType(), leftType, rightType);
  setExprType(node, resultType);
}

void SemanticAnalyzer::visitCallExprNode(CallExprNode *node) {
  // Visit the callee
  node->getCallee().accept(*this);

  // Visit the arguments
  for (const auto &arg : node->getArguments()) {
    arg->accept(*this);
  }

  // Get the type of the callee
  Type calleeType = getExprType(&node->getCallee());

  // For now, assume all calls return a value of type ANY
  // In a real implementation, we'd look up the function signature
  setExprType(node, Type(DataType::UNKNOWN));
}

void SemanticAnalyzer::visitAssignExprNode(AssignExprNode *node) {
  // Look up the variable in the symbol table
  const std::string &name = node->getName().getLexeme();
  Symbol *symbol = symbolTable.lookupSymbol(name);

  if (!symbol) {
    reportError("Undefined variable '" + name + "'", node->getName());
    setExprType(node, Type(DataType::UNKNOWN));
    return;
  }

  // Visit the value
  node->getValue().accept(*this);

  // Get the type of the value
  Type valueType = getExprType(&node->getValue());

  // Check if the types are compatible
  if (symbol->type) {
    Type varType = typeChecker.getTypeFromTypeNode(symbol->type);

    if (!typeChecker.checkAssignment(varType, valueType)) {
      reportError("Cannot assign value of type '" + valueType.toString() +
                      "' to variable '" + name + "' of type '" +
                      varType.toString() + "'",
                  node->getName());
    }

    setExprType(node, varType);
  } else {
    setExprType(node, valueType);
  }

  // Mark the variable as initialized
  symbol->initialized = true;
}

void SemanticAnalyzer::visitExprStmtNode(ExprStmtNode *node) {
  // Visit the expression
  node->getExpression().accept(*this);
}

void SemanticAnalyzer::visitBlockStmtNode(BlockStmtNode *node) {
  // Enter a new scope
  symbolTable.enterScope();

  // Visit all statements in the block
  for (const auto &stmt : node->getStatements()) {
    stmt->accept(*this);
  }

  // Exit the scope
  symbolTable.exitScope();
}

void SemanticAnalyzer::visitIfStmtNode(IfStmtNode *node) {
  // Visit the condition
  node->getCondition().accept(*this);

  // Check if the condition is a boolean
  Type condType = getExprType(&node->getCondition());
  if (condType.getBaseType() != DataType::BOOLEAN &&
      condType.getBaseType() != DataType::UNKNOWN) {
    reportError("Condition must be a boolean expression", node->getLocation());
  }

  // Visit the then branch
  node->getThenBranch().accept(*this);

  // Visit the else branch if it exists
  if (node->getElseBranch()) {
    node->getElseBranch()->accept(*this);
  }
}

void SemanticAnalyzer::visitWhileStmtNode(WhileStmtNode *node) {
  // Visit the condition
  node->getCondition().accept(*this);

  // Check if the condition is a boolean
  Type condType = getExprType(&node->getCondition());
  if (condType.getBaseType() != DataType::BOOLEAN &&
      condType.getBaseType() != DataType::UNKNOWN) {
    reportError("Condition must be a boolean expression", node->getLocation());
  }

  // Visit the body
  node->getBody().accept(*this);
}

void SemanticAnalyzer::visitVarDeclNode(VarDeclNode *node) {
  // Get the variable name
  const std::string &name = node->getName().getLexeme();

  // Check for redeclaration in the current scope
  if (symbolTable.lookupSymbolInCurrentScope(name)) {
    reportError("Redeclaration of variable '" + name + "'", node->getName());
    return;
  }

  // Visit the initializer if it exists
  bool initialized = false;
  if (node->getInitializer()) {
    node->getInitializer()->accept(*this);
    initialized = true;

    // Check if the initializer type is compatible with the declared type
    if (node->getType()) {
      Type declaredType = typeChecker.getTypeFromTypeNode(node->getType());
      Type initType = getExprType(node->getInitializer());

      if (!typeChecker.checkAssignment(declaredType, initType)) {
        reportError("Cannot initialize variable of type '" +
                        declaredType.toString() + "' with value of type '" +
                        initType.toString() + "'",
                    node->getName());
      }
    }
  }

  // Add the variable to the symbol table
  symbolTable.addSymbol(name, Symbol::Kind::VARIABLE, node->getType(),
                        initialized);
}

void SemanticAnalyzer::visitResourceStmtNode(ResourceStmtNode *node) {
  // Visit the chronons and aethel expressions if they exist
  if (node->getChronons()) {
    node->getChronons()->accept(*this);

    // Check if chronons is an integer
    Type chrononsType = getExprType(node->getChronons());
    if (chrononsType.getBaseType() != DataType::INTEGER &&
        chrononsType.getBaseType() != DataType::UNKNOWN) {
      reportError("Chronons value must be an integer", node->getLocation());
    }
  }

  if (node->getAethel()) {
    node->getAethel()->accept(*this);

    // Check if aethel is an integer
    Type aethelType = getExprType(node->getAethel());
    if (aethelType.getBaseType() != DataType::INTEGER &&
        aethelType.getBaseType() != DataType::UNKNOWN) {
      reportError("Aethel value must be an integer", node->getLocation());
    }
  }

  // Visit the body
  node->getBody().accept(*this);
}

void SemanticAnalyzer::visitStmtDeclNode(StmtDeclNode *node) {
  // Visit the statement
  node->getStmt().accept(*this);
}

void SemanticAnalyzer::visitParallelExecutionStmtNode(
    ParallelExecutionStmtNode *node) {
  // Check and analyze the thread count expression
  node->getThreadCount().accept(*this);

  // Visit the body
  node->getBody().accept(*this);
}

void SemanticAnalyzer::visitBranchTimelineStmtNode(
    BranchTimelineStmtNode *node) {
  // Check the branch count expression
  node->getBranchCount().accept(*this);

  // Visit the body
  node->getBody().accept(*this);
}

Type SemanticAnalyzer::getExprType(const ExprNode *expr) {
  if (!expr) {
    return Type(DataType::UNKNOWN);
  }

  auto it = exprTypes.find(expr);
  if (it != exprTypes.end()) {
    return it->second;
  }

  return Type(DataType::UNKNOWN);
}

void SemanticAnalyzer::setExprType(const ExprNode *expr, const Type &type) {
  if (expr) {
    exprTypes[expr] = type;
  }
}

void SemanticAnalyzer::reportError(const std::string &message,
                                   const Token &token) {
  diagnostics.addError(token, message);
}

void SemanticAnalyzer::reportWarning(const std::string &message,
                                     const Token &token) {
  diagnostics.addWarning(token, message);
}

void SemanticAnalyzer::reportError(const std::string &message,
                                   const SourceLocation &location) {
  diagnostics.addError(location, message);
}

void SemanticAnalyzer::reportWarning(const std::string &message,
                                     const SourceLocation &location) {
  diagnostics.addWarning(location, message);
}

} // namespace compiler
} // namespace chronovyan