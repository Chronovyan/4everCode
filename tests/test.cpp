#include <iostream>
#include <memory>

#include "compiler/AstNode.h"
#include "compiler/AstPrinter.h"
#include "compiler/Token.h"

using namespace chronovyan::compiler;

int main() {
    // Create a simple AST
    // Basic structure:
    // Program
    //   VarDecl: x
    //     Type: int
    //     Initializer: Literal: 42
    //   ExprStmt
    //     Binary: +
    //       Variable: x
    //       Literal: 10

    // Create tokens
    Token varNameToken(TokenType::IDENTIFIER, "x", {}, 1);
    Token intTypeToken(TokenType::IDENTIFIER, "int", {}, 1);
    Token numberToken(TokenType::NUMBER, "42", 42.0, 1);
    Token plusToken(TokenType::PLUS, "+", {}, 1);
    Token tenToken(TokenType::NUMBER, "10", 10.0, 1);

    // Create the AST nodes
    auto intType = std::make_shared<BasicTypeNode>(intTypeToken);
    auto numberLiteral = std::make_shared<LiteralExprNode>(numberToken);
    auto varDecl = std::make_shared<VarDeclNode>(varNameToken, intType, numberLiteral);

    auto varExpr = std::make_shared<VariableExprNode>(varNameToken);
    auto tenLiteral = std::make_shared<LiteralExprNode>(tenToken);
    auto binaryExpr = std::make_shared<BinaryExprNode>(varExpr, plusToken, tenLiteral);
    auto exprStmt = std::make_shared<ExprStmtNode>(binaryExpr);

    std::vector<std::shared_ptr<DeclNode>> statements;
    statements.push_back(varDecl);
    statements.push_back(exprStmt);

    auto program = std::make_shared<ProgramNode>(statements);

    // Print the AST
    AstPrinter printer;
    program->accept(printer);

    return 0;
}
