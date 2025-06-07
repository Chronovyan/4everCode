#include <iostream>
#include <string>
#include <vector>

// Simple character literal parser for testing
class CharacterLiteralParser {
    std::string source;
    size_t position = 0;

    char getChar() {
        if (position >= source.length())
            return '\0';
        return source[position++];
    }

    char peek() {
        if (position >= source.length())
            return '\0';
        return source[position];
    }

    bool isHexDigit(char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    int hexValue(char c) {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'a' && c <= 'f')
            return 10 + (c - 'a');
        if (c >= 'A' && c <= 'F')
            return 10 + (c - 'A');
        return 0;
    }

public:
    CharacterLiteralParser(const std::string& src) : source(src) {}

    // Parse a character literal, returns true if successful
    bool parseChar(char& result) {
        // Skip whitespace
        while (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n') {
            position++;
        }

        // Expect opening quote
        if (getChar() != '\'') {
            std::cout << "  Error: Expected opening quote" << std::endl;
            return false;
        }

        // Handle empty character literal
        if (peek() == '\'') {
            std::cout << "  Error: Empty character literal" << std::endl;
            position++;  // Skip closing quote
            return false;
        }

        // Handle escape sequences
        if (peek() == '\\') {
            position++;  // Skip backslash
            char escape = getChar();
            switch (escape) {
                case 'n':
                    result = '\n';
                    break;
                case 't':
                    result = '\t';
                    break;
                case '\\':
                    result = '\\';
                    break;
                case '\'':
                    result = '\'';
                    break;
                case 'x': {
                    // Simple hex escape (\xHH)
                    if (!isHexDigit(peek())) {
                        std::cout << "  Error: Invalid hex escape sequence" << std::endl;
                        return false;
                    }

                    // Parse first hex digit
                    char c1 = getChar();
                    int value = hexValue(c1);

                    // Parse second hex digit if available
                    if (isHexDigit(peek())) {
                        char c2 = getChar();
                        value = (value << 4) | hexValue(c2);
                    }

                    result = static_cast<char>(value);
                    break;
                }
                default:
                    std::cout << "  Error: Unrecognized escape sequence: \\" << escape << std::endl;
                    return false;
            }
        } else {
            // Regular character
            result = getChar();
        }

        // Expect closing quote
        if (peek() != '\'') {
            std::cout << "  Error: Missing closing quote" << std::endl;
            // Skip to next quote or end of input
            while (peek() != '\'' && peek() != '\0')
                position++;
            if (peek() == '\'')
                position++;  // Skip closing quote if found
            return false;
        }

        // Skip closing quote
        position++;

        // Check for extra characters
        while (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n') {
            position++;
        }

        if (peek() != '\0') {
            std::cout << "  Warning: Extra characters after character literal" << std::endl;
        }

        return true;
    }
};

void testCharacterLiteral(const std::string& input) {
    std::cout << "Testing: " << input << std::endl;

    CharacterLiteralParser parser(input);
    char result;

    if (parser.parseChar(result)) {
        std::cout << "  Success: '" << result << "' (" << static_cast<int>(result) << ")"
                  << std::endl;
    } else {
        std::cout << "  Failed to parse character literal" << std::endl;
    }

    std::cout << std::endl;
}

int main() {
    std::cout << "Testing Character Literal Parser\n";
    std::cout << "================================\n\n";

    // Test cases
    testCharacterLiteral("'a'");      // Simple character
    testCharacterLiteral("'\\n'");    // Newline
    testCharacterLiteral("'\\t'");    // Tab
    testCharacterLiteral("'\\\\'");   // Backslash
    testCharacterLiteral("'\\''");    // Single quote
    testCharacterLiteral("'\\x41'");  // Hex value 'A'
    testCharacterLiteral("'\\x1F'");  // Hex value 0x1F
    testCharacterLiteral("''");       // Empty (error)
    testCharacterLiteral("'ab'");     // Multi-character (error)
    testCharacterLiteral("'a'b");     // Extra characters after

    std::cout << "================================\n";
    std::cout << "Testing Complete\n";

    return 0;
}
