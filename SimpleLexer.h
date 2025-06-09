#ifndef SIMPLE_LEXER_H
#define SIMPLE_LEXER_H

#include <string>
#include <unordered_map>
#include <vector>

// Simple token types for our minimal lexer
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

  // Keywords
  FN,
  LET,
  IF,
  ELSE,
  FOR,
  IN,
  PRINTLN,

  // End of file
  END_OF_FILE,

  // Error
  ERROR
};

// Simple token structure
struct Token {
  TokenType type;
  std::string lexeme;
  int line;

  Token(TokenType type, std::string lexeme, int line)
      : type(type), lexeme(std::move(lexeme)), line(line) {}

  std::string toString() const;
};

class SimpleLexer {
public:
  explicit SimpleLexer(std::string source);

  // Get all tokens from the source
  std::vector<Token> tokenize();

  // Check if there were any errors during lexing
  bool hadError() const { return hadErrorFlag; }

private:
  const std::string source;
  std::vector<Token> tokens;
  int start = 0;
  int current = 0;
  int line = 1;
  bool hadErrorFlag = false;

  // Keywords map
  static const std::unordered_map<std::string, TokenType> keywords;

  // Helper methods
  bool isAtEnd() const;
  char advance();
  char peek() const;
  char peekNext() const;
  bool match(char expected);
  void addToken(TokenType type);
  void scanToken();
  void string();
  void number();
  void identifier();
  void skipWhitespace();
  void error(const std::string &message);
};

#endif // SIMPLE_LEXER_H
