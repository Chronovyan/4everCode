#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Simple Token class for testing
class Token {
public:
    enum class Type {
        IDENTIFIER,
        NUMBER,
        STRING,
        OPERATOR,
        KEYWORD,
        PUNCTUATION,
        END_OF_FILE,
        ERROR
    };

    Token(Type type, const std::string& lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}

    std::string toString() const {
        return "[" + std::to_string(line) + ":" + std::to_string(column) + "] " +
               typeToString(type) + " '" + lexeme + "'";
    }

    Type getType() const { return type; }
    const std::string& getLexeme() const { return lexeme; }
    int getLine() const { return line; }
    int getColumn() const { return column; }

private:
    static std::string typeToString(Type type) {
        switch (type) {
            case Type::IDENTIFIER:
                return "IDENTIFIER";
            case Type::NUMBER:
                return "NUMBER";
            case Type::STRING:
                return "STRING";
            case Type::OPERATOR:
                return "OPERATOR";
            case Type::KEYWORD:
                return "KEYWORD";
            case Type::PUNCTUATION:
                return "PUNCTUATION";
            case Type::END_OF_FILE:
                return "END_OF_FILE";
            case Type::ERROR:
                return "ERROR";
            default:
                return "UNKNOWN";
        }
    }

    Type type;
    std::string lexeme;
    int line;
    int column;
};

// Simple Lexer class for testing
class Lexer {
public:
    Lexer(const std::string& source) : source(source), position(0), line(1), column(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (!isAtEnd()) {
            // Skip whitespace
            if (isspace(peek())) {
                advance();
                continue;
            }

            // Skip comments
            if (peek() == '/' && peekNext() == '/') {
                while (!isAtEnd() && peek() != '\n') {
                    advance();
                }
                continue;
            }

            // Handle numbers
            if (isdigit(peek())) {
                tokens.push_back(scanNumber());
                continue;
            }

            // Handle strings
            if (peek() == '"') {
                tokens.push_back(scanString());
                continue;
            }

            // Handle identifiers and keywords
            if (isalpha(peek()) || peek() == '_') {
                tokens.push_back(scanIdentifierOrKeyword());
                continue;
            }

            // Handle operators and punctuation
            tokens.push_back(scanOperatorOrPunctuation());
        }

        // Add EOF token
        tokens.emplace_back(Token::Type::END_OF_FILE, "", line, column);
        return tokens;
    }

private:
    const std::string source;
    size_t position;
    int line;
    int column;

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

    char peek() const {
        if (isAtEnd())
            return '\0';
        return source[position];
    }

    char peekNext() const {
        if (position + 1 >= source.length())
            return '\0';
        return source[position + 1];
    }

    bool isAtEnd() const { return position >= source.length(); }

    Token scanNumber() {
        int start = position;
        int startLine = line;
        int startCol = column;

        while (!isAtEnd() && isdigit(peek())) {
            advance();
        }

        // Handle decimal point
        if (peek() == '.' && isdigit(peekNext())) {
            advance();  // Consume the '.'
            while (!isAtEnd() && isdigit(peek())) {
                advance();
            }
        }

        std::string lexeme = source.substr(start, position - start);
        return Token(Token::Type::NUMBER, lexeme, startLine, startCol);
    }

    Token scanString() {
        int startLine = line;
        int startCol = column;
        advance();  // Skip the opening "

        std::string value;
        while (!isAtEnd() && peek() != '"') {
            if (peek() == '\\') {
                advance();  // Skip the \
                // Handle escape sequences here if needed
            }
            value += advance();
        }

        if (isAtEnd()) {
            return Token(Token::Type::ERROR, "Unterminated string", startLine, startCol);
        }

        advance();  // Skip the closing "
        return Token(Token::Type::STRING, value, startLine, startCol);
    }

    Token scanIdentifierOrKeyword() {
        int start = position;
        int startLine = line;
        int startCol = column;

        while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
            advance();
        }

        std::string lexeme = source.substr(start, position - start);

        // Check if it's a keyword
        if (isKeyword(lexeme)) {
            return Token(Token::Type::KEYWORD, lexeme, startLine, startCol);
        }

        return Token(Token::Type::IDENTIFIER, lexeme, startLine, startCol);
    }

    Token scanOperatorOrPunctuation() {
        int startLine = line;
        int startCol = column;

        char c = advance();
        std::string op(1, c);

        // Handle multi-character operators (e.g., ==, !=, <=, >=)
        if (!isAtEnd()) {
            char next = peek();
            std::string twoCharOp = op + next;
            if (twoCharOp == "==" || twoCharOp == "!=" || twoCharOp == "<=" || twoCharOp == ">=" ||
                twoCharOp == "&&" || twoCharOp == "||") {
                advance();
                op = twoCharOp;
            }
        }

        return Token(Token::Type::OPERATOR, op, startLine, startCol);
    }

    bool isKeyword(const std::string& word) {
        static const std::vector<std::string> keywords = {
            "let",    "const", "if",    "else", "while", "for", "function",
            "return", "true",  "false", "null", "and",   "or",  "not"};
        return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
    }
};

int main() {
    // Simple test program
    std::string source = R"(
        // A simple test program
        let x = 42;
        let name = "Chronovyan";
        
        function greet(name) {
            print("Hello, " + name);
        }
        
        if (x > 10) {
            greet(name);
        } else {
            print("x is too small");
        }
    )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // Print all tokens
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }

    return 0;
}
