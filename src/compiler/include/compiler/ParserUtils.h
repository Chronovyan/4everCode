#ifndef CHRONOVYAN_COMPILER_PARSER_UTILS_H
#define CHRONOVYAN_COMPILER_PARSER_UTILS_H

#include <memory>

#include "compiler/DiagnosticReporter.h"
#include "compiler/Token.h"

namespace chronovyan {
namespace compiler {

// Common parser constants
constexpr size_t MAX_PARAMETERS = 255;
constexpr size_t MAX_ARGUMENTS = 255;

// Common error messages
namespace ErrorMessages {
constexpr const char *EXPECTED_IDENTIFIER = "Expected identifier";
constexpr const char *EXPECTED_SEMICOLON = "Expected ';' after statement";
constexpr const char *EXPECTED_LEFT_BRACE = "Expected '{' before block";
constexpr const char *EXPECTED_RIGHT_BRACE = "Expected '}' after block";
constexpr const char *EXPECTED_LEFT_PAREN = "Expected '(' after condition";
constexpr const char *EXPECTED_RIGHT_PAREN = "Expected ')' after condition";
constexpr const char *INVALID_ASSIGNMENT_TARGET = "Invalid assignment target";
constexpr const char *TOO_MANY_ARGUMENTS =
    "Cannot have more than 255 arguments";
constexpr const char *TOO_MANY_PARAMETERS =
    "Cannot have more than 255 parameters";
} // namespace ErrorMessages

// Helper functions for common parsing patterns
class ParserUtils {
public:
  // Check if the current token is a valid start of a statement
  static bool isStartOfStatement(TokenType type) {
    switch (type) {
    case TokenType::LEFT_BRACE:
    case TokenType::IF:
    case TokenType::WHILE:
    case TokenType::FOR:
    case TokenType::RETURN:
    case TokenType::BREAK:
    case TokenType::CONTINUE:
    case TokenType::ALLOCATE_RESOURCES:
    case TokenType::EXPEND_RESOURCES:
    case TokenType::BORROW_RESOURCES:
    case TokenType::REPAY_DEBT:
    case TokenType::BRANCH_TIMELINE:
    case TokenType::MERGE_TIMELINES:
    case TokenType::REWIND_FLOW:
    case TokenType::REVERSE_FLOW:
      return true;
    default:
      return false;
    }
  }

  // Report an error and synchronize to the next statement
  template <typename T>
  static std::unique_ptr<T>
  reportErrorAndSynchronize(DiagnosticReporter &reporter, const Token &token,
                            const std::string &message) {
    reporter.addError(token, message);
    // Skip synchronization here - let the caller handle it
    return nullptr;
  }
};

} // namespace compiler
} // namespace chronovyan

#endif // CHRONOVYAN_COMPILER_PARSER_UTILS_H
