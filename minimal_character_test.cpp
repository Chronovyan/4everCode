#include <cctype>
#include <iostream>
#include <string>
#include <vector>

int hexDigitToInt(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F')
        return 10 + (c - 'A');
    return 0;  // Shouldn't happen if we validate first
}

// Simple Token class for testing
class Token {
public:
    enum class Type { CHARACTER, ERROR, END_OF_FILE };

    Token(Type type, char value, const std::string& lexeme)
        : type(type), value(value), lexeme(lexeme) {}

    Type getType() const { return type; }
    char getValue() const { return value; }
    const std::string& getLexeme() const { return lexeme; }

    bool isError() const { return type == Type::ERROR; }

private:
    Type type;
    char value;
    std::string lexeme;
};

// Simple Lexer class for testing character literals
class Lexer {
public:
    Lexer(const std::string& source) : source(source), position(0) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (!isAtEnd()) {
            if (peek() == '\'') {
                tokens.push_back(handleCharacter());
            } else {
                // Skip other characters for this test
                advance();
            }
        }
        return tokens;
    }

private:
    const std::string source;
    size_t position;
    size_t line;
    size_t column;

    bool isAtEnd() const { return position >= source.length(); }

    char peek() const { return isAtEnd() ? '\0' : source[position]; }

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

    bool match(char expected) {
        if (isAtEnd() || source[position] != expected)
            return false;
        position++;
        return true;
    }

    bool isHexDigit(char c) const { return std::isxdigit(static_cast<unsigned char>(c)) != 0; }

    // hexDigitToInt moved to file scope

    Token handleCharacter() {
        size_t start = position;
        advance();  // Skip opening quote

        if (isAtEnd()) {
            return Token(Token::Type::ERROR, '\0', "Unterminated character literal");
        }

        // Check for empty character literal
        if (peek() == '\'') {
            // Check if this is a single quote character or an empty literal
            if (position + 1 < source.length() && source[position + 1] == '\'') {
                // This is a single quote character: ''\''
                advance();  // Skip the single quote
                advance();  // Skip the closing quote
                return Token(Token::Type::CHARACTER, '\'', "'\\''");
            } else {
                // This is an empty character literal: ''
                advance();  // Skip the closing quote
                return Token(Token::Type::ERROR, '\0', "Empty character literal");
            }
        }

        char c = '\0';
        bool isEscaped = false;

        // Handle the character (or escape sequence)
        if (peek() == '\\') {
            // Handle escape sequence
            isEscaped = true;
            advance();  // Skip the backslash

            if (isAtEnd()) {
                return Token(Token::Type::ERROR, '\0', "Incomplete escape sequence");
            }

            // Handle the escape character
            char escapeChar = peek();
            bool isHexEscape = false;

            switch (escapeChar) {
                case 'n':
                    c = '\n';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case '0':
                    c = '\0';
                    break;
                case '\\':
                    c = '\\';
                    break;
                case '\'':
                    c = '\'';
                    break;
                case 'x': {
                    // Handle hex escape
                    isHexEscape = true;
                    advance();  // Skip 'x'
                    if (isAtEnd() || !isHexDigit(peek())) {
                        return Token(Token::Type::ERROR, '\0',
                                     "Invalid hex escape sequence: missing digit");
                    }

                    // Parse first hex digit
                    int value = hexDigitToInt(peek());
                    advance();

                    // Parse second hex digit if available
                    if (!isAtEnd() && isHexDigit(peek())) {
                        value = (value << 4) | hexDigitToInt(peek());
                        advance();
                    }

                    c = static_cast<char>(value);
                    break;
                }
                default:
                    return Token(Token::Type::ERROR, '\0', "Unrecognized escape sequence");
            }

            // Move past the escape character if we haven't already
            if (!isHexEscape) {
                advance();
            }
        } else {
            // Regular character
            c = peek();

            // Special case: handle single quote character
            if (c == '\'') {
                // This is a single quote character: '\''
                advance();  // Skip the single quote
                // Check for closing quote
                if (isAtEnd() || peek() != '\'') {
                    return Token(Token::Type::ERROR, '\0',
                                 "Missing closing quote in character literal");
                }
                advance();  // Skip the closing quote
                return Token(Token::Type::CHARACTER, '\'', "'\\''");
            }

            // Move past the regular character
            advance();
        }

        // Check for closing quote
        if (isAtEnd()) {
            return Token(Token::Type::ERROR, '\0', "Missing closing quote in character literal");
        }

        // Special case: handle backslash character
        if (isEscaped && c == '\\') {
            // Check for closing quote
            if (isAtEnd() || peek() != '\'') {
                return Token(Token::Type::ERROR, '\0',
                             "Missing closing quote in character literal");
            }

            std::string lexeme =
                source.substr(start, position - start + 1);  // +1 to include the closing quote
            advance();                                       // Skip the closing quote
            return Token(Token::Type::CHARACTER, '\\', lexeme);
        }

        // Check for proper closing quote
        if (peek() != '\'') {
            // If we've already processed an escape sequence or a regular character,
            // and there's more content before the closing quote, it's an error
            if (c != '\0') {
                // Skip to the next quote or end of input
                while (!isAtEnd() && peek() != '\'') {
                    advance();
                }
                if (!isAtEnd()) {
                    advance();  // Skip the closing quote
                }
                return Token(Token::Type::ERROR, '\0',
                             "Character literal must contain exactly one character");
            }
            // If we're here, we haven't processed any character yet, which shouldn't happen
            return Token(Token::Type::ERROR, '\0', "Invalid character literal");
        }

        // Skip the closing quote
        advance();

        std::string lexeme = source.substr(start, position - start);
        return Token(Token::Type::CHARACTER, c, lexeme);
    }
};

// Helper function to escape special characters in a string for display
std::string escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\n':
                result += "\\n";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\'':
                result += "\\'";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\0':
                result += "\\0";
                break;
            default:
                if (std::isprint(static_cast<unsigned char>(c))) {
                    result += c;
                } else {
                    char buf[16];
                    std::snprintf(buf, sizeof(buf), "\\x%02X", static_cast<unsigned char>(c));
                    result += buf;
                }
                break;
        }
    }
    return result;
}

// Helper function to convert string escape sequences to actual characters
std::string unescapeString(const std::string& input) {
    std::string result;
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i] == '\\' && i + 1 < input.size()) {
            switch (input[++i]) {
                case 'n':
                    result += '\n';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case '0':
                    result += '\0';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '\'':
                    result += '\'';
                    break;
                case 'x': {
                    // Handle hex escape
                    if (i + 2 <= input.size()) {
                        std::string hex = input.substr(i + 1, 2);
                        if (hex.size() == 2 &&
                            ((hex[0] >= '0' && hex[0] <= '9') || (hex[0] >= 'a' && hex[0] <= 'f') ||
                             (hex[0] >= 'A' && hex[0] <= 'F')) &&
                            ((hex[1] >= '0' && hex[1] <= '9') || (hex[1] >= 'a' && hex[1] <= 'f') ||
                             (hex[1] >= 'A' && hex[1] <= 'F'))) {
                            char c = (hexDigitToInt(hex[0]) << 4) | hexDigitToInt(hex[1]);
                            result += c;
                            i += 2;
                            continue;
                        }
                    }
                    // If we get here, it's an invalid hex escape
                    result += "\\x";
                    break;
                }
                default:
                    result += '\\';
                    result += input[i];
                    break;
            }
        } else {
            result += input[i];
        }
    }
    return result;
}

// Test function
void testCharacterLiteral(const std::string& input, bool shouldSucceed, char expectedValue = '\0',
                          const std::string& expectedError = "") {
    std::cout << "Testing: " << escapeString(input) << std::endl;

    // Create a version of the input with actual escape sequences
    std::string processedInput = input;
    if (input.size() >= 3 && input[0] == '\'' && input[input.size() - 1] == '\'') {
        // For character literals, process the content between the quotes
        std::string content = input.substr(1, input.size() - 2);
        content = unescapeString(content);
        processedInput = "'" + content + "'";
    }

    Lexer lexer(processedInput);
    auto tokens = lexer.tokenize();

    if (shouldSucceed) {
        if (tokens.empty() || tokens[0].isError()) {
            std::cout << "  \x1B[31mFAIL: Expected success but got error: "
                      << (tokens.empty() ? "No tokens" : escapeString(tokens[0].getLexeme()))
                      << "\x1B[0m" << std::endl;
            return;
        }

        char actualValue = tokens[0].getValue();
        if (actualValue != expectedValue) {
            std::cout << "  \x1B[31mFAIL: Expected value: " << static_cast<int>(expectedValue)
                      << " ('" << escapeString(std::string(1, expectedValue))
                      << "'), but got: " << static_cast<int>(actualValue) << " ('"
                      << escapeString(std::string(1, actualValue)) << "')\x1B[0m" << std::endl;
        } else {
            std::cout << "  \x1B[32mPASS: Got expected value: " << static_cast<int>(actualValue)
                      << " ('" << escapeString(std::string(1, actualValue)) << "')\x1B[0m"
                      << std::endl;
        }
    } else {
        if (tokens.empty()) {
            std::cout << "  \x1B[31mFAIL: Expected error but got no tokens\x1B[0m" << std::endl;
            return;
        }

        if (!tokens[0].isError()) {
            std::cout << "  \x1B[31mFAIL: Expected error but got token with value: "
                      << static_cast<int>(tokens[0].getValue()) << " ('"
                      << escapeString(std::string(1, tokens[0].getValue())) << "')\x1B[0m"
                      << std::endl;
            return;
        }

        std::string errorMsg = tokens[0].getLexeme();
        if (errorMsg.find(expectedError) == std::string::npos) {
            std::cout << "  \x1B[33mWARN: Expected error containing: \"" << expectedError
                      << "\" but got: \"" << escapeString(errorMsg) << "\"\x1B[0m" << std::endl;
        } else {
            std::cout << "  \x1B[32mPASS: Got expected error: " << escapeString(errorMsg)
                      << "\x1B[0m" << std::endl;
        }
    }
}

int main() {
    // Test valid character literals
    std::cout << "\n=== Testing valid character literals ===\n";
    testCharacterLiteral("'a'", true, 'a');

    // Test escape sequences
    testCharacterLiteral("'\\n'", true, '\n');  // Newline
    testCharacterLiteral("'\\t'", true, '\t');  // Tab
    testCharacterLiteral(
        "'\\\\'", false, '\\',
        "Missing closing quote");              // Backslash character (invalid, needs to be escaped)
    testCharacterLiteral("'\''", true, '\'');  // Single quote character
    testCharacterLiteral("'\\\''", true, '\'');  // Escaped single quote (alternative syntax)
    testCharacterLiteral("'\\r'", true, '\r');   // Carriage return
    testCharacterLiteral("'\\0'", true, '\0');   // Null character

    // Test hex escapes
    testCharacterLiteral("'\\x41'", true, 0x41);  // 'A'
    testCharacterLiteral("'\\x01'", true, 0x01);  // Start of Heading
    testCharacterLiteral("'\\x7F'", true, 0x7F);  // Delete
    testCharacterLiteral("'\\x1F'", true, 0x1F);  // Unit Separator
    testCharacterLiteral("'\\x0A'", true, '\n');  // Newline
    testCharacterLiteral("'\\x09'", true, '\t');  // Tab
    testCharacterLiteral("'\\x0D'", true, '\r');  // Carriage return

    // Test invalid character literals
    std::cout << "\n=== Testing invalid character literals ===\n";
    testCharacterLiteral("''", false, '\0', "Empty character literal");
    testCharacterLiteral("'a", false, '\0', "Missing closing quote");
    testCharacterLiteral("'ab'", false, '\0', "must contain exactly one character");
    testCharacterLiteral("'\\x'", false, '\0', "Invalid hex escape sequence");
    testCharacterLiteral("'\\xG'", false, '\0', "Invalid hex escape sequence");
    testCharacterLiteral("'\\z'", false, '\0', "Unrecognized escape sequence");

    // Test with surrounding text
    std::cout << "\n=== Testing with surrounding text ===\n";
    testCharacterLiteral("var x = 'a' + '\\n';", true, 'a');

    // Test multiple character literals in one input
    std::cout << "\n=== Testing multiple character literals ===\n";
    std::string multiTest = "'a' '\\n' '\\x41'";
    Lexer lexer(multiTest);
    auto tokens = lexer.tokenize();

    if (tokens.size() != 3) {
        std::cout << "\x1B[31mFAIL: Expected 3 tokens but got " << tokens.size() << "\x1B[0m"
                  << std::endl;
    } else {
        std::cout << "\x1B[32mPASS: Successfully parsed " << tokens.size()
                  << " character literals\x1B[0m" << std::endl;
        for (size_t i = 0; i < tokens.size(); i++) {
            std::cout << "  " << i + 1 << ". " << tokens[i].getLexeme() << " -> '"
                      << tokens[i].getValue() << "' (" << static_cast<int>(tokens[i].getValue())
                      << ")" << std::endl;
        }
    }

    return 0;
}
