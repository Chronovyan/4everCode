#include <algorithm> // for std::remove_copy
#include <sstream>
#include <unordered_map>

#include "compiler/Token.h"

namespace chronovyan {
namespace compiler {

std::string Token::toString() const {
  std::stringstream ss;
  ss << "Token(" << tokenTypeToString(type) << ", '" << lexeme << "', ";

  if (value.has_value()) {
    const auto &val = *value;
    if (std::holds_alternative<int>(val)) {
      ss << std::get<int>(val);
    } else if (std::holds_alternative<double>(val)) {
      ss << std::get<double>(val);
    } else if (std::holds_alternative<std::string>(val)) {
      ss << "\"" << std::get<std::string>(val) << "\"";
    } else if (std::holds_alternative<bool>(val)) {
      ss << (std::get<bool>(val) ? "true" : "false");
    } else {
      ss << "no value";
    }
  } else {
    ss << "no value";
  }

  ss << ", " << location.line << ":" << location.column << ")";
  return ss.str();
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
  return os << token.toString();
}

const char *tokenTypeToString(TokenType type) {
  switch (type) {
  // Single-character tokens
  case TokenType::LEFT_PAREN:
    return "LEFT_PAREN";
  case TokenType::RIGHT_PAREN:
    return "RIGHT_PAREN";
  case TokenType::LEFT_BRACE:
    return "LEFT_BRACE";
  case TokenType::RIGHT_BRACE:
    return "RIGHT_BRACE";
  case TokenType::LEFT_BRACKET:
    return "LEFT_BRACKET";
  case TokenType::RIGHT_BRACKET:
    return "RIGHT_BRACKET";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::DOT:
    return "DOT";
  case TokenType::MINUS:
    return "MINUS";
  case TokenType::PLUS:
    return "PLUS";
  case TokenType::SEMICOLON:
    return "SEMICOLON";
  case TokenType::SLASH:
    return "SLASH";
  case TokenType::STAR:
    return "STAR";
  case TokenType::PERCENT:
    return "PERCENT";
  case TokenType::COLON:
    return "COLON";

  // One or two character tokens
  case TokenType::BANG:
    return "BANG";
  case TokenType::BANG_EQUAL:
    return "BANG_EQUAL";
  case TokenType::EQUAL:
    return "EQUAL";
  case TokenType::EQUAL_EQUAL:
    return "EQUAL_EQUAL";
  case TokenType::GREATER:
    return "GREATER";
  case TokenType::GREATER_EQUAL:
    return "GREATER_EQUAL";
  case TokenType::LESS:
    return "LESS";
  case TokenType::LESS_EQUAL:
    return "LESS_EQUAL";
  case TokenType::PLUS_PLUS:
    return "PLUS_PLUS";
  case TokenType::MINUS_MINUS:
    return "MINUS_MINUS";
  case TokenType::PLUS_EQUAL:
    return "PLUS_EQUAL";
  case TokenType::MINUS_EQUAL:
    return "MINUS_EQUAL";
  case TokenType::STAR_EQUAL:
    return "STAR_EQUAL";
  case TokenType::SLASH_EQUAL:
    return "SLASH_EQUAL";
  case TokenType::PERCENT_EQUAL:
    return "PERCENT_EQUAL";
  case TokenType::COLON_COLON:
    return "COLON_COLON";
  case TokenType::ARROW:
    return "ARROW";

  // Logical operators
  case TokenType::AND:
    return "AND";
  case TokenType::OR:
    return "OR";

  // Literals
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";
  case TokenType::STRING:
    return "STRING";
  case TokenType::INTEGER:
    return "INTEGER";
  case TokenType::FLOAT:
    return "FLOAT";
  case TokenType::CHRONON_LITERAL:
    return "CHRONON_LITERAL";
  case TokenType::AETHEL_LITERAL:
    return "AETHEL_LITERAL";

  // Keywords - Resource Management
  case TokenType::ALLOCATE_RESOURCES:
    return "ALLOCATE_RESOURCES";
  case TokenType::EXPEND_RESOURCES:
    return "EXPEND_RESOURCES";
  case TokenType::BORROW_RESOURCES:
    return "BORROW_RESOURCES";
  case TokenType::REPAY_DEBT:
    return "REPAY_DEBT";
  case TokenType::CHRONON:
    return "CHRONON";
  case TokenType::AETHEL:
    return "AETHEL";

  // Keywords - Variable Declaration
  case TokenType::LET:
    return "LET";
  case TokenType::CONST:
    return "CONST";
  case TokenType::CONF:
    return "CONF";
  case TokenType::REB:
    return "REB";

  // Keywords - Control Flow
  case TokenType::IF:
    return "IF";
  case TokenType::ELSE:
    return "ELSE";
  case TokenType::WHILE:
    return "WHILE";
  case TokenType::FOR:
    return "FOR";
  case TokenType::FUNCTION:
    return "FUNCTION";
  case TokenType::RETURN:
    return "RETURN";

  // Keywords - Module System
  case TokenType::IMPORT:
    return "IMPORT";
  case TokenType::EXPORT:
    return "EXPORT";

  // Keywords - Temporal Control
  case TokenType::BRANCH_TIMELINE:
    return "BRANCH_TIMELINE";
  case TokenType::MERGE_TIMELINES:
    return "MERGE_TIMELINES";
  case TokenType::REWIND_FLOW:
    return "REWIND_FLOW";
  case TokenType::REVERSE_FLOW:
    return "REVERSE_FLOW";
  case TokenType::ANCHOR:
    return "ANCHOR";
  case TokenType::WEAVER:
    return "WEAVER";

  // Keywords - Type System
  case TokenType::STATIC:
    return "STATIC";
  case TokenType::VOLATILE:
    return "VOLATILE";

  // Keywords - Boolean Literals, Null, and Special Values
  case TokenType::TRUE:
    return "TRUE";
  case TokenType::FALSE:
    return "FALSE";
  case TokenType::NULL_VALUE:
    return "NULL";

  // Special tokens
  case TokenType::ERROR:
    return "ERROR";
  case TokenType::END_OF_FILE:
    return "END_OF_FILE";

  // Special literals
  case TokenType::CHARACTER:
    return "CHARACTER";

  // Numeric literals with explicit base
  case TokenType::HEX_LITERAL:
    return "HEX_LITERAL";
  case TokenType::BINARY_LITERAL:
    return "BINARY_LITERAL";
  case TokenType::OCTAL_LITERAL:
    return "OCTAL_LITERAL";

  // Special operators
  case TokenType::NULLISH_COALESCING:
    return "NULLISH_COALESCING";
  case TokenType::NULLISH_COALESCING_EQUAL:
    return "NULLISH_COALESCING_EQUAL";
  case TokenType::OPTIONAL_CHAINING:
    return "OPTIONAL_CHAINING";

  // Type operators
  case TokenType::IN:
    return "IN";

  // Other
  case TokenType::ELLIPSIS:
    return "ELLIPSIS";

  // Chronoscript Block Types
  case TokenType::ANTECEDENCE:
    return "ANTECEDENCE";
  case TokenType::CONCURRENCY:
    return "CONCURRENCY";
  case TokenType::CONSEQUENCE:
    return "CONSEQUENCE";

  // Chronoscript Keywords
  case TokenType::DECLARE:
    return "DECLARE";
  case TokenType::FLUX:
    return "FLUX";
  case TokenType::SOURCE_INFUSED:
    return "SOURCE_INFUSED";

  // Temporal Types
  case TokenType::WEAVE_PATTERN:
    return "WEAVE_PATTERN";
  case TokenType::TEMPORAL_SIGNATURE:
    return "TEMPORAL_SIGNATURE";
  case TokenType::AETHEL_RESERVE:
    return "AETHEL_RESERVE";
  case TokenType::CHRONON_STREAM:
    return "CHRONON_STREAM";

  // Temporal Operations
  case TokenType::INFUSE:
    return "INFUSE";
  case TokenType::REWIND_TO:
    return "REWIND_TO";
  case TokenType::GUARD:
    return "GUARD";

  // Control Flow
  case TokenType::CATCH_TEMPORAL:
    return "CATCH_TEMPORAL";
  case TokenType::DEJAVU:
    return "DEJAVU";
  case TokenType::LOOT_TABLE:
    return "LOOT_TABLE";

  // Constants
  case TokenType::PARADOX_LEVEL:
    return "PARADOX_LEVEL";

  // For future use
  case TokenType::RESERVED_WORD:
    return "RESERVED_WORD";

  default:
    return "UNKNOWN";
  }
}

bool isKeyword(const std::string &identifier, TokenType &keywordType) {
  // Convert identifier to uppercase for case-insensitive comparison
  std::string upperIdentifier;
  upperIdentifier.reserve(identifier.size());
  for (char c : identifier) {
    upperIdentifier.push_back(
        static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
  }

  // Map of keywords to their token types
  static const std::unordered_map<std::string, TokenType> keywords = {
      // Variable Declaration
      {"LET", TokenType::LET},
      {"CONST", TokenType::CONST},

      // Control Flow
      {"IF", TokenType::IF},
      {"ELSE", TokenType::ELSE},
      {"WHILE", TokenType::WHILE},
      {"FOR", TokenType::FOR},
      {"FUNCTION", TokenType::FUNCTION},
      {"RETURN", TokenType::RETURN},

      // Boolean Literals and Special Values
      {"TRUE", TokenType::TRUE},
      {"FALSE", TokenType::FALSE},
      {"NULL", TokenType::NULL_VALUE},

      // Type System
      {"STATIC", TokenType::STATIC},
      {"VOLATILE", TokenType::VOLATILE},

      // Module System
      {"IMPORT", TokenType::IMPORT},
      {"EXPORT", TokenType::EXPORT},

      // Chronoscript Block Types
      {"ANTECEDENCE", TokenType::ANTECEDENCE},
      {"CONCURRENCY", TokenType::CONCURRENCY},
      {"CONSEQUENCE", TokenType::CONSEQUENCE},

      // Chronoscript Keywords
      {"DECLARE", TokenType::DECLARE},
      {"FLUX", TokenType::FLUX},
      {"SOURCE_INFUSED", TokenType::SOURCE_INFUSED},

      // Temporal Types
      {"WEAVE_PATTERN", TokenType::WEAVE_PATTERN},
      {"TEMPORAL_SIGNATURE", TokenType::TEMPORAL_SIGNATURE},
      {"AETHEL_RESERVE", TokenType::AETHEL_RESERVE},
      {"CHRONON_STREAM", TokenType::CHRONON_STREAM},

      // Temporal Operations
      {"INFUSE", TokenType::INFUSE},
      {"REWIND_TO", TokenType::REWIND_TO},
      {"GUARD", TokenType::GUARD},

      // Control Flow
      {"CATCH_TEMPORAL", TokenType::CATCH_TEMPORAL},
      {"DEJAVU", TokenType::DEJAVU},
      {"LOOT_TABLE", TokenType::LOOT_TABLE},

      // Constants
      {"PARADOX_LEVEL", TokenType::PARADOX_LEVEL},

      // Temporal Control
      {"BRANCH_TIMELINE", TokenType::BRANCH_TIMELINE},
      {"MERGE_TIMELINES", TokenType::MERGE_TIMELINES},
      {"REWIND_FLOW", TokenType::REWIND_FLOW},
      {"REVERSE_FLOW", TokenType::REVERSE_FLOW},
      {"ANCHOR", TokenType::ANCHOR},
      {"WEAVER", TokenType::WEAVER},

      // Resource Management
      {"CHRONON", TokenType::CHRONON},
      {"AETHEL", TokenType::AETHEL},
      {"ALLOCATE_RESOURCES", TokenType::ALLOCATE_RESOURCES},
      {"EXPEND_RESOURCES", TokenType::EXPEND_RESOURCES},
      {"BORROW_RESOURCES", TokenType::BORROW_RESOURCES},
      {"REPAY_DEBT", TokenType::REPAY_DEBT}};

  // First check exact matches
  auto it = keywords.find(upperIdentifier);
  if (it != keywords.end()) {
    keywordType = it->second;
    return true;
  }

  // For compound type names with underscores, try removing underscores
  if (upperIdentifier.find('_') != std::string::npos) {
    std::string noUnderscore;
    noUnderscore.reserve(upperIdentifier.size());
    std::remove_copy(upperIdentifier.begin(), upperIdentifier.end(),
                     std::back_inserter(noUnderscore), '_');

    it = keywords.find(noUnderscore);
    if (it != keywords.end()) {
      keywordType = it->second;
      return true;
    }
  }

  return false;
}

} // namespace compiler
} // namespace chronovyan