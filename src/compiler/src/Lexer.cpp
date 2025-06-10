#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iomanip> // For std::hex, std::setw, std::setfill
#include <sstream>
#include <string>
#include <vector>

#include "compiler/Diagnostic.h"
#include "compiler/Lexer.h"

namespace chronovyan {
namespace compiler {

// ... (rest of the code remains the same)

void Lexer::debugPrint(const std::string &message) {
  if (diagnosticReporter && diagnosticReporter->getDebugOutput()) {
    diagnosticReporter->addInfo(SourceLocation(filename, line, column),
                                message);
  }
}

Token Lexer::handleCharacter() {
  debugPrint("=== Starting character literal processing ===");
  tokenStart = position - 1; // Position of the opening single quote
  tokenStartLine = line;
  tokenStartColumn = column - 1;

  debugPrint("Found opening quote at position " + std::to_string(tokenStart) +
             ", line " + std::to_string(tokenStartLine) + ", column " +
             std::to_string(tokenStartColumn));

  // Debug log
  if (diagnosticReporter && diagnosticReporter->getDebugOutput()) {
    std::string debugMsg = "handleCharacter: Starting at position " +
                           std::to_string(position) + ", char='" +
                           (isAtEnd(0) ? "<EOF>" : escapeChar(peek(0))) + "'";
    debugMsg += ", source length: " + std::to_string(source.length());
    if (static_cast<size_t>(position) + 2 < source.length()) {
      debugMsg += ", next 3 chars: ";
      for (int i = 0; i < 3 && (position + i) < source.length(); i++) {
        debugMsg += escapeChar(peek(i));
      }
    }
    diagnosticReporter->addInfo(
        SourceLocation(filename, tokenStartLine, tokenStartColumn), debugMsg);
  }

  // Skip the opening quote
  advance();

  if (isAtEnd()) {
    debugPrint("Error: Reached end of input after opening quote");
    return errorToken("Unterminated character literal: missing closing quote");
  }

  debugPrint(
      "After skipping opening quote, position: " + std::to_string(position) +
      ", current char: " + (isAtEnd() ? "<EOF>" : escapeChar(peek())));

  // Check for empty character literal (two quotes in a row)
  if (peek() == '\'') {
    debugPrint("Found empty character literal ('')");
    advance(); // Skip the closing quote
    return errorToken("Empty character literal");
  }

  // Get the character (may be start of an escape sequence)
  char c = '\0';
  bool isValid = true;
  std::string errorMsg;

  // Handle escape sequences
  if (peek() == '\\') {
    // Skip the backslash
    debugPrint("Found escape sequence");
    advance();

    if (isAtEnd()) {
      debugPrint("Error: Incomplete escape sequence at end of input");
      return errorToken("Incomplete escape sequence");
    }

    // Handle the escape sequence
    char escapeChar = peek();
    switch (escapeChar) {
    case 'n':
      c = '\n';
      debugPrint("Handling newline escape sequence");
      break;
    case 't':
      c = '\t';
      debugPrint("Handling tab escape sequence");
      break;
    case '\\\\':
      c = '\\';
      debugPrint("Handling backslash escape sequence");
      break;
    case '\'':
      c = '\'';
      debugPrint("Handling single quote escape sequence");
      break;
    case 'x': {
      // Hex escape (\xHH)
      debugPrint("Handling hex escape sequence");
      advance(); // Skip 'x'

      if (isAtEnd() || !isHexDigit(peek())) {
        debugPrint("Error: Invalid hex escape sequence - missing digit");
        return errorToken("Invalid hex escape sequence: missing digit");
      }

      // Parse first hex digit
      int value = hexDigitToInt(peek());
      debugPrint("First hex digit: " + std::string(1, peek()) + " (" +
                 std::to_string(value) + ")");
      advance();

      // Parse second hex digit if available
      if (!isAtEnd() && isHexDigit(peek())) {
        debugPrint("Second hex digit: " + std::string(1, peek()) + " (" +
                   std::to_string(hexDigitToInt(peek())) + ")");
        value = (value << 4) | hexDigitToInt(peek());
        advance();
      }

      c = static_cast<char>(value);
      debugPrint("Hex escape sequence value: " +
                 std::to_string(static_cast<int>(c)));
      break;
    }
    default:
      debugPrint("Error: Unrecognized escape sequence: \\" +
                 std::string(1, escapeChar));
      return errorToken("Unrecognized escape sequence: \\" +
                        std::string(1, escapeChar));
    }

    // Consume the escape character
    advance();
  } else {
    // Regular character
    c = peek();
    debugPrint("Regular character: " + escapeChar(c) + " (" +
               std::to_string(static_cast<int>(c)) + ")");
    advance();
  }

  // Check for multi-character literal (but allow closing quote)
  if (!isAtEnd() && peek() != '\'') {
    debugPrint("Error: Multiple characters in character literal");
    // Skip to the next quote or end of input
    while (!isAtEnd() && peek() != '\'') {
      debugPrint("Skipping character: " + escapeChar(peek()));
      advance();
    }
    if (!isAtEnd()) {
      debugPrint("Found closing quote at position: " +
                 std::to_string(position));
      advance(); // Skip the closing quote
    }
    return errorToken("Character literal must contain exactly one character");
  }

  // Expect closing quote
  if (isAtEnd()) {
    debugPrint("Error: Reached end of input before closing quote");
    return errorToken("Missing closing quote in character literal");
  }

  // Skip the closing quote
  debugPrint("Found closing quote at position: " + std::to_string(position));
  advance();

  // Create the token
  std::string lexeme =
      source.substr(static_cast<size_t>(tokenStart), position - tokenStart);

  debugPrint("Successfully parsed character literal: " + lexeme + ", value: '" +
             escapeChar(c) + "' (" + std::to_string(static_cast<int>(c)) + ")");
  debugPrint("=== End of character literal processing ===\n");

  // Store the character as an integer in the token
  return makeToken(TokenType::CHARACTER, static_cast<int>(c), lexeme);
}

// Character classification helpers
bool Lexer::isDigit(char c) { return c >= '0' && c <= '9'; }

bool Lexer::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

bool Lexer::isHexDigit(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

bool Lexer::isOctDigit(char c) { return c >= '0' && c <= '7'; }

bool Lexer::isDigitSeparator(char c) { return c == '_'; }

// Helper function to convert a hex digit to its integer value
int Lexer::hexDigitToInt(char c) {
  if (c >= '0' && c <= '9')
    return static_cast<int>(c - '0');
  if (c >= 'a' && c <= 'f')
    return 10 + static_cast<int>(c - 'a');
  if (c >= 'A' && c <= 'F')
    return 10 + static_cast<int>(c - 'A');
  return 0; // Shouldn't happen as we check isHexDigit first
}

char Lexer::peek(int offset) {
  int targetPos = position + offset;
  if (targetPos < 0 || targetPos >= static_cast<int>(source.length()))
    return '\0';
  return source[targetPos];
}

char Lexer::peekNext() { return peek(1); }

bool Lexer::match(char expected) {
  if (isAtEnd())
    return false;
  if (source[position] != expected)
    return false;

  position++;
  return true;
}

char Lexer::advance() {
  char c = source[position++];
  column++;
  return c;
}

void Lexer::advanceLine() {
  line++;
  column = 1;
}

bool Lexer::isAtEnd(int offset) const {
  // Cast to size_t to avoid sign comparison warning
  size_t newPosition =
      static_cast<size_t>(position) + static_cast<size_t>(offset);
  return newPosition >= source.length();
}

void Lexer::skipWhitespace() {
  while (true) {
    char c = peek();
    switch (c) {
    case ' ':
    case '\t':
    case '\r':
      advance();
      break;
    case '\n':
      advanceLine();
      advance();
      break;
    default:
      return;
    }
  }
}

void Lexer::skipLineComment() {
  while (peek() != '\n' && !isAtEnd()) {
    advance();
  }
}

void Lexer::skipBlockComment() {
  int nestingLevel = 1;

  while (nestingLevel > 0 && !isAtEnd()) {
    if (peek() == '/' && peekNext() == '*') {
      advance(); // /
      advance(); // *
      nestingLevel++;
    } else if (peek() == '*' && peekNext() == '/') {
      advance(); // *
      advance(); // /
      nestingLevel--;
    } else if (peek() == '\n') {
      advanceLine();
      advance();
    } else {
      advance();
    }
  }

  if (nestingLevel > 0 && diagnosticReporter) {
    SourceLocation location(filename, line, column);
    (*diagnosticReporter).addError(location, "Unterminated block comment.");
  }
}

Token Lexer::makeToken(TokenType type) {
  size_t length = position - tokenStart;
  std::string lexeme = source.substr(tokenStart, length);
  SourceLocation location(filename, tokenStartLine, tokenStartColumn);
  return Token(type, lexeme, location);
}

Token Lexer::makeToken(TokenType type, Token::ValueType value) {
  size_t length = position - tokenStart;
  std::string lexeme = source.substr(tokenStart, length);
  SourceLocation location(filename, tokenStartLine, tokenStartColumn);
  return Token(type, lexeme, location, value);
}

Token Lexer::makeToken(TokenType type, Token::ValueType value,
                       const std::string &lexeme) {
  SourceLocation location(filename, tokenStartLine, tokenStartColumn);
  return Token(type, lexeme, location, value);
}

Token Lexer::errorToken(const std::string &message) {
  SourceLocation location(filename, line, column);
  diagnosticReporter->addError(location, message);
  return Token(TokenType::ERROR, message, location);
}

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;

  // Reset the lexer state
  position = 0;
  line = 1;
  column = 1;
  hasCurrentToken = false;

  // Skip the UTF-8 BOM if present
  skipBOM(source, position);

  // Keep getting tokens until we reach the end of the file
  while (true) {
    Token token = nextToken();
    tokens.push_back(token);

    if (token.getType() == TokenType::END_OF_FILE ||
        token.getType() == TokenType::ERROR) {
      break;
    }
  }

  return tokens;
}

// Helper method to escape special characters in debug output
std::string Lexer::escapeChar(char c) {
  switch (c) {
  case '\n':
    return "\\n";
  case '\t':
    return "\\t";
  case '\\':
    return "\\\\";
  case '\'':
    return "\\'";
  case '\r':
    return "\\r";
  case '\0':
    return "\\0";
  default:
    if (std::isprint(static_cast<unsigned char>(c))) {
      return std::string(1, c);
    } else {
      std::ostringstream oss;
      oss << "\\x" << std::hex << std::setw(2) << std::setfill('0')
          << (static_cast<unsigned int>(c) & 0xff);
      return oss.str();
    }
  }
}

} // namespace compiler
} // namespace chronovyan
