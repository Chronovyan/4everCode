#include <iostream>
#include <string>
#include <vector>

// Simple Lexer class for testing character literals
class Lexer {
    std::string source;
    size_t position = 0;
    size_t line = 1;
    size_t column = 1;

    char advance() {
        if (isAtEnd())
            return '\0';
        char c = source[position++];
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        return c;
    }

    char peek() const { return isAtEnd() ? '\0' : source[position]; }

    bool isAtEnd() const { return position >= source.length(); }

    void skipWhitespace() {
        while (!isAtEnd() && (peek() == ' ' || peek() == '\t' || peek() == '\r')) {
            advance();
        }
    }

    std::string getLexeme(size_t start, size_t end) const {
        return source.substr(start, end - start);
    }

    std::string debugChar(char c) {
        if (c == '\n')
            return "\\n";
        if (c == '\t')
            return "\\t";
        if (c == '\\')
            return "\\\\";
        if (c == '\'')
            return "\\'";
        return std::string(1, c);
    }

public:
    struct Token {
        enum Type { CHARACTER, ERROR, END } type;
        std::string lexeme;
        char value;
        std::string error;

        Token(Type t, const std::string& l, char v = '\0') : type(t), lexeme(l), value(v) {}

        Token(const std::string& err) : type(ERROR), error(err) {}
    };

    Lexer(const std::string& src) : source(src) {}

    Token scanToken() {
        skipWhitespace();
        if (isAtEnd())
            return {Token::END, ""};

        char c = advance();

        // Handle character literals
        if (c == '\'') {
            return handleCharacter();
        }

        // Skip other tokens for this test
        return {Token::END, ""};
    }

    Token handleCharacter() {
        std::cout << "=== Starting character literal processing ===\n";
        size_t start = position - 1;  // Position of the opening single quote

        if (isAtEnd()) {
            std::cout << "Error: Reached end of input after opening quote\n";
            return {"Unterminated character literal: missing closing quote"};
        }

        // Check for empty character literal (two quotes in a row)
        if (peek() == '\'') {
            advance();  // Skip the closing quote
            return {"Empty character literal"};
        }

        // Get the character
        char c = advance();

        // Handle escape sequences
        if (c == '\\') {
            if (isAtEnd()) {
                return {"Unterminated escape sequence"};
            }

            char next = advance();
            switch (next) {
                case 'n':
                    c = '\n';
                    break;
                case 't':
                    c = '\t';
                    break;
                case '\\':
                    c = '\\';
                    break;
                case '\'':
                    c = '\'';
                    break;
                case 'x': {
                    // Simple hex escape (\xHH)
                    if (isAtEnd() || !isxdigit(peek())) {
                        return {"Invalid hex escape sequence"};
                    }
                    // Just consume two hex digits for this simple test
                    advance();  // Skip first digit
                    if (!isAtEnd() && isxdigit(peek())) {
                        advance();  // Skip second digit if present
                    }
                    c = 'x';  // Just use 'x' as a placeholder for this test
                    break;
                }
                default:
                    return {"Unrecognized escape sequence"};
            }
        }

        // Expect closing quote
        if (isAtEnd() || peek() != '\'') {
            std::cout << "Error: Missing closing quote\n";
            return {"Missing closing quote in character literal"};
        }

        // Skip the closing quote
        advance();

        std::string lexeme = source.substr(start, position - start);
        std::cout << "Successfully parsed character literal: " << lexeme
                  << ", value: " << debugChar(c) << " (" << static_cast<int>(c) << ")\n";
        std::cout << "=== End of character literal processing ===\n\n";

        return {Token::CHARACTER, lexeme, c};
    }
};

void testCharacterLiterals() {
    std::cout << "=== Testing Character Literals ===\n\n";

    // Test cases
    std::vector<std::string> testCases = {
        "'a'",      // Simple character
        "'\\n'",    // Newline
        "'\\t'",    // Tab
        "'\\\\'",   // Backslash
        "'\\''",    // Single quote
        "'\\x41'",  // Hex value 'A'
        "''",       // Empty (error)
        "'ab'"      // Multi-character (error)
    };

    for (const auto& testCase : testCases) {
        std::cout << "Testing: " << testCase << "\n";

        Lexer lexer(testCase);
        auto token = lexer.scanToken();

        switch (token.type) {
            case Lexer::Token::CHARACTER:
                std::cout << "  SUCCESS: Character literal: " << token.lexeme
                          << " (value: " << static_cast<int>(token.value) << ")\n";
                break;

            case Lexer::Token::ERROR:
                std::cout << "  ERROR: " << token.error << "\n";
                break;

            default:
                std::cout << "  UNEXPECTED TOKEN TYPE\n";
        }

        std::cout << std::endl;
    }
}

int main() {
    testCharacterLiterals();
    return 0;
}
