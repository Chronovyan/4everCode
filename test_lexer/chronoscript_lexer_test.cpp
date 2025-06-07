#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../src/compiler/include/compiler/Diagnostic.h"
#include "../src/compiler/include/compiler/Lexer.h"
#include "../src/compiler/include/compiler/Token.h"

using namespace chronovyan::compiler;

class ChronoscriptLexerTest : public ::testing::Test {
protected:
    std::shared_ptr<DiagnosticReporter> reporter;

    void SetUp() override { reporter = std::make_shared<DiagnosticReporter>(); }

    std::vector<Token> tokenize(const std::string& source) {
        std::istringstream stream(source);
        std::string sourceStr = stream.str();
        Lexer lexer(sourceStr, "test.chron", reporter);
        std::vector<Token> tokens;

        Token token;
        do {
            token = lexer.nextToken();
            tokens.push_back(token);
        } while (token.getType() != TokenType::END_OF_FILE);

        return tokens;
    }

    void expectTokens(const std::string& source, const std::vector<TokenType>& expectedTypes) {
        auto tokens = tokenize(source);
        ASSERT_GE(tokens.size(), expectedTypes.size()) << "Not enough tokens generated";

        for (size_t i = 0; i < expectedTypes.size(); ++i) {
            EXPECT_EQ(tokens[i].getType(), expectedTypes[i])
                << "Token mismatch at position " << i
                << ". Expected: " << tokenTypeToString(expectedTypes[i])
                << ", got: " << tokenTypeToString(tokens[i].getType());
        }

        if (expectedTypes.size() < tokens.size()) {
            EXPECT_EQ(tokens[expectedTypes.size()].getType(), TokenType::END_OF_FILE)
                << "Expected END_OF_FILE after " << expectedTypes.size() << " tokens";
        }
    }
};

// Test Chronoscript block types
TEST_F(ChronoscriptLexerTest, BlockTypes) {
    expectTokens("ANTECEDENCE", {TokenType::ANTECEDENCE});
    expectTokens("CONCURRENCY", {TokenType::CONCURRENCY});
    expectTokens("CONSEQUENCE", {TokenType::CONSEQUENCE});
}

// Test Chronoscript declaration keywords
TEST_F(ChronoscriptLexerTest, DeclarationKeywords) {
    expectTokens("DECLARE", {TokenType::DECLARE});
    expectTokens("CONF", {TokenType::CONF});
    expectTokens("REB", {TokenType::REB});
    expectTokens("STATIC", {TokenType::STATIC});
    expectTokens("FLUX", {TokenType::FLUX});
    expectTokens("ANCHOR", {TokenType::ANCHOR});
    expectTokens("SOURCE_INFUSED", {TokenType::SOURCE_INFUSED});
}

// Test Chronoscript temporal types
TEST_F(ChronoscriptLexerTest, TemporalTypes) {
    expectTokens("WEAVE_PATTERN", {TokenType::WEAVE_PATTERN});
    expectTokens("TEMPORAL_SIGNATURE", {TokenType::TEMPORAL_SIGNATURE});
    expectTokens("AETHEL_RESERVE", {TokenType::AETHEL_RESERVE});
    expectTokens("CHRONON_STREAM", {TokenType::CHRONON_STREAM});
}

// Test Chronoscript temporal operations
TEST_F(ChronoscriptLexerTest, TemporalOperations) {
    expectTokens("INFUSE", {TokenType::INFUSE});
    expectTokens("REWIND_TO", {TokenType::REWIND_TO});
    expectTokens("GUARD", {TokenType::GUARD});
}

// Test Chronoscript control flow
TEST_F(ChronoscriptLexerTest, ControlFlow) {
    expectTokens("CATCH_TEMPORAL", {TokenType::CATCH_TEMPORAL});
    expectTokens("DEJAVU", {TokenType::DEJAVU});
    expectTokens("LOOT_TABLE", {TokenType::LOOT_TABLE});
}

// Test Chronoscript constants
TEST_F(ChronoscriptLexerTest, Constants) {
    expectTokens("PARADOX_LEVEL", {TokenType::PARADOX_LEVEL});
}

// Test a complete Chronoscript example
TEST_F(ChronoscriptLexerTest, CompleteExample) {
    const char* source = R"(
        ANTECEDENCE {
            DECLARE CONF::STATIC Stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
                INFUSE(Stability_Field, Time_Crystals, "Restabilize"),
                GUARD(Stability_Field > 0.6, "Low stability")
            ]);
        }
    )";

    std::vector<TokenType> expected = {
        TokenType::ANTECEDENCE, TokenType::LEFT_BRACE,    TokenType::DECLARE,
        TokenType::CONF,        TokenType::COLON_COLON,   TokenType::STATIC,
        TokenType::IDENTIFIER,  // Stabilization
        TokenType::COLON,       TokenType::WEAVE_PATTERN, TokenType::EQUAL,
        TokenType::IDENTIFIER,  // define_pattern
        TokenType::LEFT_PAREN,
        TokenType::STRING,  // "Stabilize"
        TokenType::COMMA,       TokenType::LEFT_BRACKET,  TokenType::INFUSE,
        TokenType::LEFT_PAREN,
        TokenType::IDENTIFIER,  // Stability_Field
        TokenType::COMMA,
        TokenType::IDENTIFIER,  // Time_Crystals
        TokenType::COMMA,
        TokenType::STRING,  // "Restabilize"
        TokenType::RIGHT_PAREN, TokenType::COMMA,         TokenType::GUARD,
        TokenType::LEFT_PAREN,
        TokenType::IDENTIFIER,  // Stability_Field
        TokenType::GREATER,
        TokenType::FLOAT,  // 0.6
        TokenType::COMMA,
        TokenType::STRING,  // "Low stability"
        TokenType::RIGHT_PAREN, TokenType::RIGHT_BRACKET, TokenType::RIGHT_PAREN,
        TokenType::SEMICOLON,   TokenType::RIGHT_BRACE,   TokenType::END_OF_FILE};

    expectTokens(source, expected);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
