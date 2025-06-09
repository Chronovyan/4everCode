#include <cassert>

#include "ast/AstBuilder.h"
#include "compiler/AstNode.h"
#include "compiler/AstVisitor.h"
#include "compiler/Token.h"

namespace chronovyan {
namespace compiler {

AstBuilder::AstBuilder() {}

DiagnosticReporter &AstBuilder::getDiagnosticReporter() {
  return m_diagnostics;
}

std::unique_ptr<ExpressionNode>
AstBuilder::createLiteral(const std::string &value) {
  // Create a token for the literal
  Token token;

  // Determine the type of literal based on the value
  if (value == "true" || value == "false") {
    token = Token(value == "true" ? TokenType::TRUE : TokenType::FALSE, value,
                  SourceLocation());
    return std::make_unique<LiteralExprNode>(token, value == "true");
  } else if (value.find_first_of('.') != std::string::npos) {
    // Assume it's a floating-point number
    token = Token(TokenType::FLOAT, value, SourceLocation());
    try {
      double doubleValue = std::stod(value);
      return std::make_unique<LiteralExprNode>(token, doubleValue);
    } catch (const std::exception &e) {
      m_diagnostics.addError(SourceLocation(),
                             "Invalid float literal: " + value);
      return nullptr;
    }
  } else if (value.front() == '"' && value.back() == '"') {
    // String literal
    token = Token(TokenType::STRING, value, SourceLocation());
    // Remove the quotes from the value
    std::string stringValue = value.substr(1, value.length() - 2);
    return std::make_unique<LiteralExprNode>(token, stringValue);
  } else {
    // Assume it's an integer
    token = Token(TokenType::INTEGER, value, SourceLocation());
    try {
      int intValue = std::stoi(value);
      return std::make_unique<LiteralExprNode>(token, intValue);
    } catch (const std::exception &e) {
      m_diagnostics.addError(SourceLocation(),
                             "Invalid integer literal: " + value);
      return nullptr;
    }
  }
}

std::unique_ptr<ExpressionNode>
AstBuilder::createVariable(const std::string &name) {
  // Create a token for the variable name
  Token token(TokenType::IDENTIFIER, name, SourceLocation());
  return std::make_unique<VariableExprNode>(token);
}

std::unique_ptr<ExpressionNode>
AstBuilder::createBinaryExpr(std::unique_ptr<ExpressionNode> left,
                             const std::string &op,
                             std::unique_ptr<ExpressionNode> right) {
  // Create a token for the operator
  TokenType opType;
  if (op == "+")
    opType = TokenType::PLUS;
  else if (op == "-")
    opType = TokenType::MINUS;
  else if (op == "*")
    opType = TokenType::STAR;
  else if (op == "/")
    opType = TokenType::SLASH;
  else if (op == "==")
    opType = TokenType::EQUAL_EQUAL;
  else if (op == "!=")
    opType = TokenType::BANG_EQUAL;
  else if (op == "<")
    opType = TokenType::LESS;
  else if (op == "<=")
    opType = TokenType::LESS_EQUAL;
  else if (op == ">")
    opType = TokenType::GREATER;
  else if (op == ">=")
    opType = TokenType::GREATER_EQUAL;
  else if (op == "&&")
    opType = TokenType::AND;
  else if (op == "||")
    opType = TokenType::OR;
  else {
    m_diagnostics.addError(SourceLocation(),
                           "Unsupported binary operator: " + op);
    return nullptr;
  }

  Token opToken(opType, op, SourceLocation());
  return std::make_unique<BinaryExprNode>(std::move(left), opToken,
                                          std::move(right));
}

std::unique_ptr<ExpressionNode>
AstBuilder::createCall(std::unique_ptr<ExpressionNode> callee,
                       std::vector<std::unique_ptr<ExpressionNode>> arguments) {
  return std::make_unique<CallExprNode>(std::move(callee), std::move(arguments),
                                        SourceLocation());
}

std::unique_ptr<StatementNode>
AstBuilder::createVariableDecl(const std::string &name, const std::string &type,
                               std::unique_ptr<ExpressionNode> initializer) {
  // Create tokens for name and type
  Token nameToken(TokenType::IDENTIFIER, name, SourceLocation());

  // Create type node if type is provided
  std::unique_ptr<TypeNode> typeNode = nullptr;
  if (!type.empty()) {
    Token typeToken(TokenType::IDENTIFIER, type, SourceLocation());
    typeNode = std::make_unique<BasicTypeNode>(typeToken);
  }

  return std::make_unique<VarDeclNode>(nameToken, std::move(typeNode),
                                       std::move(initializer));
}

std::unique_ptr<StatementNode> AstBuilder::createBlock(
    std::vector<std::unique_ptr<StatementNode>> statements) {
  return std::make_unique<BlockStmtNode>(std::move(statements),
                                         SourceLocation());
}

std::unique_ptr<StatementNode>
AstBuilder::createIf(std::unique_ptr<ExpressionNode> condition,
                     std::unique_ptr<StatementNode> thenBranch,
                     std::unique_ptr<StatementNode> elseBranch) {
  return std::make_unique<IfStmtNode>(std::move(condition),
                                      std::move(thenBranch),
                                      std::move(elseBranch), SourceLocation());
}

std::unique_ptr<StatementNode>
AstBuilder::createFor(std::unique_ptr<StatementNode> initializer,
                      std::unique_ptr<ExpressionNode> condition,
                      std::unique_ptr<ExpressionNode> increment,
                      std::unique_ptr<StatementNode> body) {
  return std::make_unique<ForStmtNode>(
      std::move(initializer), std::move(condition), std::move(increment),
      std::move(body), SourceLocation());
}

std::unique_ptr<StatementNode>
AstBuilder::createWhile(std::unique_ptr<ExpressionNode> condition,
                        std::unique_ptr<StatementNode> body) {
  return std::make_unique<WhileStmtNode>(std::move(condition), std::move(body),
                                         SourceLocation());
}

std::unique_ptr<StatementNode> AstBuilder::createFunctionDecl(
    const std::string &name, const std::vector<std::string> &params,
    const std::string &returnType, std::unique_ptr<StatementNode> body) {
  // Create token for function name
  Token nameToken(TokenType::IDENTIFIER, name, SourceLocation());

  // Create parameter tokens
  std::vector<Token> paramTokens;
  for (const auto &param : params) {
    paramTokens.push_back(
        Token(TokenType::IDENTIFIER, param, SourceLocation()));
  }

  // Create return type node if provided
  std::unique_ptr<TypeNode> returnTypeNode = nullptr;
  if (!returnType.empty()) {
    Token returnTypeToken(TokenType::IDENTIFIER, returnType, SourceLocation());
    returnTypeNode = std::make_unique<BasicTypeNode>(returnTypeToken);
  }

  return std::make_unique<FunctionDeclNode>(
      nameToken, paramTokens, std::move(returnTypeNode), std::move(body));
}

std::unique_ptr<ProgramNode> AstBuilder::createProgram(
    std::vector<std::unique_ptr<StatementNode>> statements) {
  return std::make_unique<ProgramNode>(std::move(statements));
}

} // namespace compiler
} // namespace chronovyan