#ifndef CHRONOVYAN_TOKEN_H
#define CHRONOVYAN_TOKEN_H

#include <string>
#include <unordered_map>

#include "source_location.h"

namespace chronovyan {

/**
 * @enum TokenType
 * @brief Defines all possible token types in Chronovyan
 */
enum class TokenType {
    // End of file
    EOF_TOKEN,

    // Error token
    ERROR,

    // Identifiers and literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,

    // Program structure
    TEMPORAL_PROGRAM,
    NAME,
    TYPE,
    AUTHOR,
    VERSION,
    DESCRIPTION,
    RESOURCES_REQUIRED,
    TIMELINE_BEHAVIOR,
    STABILITY_TARGET,
    DEFAULT_FLOW,
    PARADOX_HANDLING,
    IMPORT_WEAVES,
    VARIABLES,
    INITIALIZATION_PHASE,
    EXECUTION_WEAVE,
    CLEANUP_PHASE,

    // Adding missing program structure tokens
    DEPENDENCIES,
    TIMELINE,
    ENTRY,
    EXIT,

    // Keywords
    DECLARE,
    IF,
    ELSE,
    FOR_CHRONON,
    WHILE_EVENT,
    REWIND_FLOW,
    BRANCH_TIMELINE,
    MERGE_TIMELINES,
    TEMPORAL_ECHO_LOOP,
    TEMPORAL_LOOP,
    PARALLEL_EXECUTION,
    PARADOX_HANDLER,
    TRUE,
    FALSE,
    COLLAPSE,
    RETURN,
    BREAK_CHRONON,
    SKIP_ITERATION,
    STABILIZE_FLOW,
    BREAK_EVENT,
    CHECK_DEADLINE,
    HALT_REWIND,
    STABILIZE_TIMELINE,
    PREVENT_MODIFICATION,
    CONTINUE,
    VAR,  // Adding VAR token for variable declarations

    // Adding missing keywords
    BREAK,
    CONST,
    NATIVE,
    PARADOX,
    TRY,
    CATCH,
    FINALLY,
    THROW,
    EXPORT,
    MUTABLE,

    // Temporal control flow
    CONTINUE_WEAVE,  // New - enhanced version of CONTINUE for Chronovyan loops
    REVERSE_FLOW,    // New - inverts the iteration direction in loops

    // Parallel execution tokens
    THREADS,     // New - specifies the number of threads for PARALLEL_EXECUTION
    SYNC_POINT,  // New - synchronization point for parallel threads
    THREAD_ID,   // New - identifies the current thread in a parallel execution

    // Resource management
    EXPEND_RESOURCES,           // New - conditionally execute a block based on available resources
    ON_INSUFFICIENT_RESOURCES,  // New - handler for when resources are insufficient

    // Error handling
    ATTEMPT_WEAVE,
    ON_DISSONANCE,
    ENSURE_HARMONY,
    RAISE_DISSONANCE,
    TEMPORAL_ASSERT,

    // Function-related
    DEFINE_PATTERN,
    DEFINE_TYPE,
    DISSONANT,  // +DISSONANT flag for functions

    // Variable modifiers
    CONF,
    REB,

    // Variable flags
    STATIC,
    VOLATILE,
    ANCHOR,
    TIMELINE_PERSISTENT,
    WEAVER,
    PARADOX_RESISTANT,
    FLUX,
    ECHO,
    IMMUTABLE,

    // Types
    INT,
    FLOAT,
    BOOLEAN,
    STRING,
    VOID,
    ARRAY,
    MAP,
    TUPLE,
    SET,
    OPTIONAL,
    TIMESTAMP,
    DURATION,  // For TEMPORAL_LOOP duration parameter
    VARIANCE,  // For TEMPORAL_LOOP variance parameter
    QUANTUM_STATE,
    ANY,
    NULL_TYPE,
    CHRONON,   // Adding CHRONON token
    EVENT,     // Adding EVENT token
    ENTITY,    // Adding ENTITY token
    PATTERN,   // Adding PATTERN token
    WEAVE,     // Adding WEAVE token
    THREAD,    // Adding THREAD token
    RESOURCE,  // Adding RESOURCE token

    // Type-related keywords
    STRUCTURE,
    ENUMERATION,
    INTERFACE,
    METHOD,
    STATIC_METHOD,
    TYPEDEF,
    IS,
    AS,

    // Custom type definitions
    STRUCT,
    ENUM,
    UNION,
    ALIAS,

    // Resource management
    AETHEL,
    CHRONONS,
    ALLOCATE_AETHEL,
    ALLOCATE_CHRONONS,
    RELEASE_AETHEL,
    RELEASE_CHRONONS,
    CONSUME_AETHEL,
    CONSUME_CHRONON,
    GET_AETHEL_LEVEL,
    GET_CHRONONS_LEVEL,

    // Adding missing resource management tokens
    ALLOCATE,
    RELEASE,
    PRESERVE,
    CONSUME,

    // Operators
    PLUS,     // +
    MINUS,    // -
    STAR,     // *
    SLASH,    // /
    PERCENT,  // %

    // Comparison operators
    EQUAL_EQUAL,    // ==
    BANG_EQUAL,     // !=
    LESS,           // <
    LESS_EQUAL,     // <=
    GREATER,        // >
    GREATER_EQUAL,  // >=

    // Assignment operators
    EQUAL,          // =
    PLUS_EQUAL,     // +=
    MINUS_EQUAL,    // -=
    STAR_EQUAL,     // *=
    SLASH_EQUAL,    // /=
    PERCENT_EQUAL,  // %=

    // Logical operators
    AND,   // &&
    OR,    // ||
    BANG,  // !

    // Special characters and operators
    QUESTION_MARK,  // ?
    AT,             // @
    HASH,           // #
    BITWISE_AND,    // &
    BITWISE_OR,     // |
    BITWISE_XOR,    // ^
    BITWISE_NOT,    // ~

    // Delimiters
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    COMMA,              // ,
    DOT,                // .
    COLON,              // :
    SEMICOLON,          // ;
    DOUBLE_COLON,       // ::
    ARROW,              // ->
    FAT_ARROW,          // =>
    QUESTION,           // ?
    QUESTION_QUESTION,  // ??

    // Temporal operators
    TIMELINE_BRANCH,  // ->
    TIMELINE_MERGE,   // <-
    TEMPORAL_QUERY,   // ?:

    FUNCTION,  // Add FUNCTION token type
    PRINT,     // Add PRINT token type
    IMPORT,    // IMPORT directive
};

/**
 * @struct Token
 * @brief Represents a token in the Chronovyan language
 */
struct Token {
    TokenType type;           // The type of token
    std::string lexeme;       // The raw text of the token
    SourceLocation location;  // Where the token appears

    /**
     * @brief Construct a token with known properties
     */
    Token(TokenType tokenType, std::string tokenLexeme, SourceLocation tokenLocation)
        : type(tokenType), lexeme(std::move(tokenLexeme)), location(std::move(tokenLocation)) {}

    /**
     * @brief Default constructor creates an ERROR token
     */
    Token() : type(TokenType::ERROR), lexeme(""), location() {}

    /**
     * @brief Check if this token is of a specific type
     */
    bool isType(TokenType t) const { return type == t; }

    /**
     * @brief Get a string representation of the token type
     */
    std::string typeToString() const;

    /**
     * @brief Get a string representation of this token
     */
    std::string toString() const;
};

/**
 * @brief Get a mapping of keywords to token types
 */
const std::unordered_map<std::string, TokenType>& getKeywordMap();

}  // namespace chronovyan

#endif  // CHRONOVYAN_TOKEN_H