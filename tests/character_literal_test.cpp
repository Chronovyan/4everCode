#include <gtest/gtest.h>

#include <sstream>

#include "compiler/DiagnosticReporter.h"
#include "compiler/Lexer.h"

using namespace chronovyan::compiler;

class LexerCharacterTest : public ::testing::Test {
protected:
    std::shared_ptr<DiagnosticReporter> reporter;

    void SetUp() override {
        reporter = std::make_shared<DiagnosticReporter>();
        reporter->setDebugOutput(false);  // Disable debug output for cleaner test output
    }

    std::vector<Token> tokenize(const std::string& source) {
        Lexer lexer(source, "test.chr", reporter);
        return lexer.tokenize();
    }

    void expectCharacterToken(const Token& token, int expectedValue,
                              const std::string& expectedLexeme) {
        EXPECT_EQ(token.getType(), TokenType::CHARACTER);
        EXPECT_TRUE(token.hasValue());
        try {
            EXPECT_EQ(token.getAs<int>(), expectedValue);
        } catch (const std::exception& e) {
            ADD_FAILURE() << "Failed to get token value: " << e.what();
        }
        EXPECT_EQ(token.getLexeme(), expectedLexeme);
    }

    void expectErrorToken(const Token& token, const std::string& expectedError) {
        EXPECT_EQ(token.getType(), TokenType::ERROR);
        EXPECT_NE(token.getLexeme().find(expectedError), std::string::npos)
            << "Expected error message to contain: " << expectedError
            << " but got: " << token.getLexeme();
    }
};

TEST_F(LexerCharacterTest, SingleCharacter) {
    auto tokens = tokenize("'a'");
    ASSERT_GE(tokens.size(), 1);
    expectCharacterToken(tokens[0], 'a', "'a'");
}

TEST_F(LexerCharacterTest, NewlineEscape) {
    auto tokens = tokenize("'\\n'");
    ASSERT_GE(tokens.size(), 1);
    expectCharacterToken(tokens[0], '\n', "'\\n'");
}

TEST_F(LexerCharacterTest, TabEscape) {
    auto tokens = tokenize("'\\t'");
    ASSERT_GE(tokens.size(), 1);
    expectCharacterToken(tokens[0], '\t', "'\\t'");
}

TEST_F(LexerCharacterTest, BackslashEscape) {
    auto tokens = tokenize("'\\\\'");
    ASSERT_GE(tokens.size(), 1);
    expectCharacterToken(tokens[0], '\\', "'\\\\'");
}

TEST_F(LexerCharacterTest, SingleQuoteEscape) {
    auto tokens = tokenize("'\\''");
    ASSERT_GE(tokens.size(), 1);
    expectCharacterToken(tokens[0], '\'', "'\\''");
}

TEST_F(LexerCharacterTest, HexEscape) {
    auto tokens = tokenize("'\\x41'");  // 'A' in hex
    ASSERT_GE(tokens.size(), 1);
    expectCharacterToken(tokens[0], 0x41, "'\\x41'");
}

TEST_F(LexerCharacterTest, HexEscapeSingleDigit) {
    auto tokens = tokenize("'\\x1'");  // Should be valid
    ASSERT_GE(tokens.size(), 1);
    expectCharacterToken(tokens[0], 0x1, "'\\x1'");
}

TEST_F(LexerCharacterTest, EmptyCharacter) {
    auto tokens = tokenize("''");
    ASSERT_GE(tokens.size(), 1);
    expectErrorToken(tokens[0], "Empty character literal");
}

TEST_F(LexerCharacterTest, MissingClosingQuote) {
    auto tokens = tokenize("'a");
    ASSERT_GE(tokens.size(), 1);
    expectErrorToken(tokens[0], "Missing closing quote");
}

TEST_F(LexerCharacterTest, MultiCharacter) {
    auto tokens = tokenize("'ab'");
    ASSERT_GE(tokens.size(), 1);
    expectErrorToken(tokens[0], "must contain exactly one character");
}

TEST_F(LexerCharacterTest, IncompleteHexEscape) {
    auto tokens = tokenize("'\\x'");
    ASSERT_GE(tokens.size(), 1);
    expectErrorToken(tokens[0], "Invalid hex escape sequence");
}

TEST_F(LexerCharacterTest, InvalidEscape) {
    auto tokens = tokenize("'\\z'");
    ASSERT_GE(tokens.size(), 1);
    expectErrorToken(tokens[0], "Unrecognized escape sequence");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
