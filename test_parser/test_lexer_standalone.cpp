#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Simple diagnostic reporter for testing
class DiagnosticReporter {
public:
    virtual ~DiagnosticReporter() = default;

    virtual void addError(const std::string& message, int line, int col) {
        std::cerr << "Error at " << line << ":" << col << ": " << message << std::endl;
    }

    virtual void addWarning(const std::string& message, int line, int col) {
        std::cerr << "Warning at " << line << ":" << col << ": " << message << std::endl;
    }
};

// Simple token class for testing
enum class TokenType {
    // Single-character tokens
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,
    CHARACTER,

    // Keywords
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    // End of file
    END_OF_FILE,

    // Error
    ERROR
};

class Token {
    TokenType type;
    std::string lexeme;
    std::string value;
    int line;
    int column;

public:
    Token(TokenType type, const std::string& lexeme, const std::string& value, int line, int column)
        : type(type), lexeme(lexeme), value(value), line(line), column(column) {}

    TokenType getType() const { return type; }
    const std::string& getLexeme() const { return lexeme; }
    const std::string& getValue() const { return value; }
    int getLine() const { return line; }
    int getColumn() const { return column; }

    std::string toString() const {
        return "Token(" + std::to_string(static_cast<int>(type)) + ", " + lexeme + ", " + value +
               ", " + std::to_string(line) + ", " + std::to_string(column) + ")";
    }
};

// Simple lexer for testing
class Lexer {
    std::string source;
    std::string filename;
    DiagnosticReporter& reporter;

    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    int column = 1;

    bool isAtEnd() const { return current >= source.length(); }

    char advance() {
        if (isAtEnd())
            return '\0';
        char c = source[current++];
        column++;
        if (c == '\n') {
            line++;
            column = 1;
        }
        return c;
    }

    char peek() const {
        if (isAtEnd())
            return '\0';
        return source[current];
    }

    char peekNext() const {
        if (current + 1 >= source.length())
            return '\0';
        return source[current + 1];
    }

    bool match(char expected) {
        if (isAtEnd())
            return false;
        if (source[current] != expected)
            return false;

        current++;
        column++;
        if (expected == '\n') {
            line++;
            column = 1;
        }
        return true;
    }

    void addToken(TokenType type, const std::string& value = "") {
        std::string text = source.substr(start, current - start);
        tokens.emplace_back(type, text, value, line, column - text.length());
    }

    void scanToken() {
        char c = advance();
        switch (c) {
            case '(':
                addToken(TokenType::LEFT_PAREN);
                break;
            case ')':
                addToken(TokenType::RIGHT_PAREN);
                break;
            case '{':
                addToken(TokenType::LEFT_BRACE);
                break;
            case '}':
                addToken(TokenType::RIGHT_BRACE);
                break;
            case ',':
                addToken(TokenType::COMMA);
                break;
            case '.':
                addToken(TokenType::DOT);
                break;
            case '-':
                addToken(TokenType::MINUS);
                break;
            case '+':
                addToken(TokenType::PLUS);
                break;
            case ';':
                addToken(TokenType::SEMICOLON);
                break;
            case '*':
                addToken(TokenType::STAR);
                break;

            case '!':
                addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
                break;
            case '=':
                addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
                break;
            case '<':
                addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
                break;
            case '>':
                addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
                break;

            case '/':
                if (match('/')) {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd())
                        advance();
                } else {
                    addToken(TokenType::SLASH);
                }
                break;

            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;

            case '\n':
                line++;
                column = 1;
                break;

            case '"':
                handleString();
                break;
            case '\'':
                handleCharacter();
                break;

            default:
                if (isdigit(c)) {
                    number();
                } else if (isalpha(c) || c == '_') {
                    identifier();
                } else {
                    reporter.addError("Unexpected character: " + std::string(1, c), line,
                                      column - 1);
                }
                break;
        }
    }

    void handleString() {
        std::string value;
        std::string lexeme = "\"";
        int startLine = line;
        int startCol = column - 1;
        bool inEscape = false;
        bool unterminated = true;

        while (!isAtEnd()) {
            char c = advance();

            if (c == '\n' && !inEscape) {
                break;  // Unterminated string
            }

            if (inEscape) {
                lexeme += '\\';
                lexeme += c;

                switch (c) {
                    case 'n':
                        value += '\n';
                        break;
                    case 't':
                        value += '\t';
                        break;
                    case 'r':
                        value += '\r';
                        break;
                    case '\\':
                        value += '\\';
                        break;
                    case '"':
                        value += '"';
                        break;
                    case '0':
                        value += '\0';
                        break;
                    case 'x': {
                        // Handle hex escape
                        std::string hexStr;
                        for (int i = 0; i < 2 && !isAtEnd() && isxdigit(peek()); i++) {
                            hexStr += advance();
                            lexeme += hexStr.back();
                        }

                        if (hexStr.empty()) {
                            reporter.addError("Invalid hex escape sequence", line, column - 1);
                            continue;
                        }

                        try {
                            int hexValue = std::stoi(hexStr, nullptr, 16);
                            if (hexValue > 255) {
                                reporter.addWarning("Hex escape sequence out of range", line,
                                                    column - hexStr.length());
                                hexValue = hexValue & 0xFF;
                            }
                            value += static_cast<char>(hexValue);
                        } catch (...) {
                            reporter.addError("Invalid hex escape sequence", line,
                                              column - hexStr.length());
                        }
                        break;
                    }
                    default:
                        reporter.addWarning("Unknown escape sequence: \\" + std::string(1, c), line,
                                            column - 2);
                        value += c;
                        break;
                }
                inEscape = false;
            } else if (c == '\\') {
                inEscape = true;
            } else if (c == '"') {
                lexeme += c;
                unterminated = false;
                break;
            } else {
                value += c;
                lexeme += c;
            }
        }

        if (unterminated) {
            reporter.addError("Unterminated string", startLine, startCol);
            tokens.emplace_back(TokenType::ERROR, lexeme, "", startLine, startCol);
        } else {
            tokens.emplace_back(TokenType::STRING, lexeme, value, startLine, startCol);
        }
    }

    void handleCharacter() {
        char value = '\0';
        std::string lexeme = "'";
        int startLine = line;
        int startCol = column - 1;
        bool valid = false;

        if (isAtEnd() || peek() == '\'') {
            reporter.addError("Empty character literal", line, column);
            tokens.emplace_back(TokenType::ERROR, "''", "", startLine, startCol);
            if (!isAtEnd() && peek() == '\'') {
                advance();  // Consume the closing quote
            }
            return;
        }

        char c = advance();
        lexeme += c;

        if (c == '\\') {
            // Handle escape sequences
            if (isAtEnd()) {
                reporter.addError("Unterminated character literal", startLine, startCol);
                tokens.emplace_back(TokenType::ERROR, lexeme, "", startLine, startCol);
                return;
            }

            c = advance();
            lexeme += c;

            switch (c) {
                case 'n':
                    value = '\n';
                    break;
                case 't':
                    value = '\t';
                    break;
                case 'r':
                    value = '\r';
                    break;
                case '\\':
                    value = '\\';
                    break;
                case '\'':
                    value = '\'';
                    break;
                case '0':
                    value = '\0';
                    break;
                case 'x': {
                    // Handle hex escape
                    std::string hexStr;
                    for (int i = 0; i < 2 && !isAtEnd() && isxdigit(peek()); i++) {
                        hexStr += advance();
                        lexeme += hexStr.back();
                    }

                    if (hexStr.empty()) {
                        reporter.addError("Invalid hex escape sequence", line, column - 1);
                        break;
                    }

                    try {
                        int hexValue = std::stoi(hexStr, nullptr, 16);
                        if (hexValue > 255) {
                            reporter.addWarning("Hex escape sequence out of range", line,
                                                column - hexStr.length());
                            hexValue = hexValue & 0xFF;
                        }
                        value = static_cast<char>(hexValue);
                    } catch (...) {
                        reporter.addError("Invalid hex escape sequence", line,
                                          column - hexStr.length());
                    }
                    break;
                }
                default:
                    reporter.addWarning("Unknown escape sequence: \\" + std::string(1, c), line,
                                        column - 2);
                    value = c;
                    break;
            }
        } else {
            value = c;
        }

        if (isAtEnd() || !match('\'')) {
            reporter.addError("Unterminated character literal", startLine, startCol);
            tokens.emplace_back(TokenType::ERROR, lexeme, "", startLine, startCol);
        } else {
            lexeme += "'";
            tokens.emplace_back(TokenType::CHARACTER, lexeme, std::string(1, value), startLine,
                                startCol);
        }
    }

    void number() {
        while (isdigit(peek()))
            advance();

        // Look for a fractional part.
        if (peek() == '.' && isdigit(peekNext())) {
            // Consume the "."
            advance();

            while (isdigit(peek()))
                advance();
        }

        std::string text = source.substr(start, current - start);
        addToken(TokenType::NUMBER, text);
    }

    void identifier() {
        while (isalnum(peek()) || peek() == '_')
            advance();

        std::string text = source.substr(start, current - start);
        // In a real lexer, we would check for keywords here
        addToken(TokenType::IDENTIFIER, text);
    }

public:
    Lexer(const std::string& source, const std::string& filename, DiagnosticReporter& reporter)
        : source(source), filename(filename), reporter(reporter) {}

    std::vector<Token> scanTokens() {
        while (!isAtEnd()) {
            // We are at the beginning of the next lexeme.
            start = current;
            scanToken();
        }

        tokens.emplace_back(TokenType::END_OF_FILE, "", "", line, column);
        return tokens;
    }
};

// Helper function to read a file into a string
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string source = readFile(filename);
    if (source.empty()) {
        return 1;
    }

    DiagnosticReporter reporter;
    Lexer lexer(source, filename, reporter);
    std::vector<Token> tokens = lexer.scanTokens();

    // Print all tokens
    for (const auto& token : tokens) {
        std::cout << "Token: " << token.toString() << std::endl;
        if (token.getType() == TokenType::STRING) {
            std::cout << "  String value: \"" << token.getValue() << "\"" << std::endl;
        } else if (token.getType() == TokenType::CHARACTER) {
            std::cout << "  Character value: '" << token.getValue() << "' ("
                      << static_cast<int>(token.getValue()[0]) << ")" << std::endl;
        } else if (token.getType() == TokenType::NUMBER) {
            std::cout << "  Number value: " << token.getValue() << std::endl;
        } else if (token.getType() == TokenType::IDENTIFIER) {
            std::cout << "  Identifier: " << token.getValue() << std::endl;
        }
    }

    return 0;
}
