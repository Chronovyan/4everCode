#include <cmath>
#include <iostream>

#include "compiler/passes/ConstantFoldingPass.h"

namespace chronovyan {
namespace compiler {

ConstantFoldingPass::ConstantFoldingPass(DiagnosticReporter *diagnostics)
    : diagnostics(diagnostics), expressionReplacements(),
      statementReplacements() {}

std::string ConstantFoldingPass::getName() const { return "ConstantFolding"; }

std::string ConstantFoldingPass::getDescription() const {
  return "Evaluates constant expressions at compile time and replaces them "
         "with their computed "
         "values.";
}

void ConstantFoldingPass::run(ProgramNode *program) {
  // Reset internal state
  folded.clear();

  // Process the program
  if (program) {
    program->accept(*this);
  }
}

bool ConstantFoldingPass::canFoldBinaryExpression(const BinaryExprNode *node) {
  // Get the operands
  const auto &left = node->getLeft();
  const auto &right = node->getRight();

  // Check if both operands are literals
  const auto *leftLiteral = dynamic_cast<const LiteralExprNode *>(&left);
  const auto *rightLiteral = dynamic_cast<const LiteralExprNode *>(&right);

  return leftLiteral && rightLiteral;
}

bool ConstantFoldingPass::canFoldUnaryExpression(const UnaryExprNode *node) {
  // Get the operand
  const auto &right = node->getRight();

  // Check if the operand is a literal
  const auto *rightLiteral = dynamic_cast<const LiteralExprNode *>(&right);

  return rightLiteral;
}

void ConstantFoldingPass::visitProgramNode(ProgramNode *node) {
  // Visit all declarations in the program
  for (auto &decl : node->getDeclarations()) {
    if (decl) {
      decl->accept(*this);
    }
  }
}

void ConstantFoldingPass::visitBasicTypeNode(BasicTypeNode *node) {
  // Nothing to do for type nodes
}

ExprNode *ConstantFoldingPass::visitLiteralExprNode(LiteralExprNode &node) {
  // Literals are already folded
  return &node;
}

ExprNode *ConstantFoldingPass::visitVariableExprNode(VariableExprNode &node) {
  // Variables cannot be folded at compile time without knowing their values
  return &node;
}

ExprNode *ConstantFoldingPass::visitUnaryExprNode(UnaryExprNode &node) {
  // First, visit the operand expression
  ExprNode *operand = node.getRight().accept(*this);

  // Try to fold this unary expression
  if (tryFoldUnaryExpr(&node)) {
    return operand; // The node has been replaced
  }

  // Return the node as is if folding wasn't possible
  return &node;
}

void ConstantFoldingPass::visitBinaryExprNode(BinaryExprNode *node) {
  // Fold the operands first
  node->getLeft().accept(*this);
  node->getRight().accept(*this);

  // Try to fold this binary expression
  if (canFoldBinaryExpression(node)) {
    // Implement binary expression folding
    tryFoldBinaryExpr(node);
  }
}

ExprNode *ConstantFoldingPass::visitCallExprNode(CallExprNode &node) {
  // Visit all arguments
  for (auto &arg : node.getArguments()) {
    arg->accept(*this);
  }

  // Call expressions cannot be folded at compile time in this implementation
  return &node;
}

ExprNode *ConstantFoldingPass::visitAssignExprNode(AssignExprNode &node) {
  // Visit the value being assigned
  node.getValue().accept(*this);

  // Assignment expressions cannot be folded at compile time
  return &node;
}

StmtNode *ConstantFoldingPass::visitExprStmtNode(ExprStmtNode &node) {
  // Visit the expression
  node.getExpression().accept(*this);

  // Return the node as is
  return &node;
}

StmtNode *ConstantFoldingPass::visitBlockStmtNode(BlockStmtNode &node) {
  // Visit all statements in the block
  for (auto &stmt : node.getStatements()) {
    stmt->accept(*this);
  }

  // Return the node as is
  return &node;
}

StmtNode *ConstantFoldingPass::visitIfStmtNode(IfStmtNode &node) {
  // First, visit the condition expression
  ExprNode *condition = node.getCondition().accept(*this);

  // Check if the condition is a literal
  LiteralExprNode *conditionLiteral =
      dynamic_cast<LiteralExprNode *>(condition);

  if (conditionLiteral) {
    // Get the literal value
    auto value = conditionLiteral->getValue();

    // Check if it's a boolean or can be interpreted as a boolean
    bool conditionValue = false;

    if (std::holds_alternative<bool>(value)) {
      conditionValue = std::get<bool>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
      conditionValue = std::get<int64_t>(value) != 0;
    } else if (std::holds_alternative<double>(value)) {
      conditionValue = std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
      conditionValue = !std::get<std::string>(value).empty();
    }

    // If condition is always true, replace with the then branch
    if (conditionValue) {
      // Visit the then branch
      StmtNode *thenBranch = node.getThenBranch().accept(*this);

      // If the then branch is a block, we can return it directly
      // Otherwise, we need to wrap it in a block
      BlockStmtNode *thenBlock = dynamic_cast<BlockStmtNode *>(thenBranch);

      if (thenBlock) {
        // Return the then block directly
        return thenBranch;
      } else {
        // Create a new block containing the then branch
        std::vector<std::unique_ptr<StmtNode>> statements;
        statements.push_back(std::make_unique<ExprStmtNode>(
            std::unique_ptr<ExprNode>(thenBranch)));

        auto block = std::make_unique<BlockStmtNode>(std::move(statements));

        // Replace the if statement with the block
        replaceStatementNode(&node, std::move(block));
        return thenBranch;
      }
    }
    // If condition is always false and there's an else branch, replace with the
    // else branch
    else if (node.getElseBranch()) {
      // Visit the else branch
      StmtNode *elseBranch = node.getElseBranch()->accept(*this);

      // If the else branch is a block, we can return it directly
      // Otherwise, we need to wrap it in a block
      BlockStmtNode *elseBlock = dynamic_cast<BlockStmtNode *>(elseBranch);

      if (elseBlock) {
        // Return the else block directly
        return elseBranch;
      } else {
        // Create a new block containing the else branch
        std::vector<std::unique_ptr<StmtNode>> statements;
        statements.push_back(std::make_unique<ExprStmtNode>(
            std::unique_ptr<ExprNode>(elseBranch)));

        auto block = std::make_unique<BlockStmtNode>(std::move(statements));

        // Replace the if statement with the block
        replaceStatementNode(&node, std::move(block));
        return elseBranch;
      }
    }
    // If condition is always false and there's no else branch, replace with an
    // empty block
    else {
      // Create an empty block
      std::vector<std::unique_ptr<StmtNode>> statements;
      auto block = std::make_unique<BlockStmtNode>(std::move(statements));

      // Replace the if statement with the empty block
      replaceStatementNode(&node, std::move(block));
      return block.get();
    }
  } else {
    // Visit the then branch
    node.getThenBranch().accept(*this);

    // Visit the else branch if it exists
    if (node.getElseBranch()) {
      node.getElseBranch()->accept(*this);
    }

    // Return the node as is
    return &node;
  }
}

StmtNode *ConstantFoldingPass::visitWhileStmtNode(WhileStmtNode &node) {
  // First, visit the condition expression
  ExprNode *condition = node.getCondition().accept(*this);

  // Check if the condition is a literal
  LiteralExprNode *conditionLiteral =
      dynamic_cast<LiteralExprNode *>(condition);

  if (conditionLiteral) {
    // Get the literal value
    auto value = conditionLiteral->getValue();

    // Check if it's a boolean or can be interpreted as a boolean
    bool conditionValue = false;

    if (std::holds_alternative<bool>(value)) {
      conditionValue = std::get<bool>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
      conditionValue = std::get<int64_t>(value) != 0;
    } else if (std::holds_alternative<double>(value)) {
      conditionValue = std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
      conditionValue = !std::get<std::string>(value).empty();
    }

    // If condition is always false, replace with an empty block
    if (!conditionValue) {
      // Create an empty block
      std::vector<std::unique_ptr<StmtNode>> statements;
      auto block = std::make_unique<BlockStmtNode>(std::move(statements));

      // Replace the while statement with the empty block
      replaceStatementNode(&node, std::move(block));
      return block.get();
    }
    // If condition is always true, we should add a warning about an infinite
    // loop but we still need to visit the body for constant folding
    else if (diagnostics) {
      diagnostics->addWarning(
          node.getLocation(),
          "While loop with constant true condition will never terminate");
    }
  }

  // Visit the body regardless of whether the condition is constant
  // This allows for constant folding within the body
  node.getBody().accept(*this);

  // Return the node as is (or a warning was added for an infinite loop)
  return &node;
}

StmtNode *ConstantFoldingPass::visitVarDeclNode(VarDeclNode &node) {
  // Visit the initializer if present
  if (node.getInitializer()) {
    node.getInitializer()->accept(*this);
  }

  // Variable declarations cannot be folded
  return &node;
}

StmtNode *ConstantFoldingPass::visitStmtDeclNode(StmtDeclNode &node) {
  // Visit the statement in the declaration
  node.getStatement().accept(*this);

  // Statement declarations cannot be folded
  return &node;
}

StmtNode *ConstantFoldingPass::visitResourceStmtNode(ResourceStmtNode &node) {
  // Visit the initialization expression if present
  if (node.getInitializer()) {
    node.getInitializer()->accept(*this);
  }

  // Resource statements cannot be folded
  return &node;
}

StmtNode *ConstantFoldingPass::visitParallelExecutionStmtNode(
    ParallelExecutionStmtNode &node) {
  // Visit all branches
  for (auto &branch : node.getBranches()) {
    branch->accept(*this);
  }

  // Parallel execution statements cannot be folded
  return &node;
}

StmtNode *
ConstantFoldingPass::visitBranchTimelineStmtNode(BranchTimelineStmtNode &node) {
  // Visit the condition
  node.getCondition().accept(*this);

  // Visit the body
  node.getBody().accept(*this);

  // Branch timeline statements cannot be folded
  return &node;
}

bool ConstantFoldingPass::tryFoldBinaryExpr(BinaryExprNode *node) {
  // Check if both operands are literals
  LiteralExprNode *leftLiteral =
      dynamic_cast<LiteralExprNode *>(&node->getLeft());
  LiteralExprNode *rightLiteral =
      dynamic_cast<LiteralExprNode *>(&node->getRight());

  if (!leftLiteral || !rightLiteral) {
    return false;
  }

  // Get the literal values
  auto leftValue = leftLiteral->getValue();
  auto rightValue = rightLiteral->getValue();

  // Try to fold based on the operator type
  TokenType op = node->getOperator().getType();

  // Result value
  std::variant<int64_t, double, std::string, bool> resultValue;
  bool folded = false;

  // Numeric operations (both operands must be numeric)
  if ((std::holds_alternative<int64_t>(leftValue) ||
       std::holds_alternative<double>(leftValue)) &&
      (std::holds_alternative<int64_t>(rightValue) ||
       std::holds_alternative<double>(rightValue))) {
    // If both operands are integers
    if (std::holds_alternative<int64_t>(leftValue) &&
        std::holds_alternative<int64_t>(rightValue)) {
      int64_t leftInt = std::get<int64_t>(leftValue);
      int64_t rightInt = std::get<int64_t>(rightValue);

      switch (op) {
      case TokenType::PLUS:
        resultValue = leftInt + rightInt;
        folded = true;
        break;
      case TokenType::MINUS:
        resultValue = leftInt - rightInt;
        folded = true;
        break;
      case TokenType::STAR:
        resultValue = leftInt * rightInt;
        folded = true;
        break;
      case TokenType::SLASH:
        // Check for division by zero
        if (rightInt == 0) {
          if (diagnostics) {
            diagnostics->addError(node->getLocation(),
                                  "Division by zero in constant expression");
          }
          return false;
        }
        resultValue = leftInt / rightInt;
        folded = true;
        break;
      case TokenType::EQUAL_EQUAL:
        resultValue = leftInt == rightInt;
        folded = true;
        break;
      case TokenType::BANG_EQUAL:
        resultValue = leftInt != rightInt;
        folded = true;
        break;
      case TokenType::LESS:
        resultValue = leftInt < rightInt;
        folded = true;
        break;
      case TokenType::LESS_EQUAL:
        resultValue = leftInt <= rightInt;
        folded = true;
        break;
      case TokenType::GREATER:
        resultValue = leftInt > rightInt;
        folded = true;
        break;
      case TokenType::GREATER_EQUAL:
        resultValue = leftInt >= rightInt;
        folded = true;
        break;
      default:
        // Operator not supported for integer operands
        break;
      }
    }
    // Otherwise, convert to double for calculations
    else {
      double leftNum = std::holds_alternative<int64_t>(leftValue)
                           ? static_cast<double>(std::get<int64_t>(leftValue))
                           : std::get<double>(leftValue);

      double rightNum = std::holds_alternative<int64_t>(rightValue)
                            ? static_cast<double>(std::get<int64_t>(rightValue))
                            : std::get<double>(rightValue);

      switch (op) {
      case TokenType::PLUS:
        resultValue = leftNum + rightNum;
        folded = true;
        break;
      case TokenType::MINUS:
        resultValue = leftNum - rightNum;
        folded = true;
        break;
      case TokenType::STAR:
        resultValue = leftNum * rightNum;
        folded = true;
        break;
      case TokenType::SLASH:
        // Check for division by zero
        if (rightNum == 0.0) {
          if (diagnostics) {
            diagnostics->addError(node->getLocation(),
                                  "Division by zero in constant expression");
          }
          return false;
        }
        resultValue = leftNum / rightNum;
        folded = true;
        break;
      case TokenType::EQUAL_EQUAL:
        resultValue = leftNum == rightNum;
        folded = true;
        break;
      case TokenType::BANG_EQUAL:
        resultValue = leftNum != rightNum;
        folded = true;
        break;
      case TokenType::LESS:
        resultValue = leftNum < rightNum;
        folded = true;
        break;
      case TokenType::LESS_EQUAL:
        resultValue = leftNum <= rightNum;
        folded = true;
        break;
      case TokenType::GREATER:
        resultValue = leftNum > rightNum;
        folded = true;
        break;
      case TokenType::GREATER_EQUAL:
        resultValue = leftNum >= rightNum;
        folded = true;
        break;
      default:
        // Operator not supported for floating-point operands
        break;
      }
    }
  }
  // String operations
  else if (std::holds_alternative<std::string>(leftValue) &&
           std::holds_alternative<std::string>(rightValue)) {
    std::string leftStr = std::get<std::string>(leftValue);
    std::string rightStr = std::get<std::string>(rightValue);

    switch (op) {
    case TokenType::PLUS: // String concatenation
      resultValue = leftStr + rightStr;
      folded = true;
      break;
    case TokenType::EQUAL_EQUAL:
      resultValue = leftStr == rightStr;
      folded = true;
      break;
    case TokenType::BANG_EQUAL:
      resultValue = leftStr != rightStr;
      folded = true;
      break;
    default:
      // Operator not supported for string operands
      break;
    }
  }
  // Boolean operations
  else if (std::holds_alternative<bool>(leftValue) &&
           std::holds_alternative<bool>(rightValue)) {
    bool leftBool = std::get<bool>(leftValue);
    bool rightBool = std::get<bool>(rightValue);

    switch (op) {
    case TokenType::EQUAL_EQUAL:
      resultValue = leftBool == rightBool;
      folded = true;
      break;
    case TokenType::BANG_EQUAL:
      resultValue = leftBool != rightBool;
      folded = true;
      break;
    case TokenType::AND:
      resultValue = leftBool && rightBool;
      folded = true;
      break;
    case TokenType::OR:
      resultValue = leftBool || rightBool;
      folded = true;
      break;
    default:
      // Operator not supported for boolean operands
      break;
    }
  }

  // If we successfully folded the expression, replace it with a literal node
  if (folded) {
    // Create a token for the literal based on the result type
    Token token;
    SourceLocation location = node->getLocation();

    // Replace the binary expression with a literal node
    if (std::holds_alternative<int64_t>(resultValue)) {
      int64_t intResult = std::get<int64_t>(resultValue);
      token = Token(TokenType::INTEGER, std::to_string(intResult), location);
      auto literal = std::make_unique<LiteralExprNode>(token, intResult);

      // Replace the node in the parent's structure
      replaceExpressionNode(node, std::move(literal));
      return true;
    } else if (std::holds_alternative<double>(resultValue)) {
      double doubleResult = std::get<double>(resultValue);
      token = Token(TokenType::FLOAT, std::to_string(doubleResult), location);
      auto literal = std::make_unique<LiteralExprNode>(token, doubleResult);

      // Replace the node in the parent's structure
      replaceExpressionNode(node, std::move(literal));
      return true;
    } else if (std::holds_alternative<std::string>(resultValue)) {
      std::string strResult = std::get<std::string>(resultValue);
      token = Token(TokenType::STRING, "\"" + strResult + "\"", location);
      auto literal = std::make_unique<LiteralExprNode>(token, strResult);

      // Replace the node in the parent's structure
      replaceExpressionNode(node, std::move(literal));
      return true;
    } else if (std::holds_alternative<bool>(resultValue)) {
      bool boolResult = std::get<bool>(resultValue);
      token = Token(boolResult ? TokenType::TRUE : TokenType::FALSE,
                    boolResult ? "true" : "false", location);
      auto literal = std::make_unique<LiteralExprNode>(token, boolResult);

      // Replace the node in the parent's structure
      replaceExpressionNode(node, std::move(literal));
      return true;
    }
  }

  return false;
}

bool ConstantFoldingPass::tryFoldUnaryExpr(UnaryExprNode *node) {
  // Check if the operand is a literal
  LiteralExprNode *operandLiteral =
      dynamic_cast<LiteralExprNode *>(&node->getRight());

  if (!operandLiteral) {
    return false;
  }

  // Get the literal value
  auto value = operandLiteral->getValue();

  // Try to fold based on the operator type
  TokenType op = node->getOperator().getType();

  // Result value
  std::variant<int64_t, double, std::string, bool> resultValue;
  bool folded = false;

  switch (op) {
  case TokenType::MINUS:
    // Numeric negation (operand must be numeric)
    if (std::holds_alternative<int64_t>(value)) {
      resultValue = -std::get<int64_t>(value);
      folded = true;
    } else if (std::holds_alternative<double>(value)) {
      resultValue = -std::get<double>(value);
      folded = true;
    }
    break;

  case TokenType::BANG:
    // Boolean negation (operand should be boolean)
    if (std::holds_alternative<bool>(value)) {
      resultValue = !std::get<bool>(value);
      folded = true;
    }
    // Also handle numeric values for boolean context
    else if (std::holds_alternative<int64_t>(value)) {
      resultValue = std::get<int64_t>(value) == 0;
      folded = true;
    } else if (std::holds_alternative<double>(value)) {
      resultValue = std::get<double>(value) == 0.0;
      folded = true;
    }
    break;

  default:
    // Operator not supported or not foldable
    break;
  }

  // If we successfully folded the expression, replace it with a literal node
  if (folded) {
    // Create a token for the literal based on the result type
    Token token;
    SourceLocation location = node->getLocation();

    // Replace the unary expression with a literal node
    if (std::holds_alternative<int64_t>(resultValue)) {
      int64_t intResult = std::get<int64_t>(resultValue);
      token = Token(TokenType::INTEGER, std::to_string(intResult), location);
      auto literal = std::make_unique<LiteralExprNode>(token, intResult);

      // Replace the node in the parent's structure
      replaceExpressionNode(node, std::move(literal));
      return true;
    } else if (std::holds_alternative<double>(resultValue)) {
      double doubleResult = std::get<double>(resultValue);
      token = Token(TokenType::FLOAT, std::to_string(doubleResult), location);
      auto literal = std::make_unique<LiteralExprNode>(token, doubleResult);

      // Replace the node in the parent's structure
      replaceExpressionNode(node, std::move(literal));
      return true;
    } else if (std::holds_alternative<bool>(resultValue)) {
      bool boolResult = std::get<bool>(resultValue);
      token = Token(boolResult ? TokenType::TRUE : TokenType::FALSE,
                    boolResult ? "true" : "false", location);
      auto literal = std::make_unique<LiteralExprNode>(token, boolResult);

      // Replace the node in the parent's structure
      replaceExpressionNode(node, std::move(literal));
      return true;
    }
  }

  return false;
}

std::variant<int64_t, double, std::string, bool>
ConstantFoldingPass::evaluateLiteral(const LiteralExprNode *node) {
  return node->getValue();
}

void ConstantFoldingPass::replaceLiteral(
    AstNode *parent, ExprNode **expr,
    std::variant<int64_t, double, std::string, bool> value) {
  // This would replace the expression with a new literal node
  // For now, this is just a placeholder
}

void ConstantFoldingPass::replaceExpressionNode(
    ExprNode *oldNode, std::unique_ptr<ExprNode> newNode) {
  // Store the new node in our replacements map
  // The AST traversal will handle the actual replacement later
  expressionReplacements[oldNode] = std::move(newNode);
}

void ConstantFoldingPass::replaceStatementNode(
    StmtNode *oldNode, std::unique_ptr<StmtNode> newNode) {
  // Store the new node in our replacements map
  // The AST traversal will handle the actual replacement later
  statementReplacements[oldNode] = std::move(newNode);
}

} // namespace compiler
} // namespace chronovyan