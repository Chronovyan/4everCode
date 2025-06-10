#ifndef CHRONOVYAN_LEXER_H
#define CHRONOVYAN_LEXER_H

#include <memory>
#include <string>
#include <vector>

#include "compiler/Diagnostic.h"
#include "compiler/Token.h"

namespace chronovyan {
namespace compiler {

/**
 * Lexer is responsible for converting a string of source code into a sequence
 * of tokens.
 */
class Lexer {
public:
  /**
   * Constructs a lexer with the given source code, filename, and diagnostic
   * reporter.
   *
   * @param source The source code to tokenize
   * @param filename The name of the file containing the source code
   * @param reporter The diagnostic reporter to use for error reporting
   */
  Lexer(std::string source, std::string filename,
        std::shared_ptr<DiagnosticReporter> reporter = nullptr);

  /**
   * Returns the next token in the source code.
   * Advances the current position to the next token.
   *
   * @return The next token
   */
  Token nextToken();

  /**
   * Returns the next token in the source code without advancing the current
   * position.
   *
   * @return The next token
   */
  Token peekToken();

  /**
   * Advances the current position to the next token.
   */
  void consumeToken();

  /**
   * Returns a reference to the diagnostic reporter.
   *
   * @return The diagnostic reporter
   */
  DiagnosticReporter &getDiagnosticReporter() { return *diagnosticReporter; }

  /**
   * Returns a const reference to the diagnostic reporter.
   *
   * @return The diagnostic reporter
   */
  const DiagnosticReporter &getDiagnosticReporter() const {
    return *diagnosticReporter;
  }

  /**
   * Returns whether the lexer has encountered any errors.
   *
   * @return True if the lexer has encountered errors
   */
  bool hasErrors() const { return diagnosticReporter->hasErrors(); }

  /**
   * Tokenizes the entire source code and returns a vector of tokens.
   * This is a convenience method that collects all tokens until END_OF_FILE.
   *
   * @return A vector of all tokens in the source code
   */
  std::vector<Token> tokenize();

private:
  std::string source;
  std::string filename;
  int position;
  int line;
  int column;
  Token currentToken;
  bool hasCurrentToken;
  std::shared_ptr<DiagnosticReporter> diagnosticReporter;

  // Token position tracking
  int tokenStart;
  int tokenStartLine;
  int tokenStartColumn;

  // Helper methods for scanning tokens
  Token scanToken();
  Token handleIdentifier();
  Token handleNumber();
  Token handleString();
  Token handleCharacter();

  // Helper methods for handling multi-character tokens
  bool match(char expected);
  // Helper methods for peeking at characters
  char peek(int offset = 0);
  char peekNext();

  // Helper methods for advancing through the source code
  char advance();
  void advanceLine();
  bool isAtEnd(int offset = 0) const;

  // Helper methods for skipping whitespace and comments
  void skipWhitespace();
  void skipLineComment();
  void skipBlockComment();

  // Helper methods for character classification
  static bool isDigit(char c);
  static bool isAlpha(char c);
  static bool isAlphaNumeric(char c);
  static bool isHexDigit(char c);
  static bool isOctDigit(char c);
  static bool isDigitSeparator(char c);
  static int hexDigitToInt(char c);

  // Helper methods for creating tokens
  Token makeToken(TokenType type);
  Token makeToken(TokenType type, Token::ValueType value);
  Token makeToken(TokenType type, Token::ValueType value,
                  const std::string &lexeme);
  Token errorToken(const std::string &message);

  // Helper method to escape special characters in debug output
  std::string escapeChar(char c);
};

} // namespace compiler
} // namespace chronovyan

#endif // CHRONOVYAN_LEXER_H