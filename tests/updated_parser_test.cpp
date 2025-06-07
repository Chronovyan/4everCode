#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <variant>

#include "../include/ast_nodes.h"
#include "../include/interpreter.h"
#include "../include/parser.h"
#include "../include/source_file.h"
#include "../include/value.h"
#include "../src/compiler/include/compiler/Lexer.h"

using namespace chronovyan;

// Helper function to parse a string and return the AST
std::unique_ptr<ProgramNode> parseString(const std::string& source) {
    auto sourceFile = std::make_shared<SourceFile>(std::string(source), "test.cvy");
    auto lexer = std::make_shared<Lexer>(sourceFile);
    Parser parser(lexer);
    return parser.parse();
}

// Test simple lexer functionality
TEST(SimpleParserTest, LexerTokenization) {
    // Create a simple Chronovyan source string
    std::string source = "var x = 42;";

    // Create a lexer
    auto sourceFile = std::make_shared<SourceFile>("test.cvy", source);
    auto lexer = std::make_shared<Lexer>(sourceFile);

    // Tokenize the source
    auto tokens = lexer->tokenizeAll();

    // Check that we have the correct number of tokens (var, x, =, 42, ;, EOF)
    ASSERT_EQ(tokens.size(), 6);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::VAR);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "x");
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER_LITERAL);
    EXPECT_EQ(tokens[3].lexeme, "42");
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].type, TokenType::EOF_TOKEN);
}

// Test simple parser functionality
TEST(SimpleParserTest, ParserBasicExpression) {
    // Create a simple Chronovyan source string
    std::string source = "var x = 42;";

    // Parse the source
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

    // Check that the initializer is a literal
    auto literal = dynamic_cast<const LiteralExprNode*>(&varDecl->getInitializer());
    ASSERT_NE(literal, nullptr);

    // Check that the literal value is 42
    ASSERT_TRUE(std::holds_alternative<int64_t>(literal->getValue()));
    EXPECT_EQ(std::get<int64_t>(literal->getValue()), 42);
}

// Test simple interpreter functionality
TEST(SimpleParserTest, InterpreterBasicExecution) {
    // Create a simple Chronovyan source string
    std::string source = "var x = 42;";

    // Parse the source
    auto program = parseString(source);

    // Create an interpreter
    Interpreter interpreter;

    // Execute the program
    interpreter.interpret(*program);

    // Check that the variable x was created with value 42
    auto env = interpreter.getGlobals();
    ASSERT_TRUE(env->exists("x"));

    Value value = env->get("x");
    ASSERT_TRUE(value.isInt());
    EXPECT_EQ(value.asInt(), 42);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}