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

    void skipWhitespace() {
        while (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n') {
            position++;
        }
    }

public:
    CharacterLiteralParser(const std::string& src) : source(src) {}

    // Parse a character literal, returns true if successful
    bool parseChar(char& result) {
        skipWhitespace();

        // Expect opening quote
        if (getChar() != '\'') {
            std::cout << "Error: Expected opening quote" << std::endl;
            return false;
        }

        // Handle empty character literal
        if (peek() == '\'') {
            std::cout << "Error: Empty character literal" << std::endl;
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
                    if (!isxdigit(peek())) {
                        std::cout << "Error: Invalid hex escape sequence" << std::endl;
                        return false;
                    }
                    // Just consume two hex digits (simplified for this example)
                    position++;
                    if (isxdigit(peek()))
                        position++;
                    result = 'x';  // Use 'x' as a placeholder for hex values in this simple example
                    break;
                }
                default:
                    std::cout << "Error: Unrecognized escape sequence: \\" << escape << std::endl;
                    return false;
            }
        } else {
            // Regular character
            result = getChar();
        }

        // Expect closing quote
        if (peek() != '\'') {
            std::cout << "Error: Missing closing quote" << std::endl;
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
        skipWhitespace();
        if (peek() != '\0') {
            std::cout << "Warning: Extra characters after character literal" << std::endl;
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
    testCharacterLiteral("''");       // Empty (error)
    testCharacterLiteral("'ab'");     // Multi-character (error)

    std::cout << "================================\n";
    std::cout << "Testing Complete\n";

    return 0;
}
