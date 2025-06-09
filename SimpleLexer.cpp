#include "SimpleLexer.h"
#include <cctype>
#include <iostream>
#include <sstream>

// Initialize keywords map
const std::unordered_map<std::string, TokenType> SimpleLexer::keywords = {
    {"fn", TokenType::FN},          {"let", TokenType::LET},
    {"if", TokenType::IF},          {"else", TokenType::ELSE},
    {"for", TokenType::FOR},        {"in", TokenType::IN},
    {"println", TokenType::PRINTLN}};

SimpleLexer::SimpleLexer(std::string source) : source(std::move(source)) {}

std::vector<Token> SimpleLexer::tokenize() {
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  // Add EOF token
  tokens.emplace_back(TokenType::END_OF_FILE, "", line);
  return tokens;
}

bool SimpleLexer::isAtEnd() const { return current >= source.length(); }

char SimpleLexer::advance() { return source[current++]; }

char SimpleLexer::peek() const {
  if (isAtEnd())
    return '\0';
  return source[current];
}

char SimpleLexer::peekNext() const {
  if (current + 1 >= source.length())
    return '\0';
  return source[current + 1];
}

bool SimpleLexer::match(char expected) {
  if (isAtEnd())
    return false;
  if (source[current] != expected)
    return false;

  current++;
  return true;
}

void SimpleLexer::addToken(TokenType type) {
  std::string text = source.substr(start, current - start);
  tokens.emplace_back(type, text, line);
}

void SimpleLexer::scanToken() {
  char c = advance();

  switch (c) {
  // Single-character tokens
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

  // One or two character tokens
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
      // A comment goes until the end of the line
      while (peek() != '\n' && !isAtEnd())
        advance();
    } else {
      addToken(TokenType::SLASH);
    }
    break;

  // Whitespace
  case ' ':
  case '\r':
  case '\t':
    // Ignore whitespace
    break;

  case '\n':
    line++;
    break;

  // String literals
  case '"':
    string();
    break;

  default:
    if (isdigit(c)) {
      number();
    } else if (isalpha(c) || c == '_') {
      identifier();
    } else {
      error("Unexpected character.");
    }
    break;
  }
}

void SimpleLexer::string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      line++;
    advance();
  }

  if (isAtEnd()) {
    error("Unterminated string.");
    return;
  }

  // The closing "
  advance();

  // Trim the surrounding quotes
  std::string value = source.substr(start + 1, current - start - 2);
  tokens.emplace_back(TokenType::STRING, value, line);
}

void SimpleLexer::number() {
  while (isdigit(peek()))
    advance();

  // Look for a fractional part
  if (peek() == '.' && isdigit(peekNext())) {
    // Consume the "."
    advance();

    while (isdigit(peek()))
      advance();
  }

  addToken(TokenType::NUMBER);
}

void SimpleLexer::identifier() {
  while (isalnum(peek()) || peek() == '_')
    advance();

  // Check if the identifier is a reserved word
  std::string text = source.substr(start, current - start);

  TokenType type;
  auto it = keywords.find(text);
  if (it != keywords.end()) {
    type = it->second;
  } else {
    type = TokenType::IDENTIFIER;
  }

  addToken(type);
}

void SimpleLexer::error(const std::string &message) {
  std::cerr << "[line " << line << "] Error: " << message << std::endl;
  hadErrorFlag = true;
}

std::string Token::toString() const {
  static const char *typeNames[] = {
      "LEFT_PAREN", "RIGHT_PAREN",   "LEFT_BRACE", "RIGHT_BRACE", "COMMA",
      "DOT",        "MINUS",         "PLUS",       "SEMICOLON",   "SLASH",
      "STAR",       "BANG",          "BANG_EQUAL", "EQUAL",       "EQUAL_EQUAL",
      "GREATER",    "GREATER_EQUAL", "LESS",       "LESS_EQUAL",  "IDENTIFIER",
      "STRING",     "NUMBER",        "FN",         "LET",         "IF",
      "ELSE",       "FOR",           "IN",         "PRINTLN",     "END_OF_FILE",
      "ERROR"};

  return std::string("[\033[1;34m") + typeNames[static_cast<int>(type)] +
         "\033[0m] " + (lexeme.empty() ? "" : "'" + lexeme + "' ") +
         "at line " + std::to_string(line);
}
