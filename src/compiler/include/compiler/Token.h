#ifndef CHRONOVYAN_TOKEN_H
#define CHRONOVYAN_TOKEN_H

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>

#include "compiler/SourceLocation.h"

namespace chronovyan {
namespace compiler {

/**
 * TokenType enumerates all possible token types in Chronovyan.
 */
enum class TokenType {
    // Single-character tokens
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    PERCENT,
    COLON,

    // One or two character tokens
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    PLUS_PLUS,
    MINUS_MINUS,
    PLUS_EQUAL,
    MINUS_EQUAL,
    STAR_EQUAL,
    SLASH_EQUAL,
    PERCENT_EQUAL,
    COLON_COLON,
    ARROW,

    // Logical operators
    AND,
    OR,

    // Literals
    IDENTIFIER,
    STRING,
    INTEGER,
    FLOAT,
    CHRONON_LITERAL,
    AETHEL_LITERAL,
    CHARACTER,

    // Keywords - Resource Management
    ALLOCATE_RESOURCES,
    EXPEND_RESOURCES,
    BORROW_RESOURCES,
    REPAY_DEBT,
    CHRONON,
    AETHEL,

    // Keywords - Variable Declaration
    LET,
    CONST,
    CONF,
    REB,

    // Keywords - Control Flow
    IF,
    ELSE,
    WHILE,
    FOR,
    FUNCTION,
    RETURN,

    // Keywords - Module System
    IMPORT,
    EXPORT,

    // Keywords - Temporal Control
    BRANCH_TIMELINE,
    MERGE_TIMELINES,
    REWIND_FLOW,
    REVERSE_FLOW,
    ANCHOR,
    WEAVER,

    // Keywords - Type System
    STATIC,
    VOLATILE,

    // Keywords - Boolean Literals and Null
    TRUE,
    FALSE,
    NULL_VALUE,

    // Special tokens and literals
    ERROR,
    END_OF_FILE,

    // Numeric literals with explicit base
    HEX_LITERAL,
    BINARY_LITERAL,
    OCTAL_LITERAL,

    // Special operators
    NULLISH_COALESCING,        // ??
    NULLISH_COALESCING_EQUAL,  // ??=
    OPTIONAL_CHAINING,         // ?.

    // Type operators
    IN,  // in

    // Other
    ELLIPSIS,  // ...

    // Chronoscript Block Types
    ANTECEDENCE,
    CONCURRENCY,
    CONSEQUENCE,

    // Chronoscript Keywords
    DECLARE,
    FLUX,
    SOURCE_INFUSED,

    // Temporal Types
    WEAVE_PATTERN,
    TEMPORAL_SIGNATURE,
    AETHEL_RESERVE,
    CHRONON_STREAM,

    // Temporal Operations
    INFUSE,
    REWIND_TO,
    GUARD,

    // Control Flow
    CATCH_TEMPORAL,
    DEJAVU,
    LOOT_TABLE,

    // Constants
    PARADOX_LEVEL,

    // For future use
    RESERVED_WORD,
};

/**
 * Token represents a lexical token in the Chronovyan language.
 */
class Token {
public:
    using ValueType = std::variant<std::monostate,  // No value
                                   int,             // Integer literal
                                   double,          // Float literal
                                   std::string,     // String literal or identifier
                                   bool             // Boolean literal
                                   >;

    Token() : type(TokenType::ERROR) {}

    Token(TokenType type, std::string lexeme, SourceLocation location)
        : type(type), lexeme(std::move(lexeme)), location(std::move(location)) {}

    Token(TokenType type, std::string lexeme, SourceLocation location, ValueType value)
        : type(type)
        , lexeme(std::move(lexeme))
        , location(std::move(location))
        , value(std::move(value)) {}

    // Getters
    TokenType getType() const { return type; }
    const std::string& getLexeme() const { return lexeme; }
    const SourceLocation& getLocation() const { return location; }

    // For testing purposes - get the raw lexeme
    const std::string& getLexemeForTesting() const { return lexeme; }
    const std::optional<ValueType>& getValue() const { return value; }

    // Value accessors
    template <typename T>
    const T& getAs() const {
        if (!value.has_value()) {
            throw std::runtime_error("Token has no value");
        }
        return std::get<T>(*value);
    }

    bool hasValue() const { return value.has_value(); }

    // String representation for debugging
    std::string toString() const;

protected:
    TokenType type;
    std::string lexeme;
    SourceLocation location;
    std::optional<ValueType> value;
};

// Output stream operator for Token
std::ostream& operator<<(std::ostream& os, const Token& token);

// Helper functions for working with TokenType
const char* tokenTypeToString(TokenType type);
bool isKeyword(const std::string& identifier, TokenType& keywordType);

}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_TOKEN_H