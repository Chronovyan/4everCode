#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>
#include <variant>

#include "../include/ast_nodes.h"
#include "../include/interpreter.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/source_file.h"
#include "../include/value.h"

using namespace chronovyan;

// Helper function to parse a string and return the AST
std::unique_ptr<ProgramNode> parseString(const std::string& source) {
    auto sourceFile = std::make_shared<SourceFile>(std::string(source), "test.cvy");
    auto lexer = std::make_shared<Lexer>(sourceFile);
    Parser parser(lexer);
    return parser.parse();
}

// Test Lexer Functionality
TEST(ChronovyanComponentTest, LexerBasicTokenization) {
    std::string source = "var x = 42;";

    auto sourceFile = std::make_shared<SourceFile>(std::string(source), "test.cvy");
    auto lexer = std::make_shared<Lexer>(sourceFile);

    std::vector<Token> tokens = lexer->tokenizeAll();

    // Debug output to see what tokens are being generated
    std::cout << "Number of tokens: " << tokens.size() << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "Token[" << i << "]: Type=" << static_cast<int>(tokens[i].type) << " Lexeme=\""
                  << tokens[i].lexeme << "\"" << std::endl;
    }

    // Check that we have the expected number of tokens (var, x, =, 42, ;, EOF)
    ASSERT_EQ(tokens.size(), 6);

    // Check that each token is correct
    EXPECT_EQ(tokens[0].type, TokenType::VAR);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "x");
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER_LITERAL);
    EXPECT_EQ(tokens[3].lexeme, "42");
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].type, TokenType::EOF_TOKEN);
}

// Test Parser Functionality
TEST(ChronovyanComponentTest, ParserBasicParsing) {
    std::string source = "var x = 42;";

    auto program = parseString(source);

    // Check that we have a program
    ASSERT_NE(program, nullptr);

    // Check that we have one statement
    ASSERT_EQ(program->getStatements().size(), 1);

    // Check that the statement is a variable declaration
    auto varDecl = dynamic_cast<VariableDeclStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(varDecl, nullptr);

    // Check that the variable name is correct
    EXPECT_EQ(varDecl->getName(), "x");

    // Check that the variable has an initializer
    ASSERT_TRUE(varDecl->hasInitializer());

    // Check that the initializer is a literal with value 42
    auto literal = dynamic_cast<const LiteralExprNode*>(&varDecl->getInitializer());
    ASSERT_NE(literal, nullptr);
    ASSERT_TRUE(std::holds_alternative<int64_t>(literal->getValue()));
    EXPECT_EQ(std::get<int64_t>(literal->getValue()), 42);
}

// Test Interpreter Functionality
TEST(ChronovyanComponentTest, InterpreterBasicExecution) {
    std::string source = R"(
        var x = 42;
        var y = "Hello, World!";
        var z = x + 8;
        var flag = z > 45;
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);

    Interpreter interpreter;
    interpreter.interpret(*program);

    // Check that variables were created correctly
    auto globals = interpreter.getGlobals();

    // Check x
    ASSERT_TRUE(globals->exists("x"));
    Value x = globals->get("x");
    ASSERT_TRUE(x.isInt());
    EXPECT_EQ(x.asInt(), 42);

    // Check y
    ASSERT_TRUE(globals->exists("y"));
    Value y = globals->get("y");
    ASSERT_TRUE(y.isString());
    EXPECT_EQ(y.asString(), "Hello, World!");

    // Check z (computed value)
    ASSERT_TRUE(globals->exists("z"));
    Value z = globals->get("z");
    ASSERT_TRUE(z.isInt());
    EXPECT_EQ(z.asInt(), 50);  // 42 + 8 = 50

    // Check flag (boolean expression result)
    ASSERT_TRUE(globals->exists("flag"));
    Value flag = globals->get("flag");
    ASSERT_TRUE(flag.isBoolean());
    EXPECT_EQ(flag.asBool(), true);  // 50 > 45 is true
}

// Test more complex code with control flow
TEST(ChronovyanComponentTest, ParserControlFlow) {
    std::string source = R"(
        if (true) {
            var result = 42;
        } else {
            var result = 0;
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto ifStmt = dynamic_cast<IfStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(ifStmt, nullptr);

    // Check the condition is a literal true value
    auto condition = dynamic_cast<const LiteralExprNode*>(&ifStmt->getCondition());
    ASSERT_NE(condition, nullptr);
    ASSERT_TRUE(std::holds_alternative<bool>(condition->getValue()));
    EXPECT_EQ(std::get<bool>(condition->getValue()), true);

    // Check that it has both then and else branches
    ASSERT_TRUE(ifStmt->hasElseBranch());

    // Verify then branch
    auto thenBranch = dynamic_cast<const BlockStmtNode*>(&ifStmt->getThenBranch());
    ASSERT_NE(thenBranch, nullptr);
    ASSERT_EQ(thenBranch->getStatements().size(), 1);

    // Verify else branch
    auto elseBranch = dynamic_cast<const BlockStmtNode*>(&ifStmt->getElseBranch());
    ASSERT_NE(elseBranch, nullptr);
    ASSERT_EQ(elseBranch->getStatements().size(), 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}