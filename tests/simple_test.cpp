#include <iostream>
#include <memory>

#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/source_file.h"
#include "../include/token.h"
#include "../src/compiler/include/compiler/AstNode.h"
#include "../src/compiler/include/compiler/AstPrinter.h"
#include "../src/compiler/include/compiler/Token.h"

using namespace chronovyan;
using namespace chronovyan::compiler;

// Simple test for the lexer
void testLexer() {
    std::cout << "=== Testing Lexer ===" << std::endl;

    // Create a simple source file
    std::string source = "var x = 42;";
    auto sourceFile = std::make_shared<SourceFile>(std::string(source), "test.chron");

    // Create a lexer
    auto lexer = std::make_shared<Lexer>(sourceFile);

    // Tokenize all
    auto tokens = lexer->tokenizeAll();

    // Print the tokens
    std::cout << "Tokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "  Type: " << static_cast<int>(token.type) << ", Lexeme: '" << token.lexeme
                  << "'" << std::endl;
    }

    // Basic assertions
    bool success = true;
    if (tokens.size() != 6) {
        std::cout << "FAIL: Expected 6 tokens, got " << tokens.size() << std::endl;
        success = false;
    }

    if (tokens[0].type != TokenType::VAR) {
        std::cout << "FAIL: First token should be VAR" << std::endl;
        success = false;
    }

    if (tokens[1].type != TokenType::IDENTIFIER || tokens[1].lexeme != "x") {
        std::cout << "FAIL: Second token should be IDENTIFIER 'x'" << std::endl;
        success = false;
    }

    if (success) {
        std::cout << "Lexer test PASSED" << std::endl;
    } else {
        std::cout << "Lexer test FAILED" << std::endl;
    }

    std::cout << std::endl;
}

// Simple test for the parser
void testParser() {
    std::cout << "=== Testing Parser ===" << std::endl;

    // Create a simple source file
    std::string source = "var x = 42;";
    auto sourceFile = std::make_shared<SourceFile>(std::string(source), "test.chron");

    // Create a lexer
    auto lexer = std::make_shared<Lexer>(sourceFile);

    // Create a parser
    Parser parser(lexer);

    // Parse the program
    auto program = parser.parse();

    // Basic assertions
    bool success = true;
    if (!program) {
        std::cout << "FAIL: Parser returned nullptr" << std::endl;
        success = false;
    } else {
        if (program->getStatements().size() != 1) {
            std::cout << "FAIL: Expected 1 statement, got " << program->getStatements().size()
                      << std::endl;
            success = false;
        }

        // Print the AST structure
        std::cout << "AST Structure:" << std::endl;
        std::cout << "  Program with " << program->getStatements().size() << " statements"
                  << std::endl;

        // Check first statement type
        auto stmt = program->getStatements()[0].get();
        std::cout << "  First statement type: " << typeid(*stmt).name() << std::endl;
    }

    if (success) {
        std::cout << "Parser test PASSED" << std::endl;
    } else {
        std::cout << "Parser test FAILED" << std::endl;
    }

    std::cout << std::endl;
}

// Helper function to parse a string and return the AST
std::unique_ptr<ProgramNode> parseString(const std::string& source) {
    auto sourceFile = std::make_shared<SourceFile>(std::string(source), "test.chron");
    auto lexer = std::make_shared<Lexer>(sourceFile);
    Parser parser(lexer);
    return parser.parse();
}

int main(int argc, char** argv) {
    std::cout << "Running simple Chronovyan tests" << std::endl;
    std::cout << "===============================" << std::endl << std::endl;

    testLexer();
    testParser();

    // Test a simple IF statement
    std::string source = "if (x > 5) { output(\"x is greater than 5\"); }";

    auto sourceFile = std::make_shared<SourceFile>(std::string(source), "test.chron");
    auto lexer = std::make_shared<Lexer>(sourceFile);

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