#include <iostream>
#include <memory>

#include "../src/compiler/include/compiler/AstNode.h"
#include "../src/compiler/include/compiler/AstPrinter.h"
#include "../src/compiler/include/compiler/Token.h"

using namespace chronovyan::compiler;

int main() {
    // Create a simple AST
    // Basic structure:
    // Program
    //   VarDecl: x
    //     Type: int
    //     Initializer: Literal: 42
    //   StmtDecl
    //     ExprStmt
    //       Binary: +
    //         Variable: x
    //         Literal: 10

    // Create source locations
    SourceLocation loc("test.chron", 1, 1);

    // Create tokens
    Token varNameToken(TokenType::IDENTIFIER, "x", loc);
    Token intTypeToken(TokenType::IDENTIFIER, "int", loc);
    Token numberToken(TokenType::INTEGER, "42", loc, 42.0);
    Token plusToken(TokenType::PLUS, "+", loc);
    Token tenToken(TokenType::INTEGER, "10", loc, 10.0);

    // Create the AST nodes
    auto intType = std::make_unique<BasicTypeNode>(intTypeToken);
    auto numberLiteral = std::make_unique<LiteralExprNode>(numberToken);
    auto varDecl =
        std::make_unique<VarDeclNode>(varNameToken, std::move(intType), std::move(numberLiteral));

    auto varExpr = std::make_unique<VariableExprNode>(varNameToken);
    auto tenLiteral = std::make_unique<LiteralExprNode>(tenToken);
    auto binaryExpr =
        std::make_unique<BinaryExprNode>(std::move(varExpr), plusToken, std::move(tenLiteral));
    auto exprStmt = std::make_unique<ExprStmtNode>(std::move(binaryExpr));
    auto stmtDecl = std::make_unique<StmtDeclNode>(std::move(exprStmt));

    std::vector<std::unique_ptr<DeclNode>> statements;
    statements.push_back(std::move(varDecl));
    statements.push_back(std::move(stmtDecl));

    auto program = std::make_unique<ProgramNode>(std::move(statements));

    // Print the AST
    AstPrinter printer;
    program->accept(printer);

    return 0;
}