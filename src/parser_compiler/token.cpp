#include <sstream>
#include <unordered_map>

#include "token.h"

namespace chronovyan {

std::string Token::typeToString() const {
    switch (type) {
        case TokenType::EOF_TOKEN:
            return "EOF";
        case TokenType::ERROR:
            return "ERROR";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL:
            return "INTEGER";
        case TokenType::FLOAT_LITERAL:
            return "FLOAT";
        case TokenType::STRING_LITERAL:
            return "STRING";

        // Program Structure
        case TokenType::TEMPORAL_PROGRAM:
            return "TEMPORAL_PROGRAM";
        case TokenType::NAME:
            return "NAME";
        case TokenType::TYPE:
            return "TYPE";
        case TokenType::AUTHOR:
            return "AUTHOR";
        case TokenType::VERSION:
            return "VERSION";
        case TokenType::DEPENDENCIES:
            return "DEPENDENCIES";
        case TokenType::TIMELINE:
            return "TIMELINE";
        case TokenType::ENTRY:
            return "ENTRY";
        case TokenType::EXIT:
            return "EXIT";

        // Keywords
        case TokenType::DECLARE:
            return "DECLARE";
        case TokenType::IF:
            return "IF";
        case TokenType::ELSE:
            return "ELSE";
        case TokenType::FOR_CHRONON:
            return "FOR_CHRONON";
        case TokenType::WHILE_EVENT:
            return "WHILE_EVENT";
        case TokenType::REWIND_FLOW:
            return "REWIND_FLOW";
        case TokenType::BRANCH_TIMELINE:
            return "BRANCH_TIMELINE";
        case TokenType::MERGE_TIMELINES:
            return "MERGE_TIMELINES";
        case TokenType::TEMPORAL_ECHO_LOOP:
            return "TEMPORAL_ECHO_LOOP";
        case TokenType::TEMPORAL_LOOP:
            return "TEMPORAL_LOOP";
        case TokenType::PARALLEL_EXECUTION:
            return "PARALLEL_EXECUTION";
        case TokenType::PARADOX_HANDLER:
            return "PARADOX_HANDLER";
        case TokenType::TRUE:
            return "TRUE";
        case TokenType::FALSE:
            return "FALSE";
        case TokenType::COLLAPSE:
            return "COLLAPSE";
        case TokenType::VAR:
            return "VAR";

        // New Keywords
        case TokenType::DEFINE_PATTERN:
            return "DEFINE_PATTERN";
        case TokenType::DEFINE_TYPE:
            return "DEFINE_TYPE";
        case TokenType::ATTEMPT_WEAVE:
            return "ATTEMPT_WEAVE";
        case TokenType::EXPORT:
            return "EXPORT";
        case TokenType::IMPORT:
            return "IMPORT";
        case TokenType::RETURN:
            return "RETURN";
        case TokenType::BREAK:
            return "BREAK";
        case TokenType::CONTINUE:
            return "CONTINUE";
        case TokenType::CONST:
            return "CONST";
        case TokenType::NATIVE:
            return "NATIVE";
        case TokenType::PARADOX:
            return "PARADOX";
        case TokenType::TRY:
            return "TRY";
        case TokenType::CATCH:
            return "CATCH";
        case TokenType::FINALLY:
            return "FINALLY";
        case TokenType::THROW:
            return "THROW";

        // Variable modifiers
        case TokenType::CONF:
            return "CONF";
        case TokenType::REB:
            return "REB";
        case TokenType::MUTABLE:
            return "MUTABLE";
        case TokenType::IMMUTABLE:
            return "IMMUTABLE";

        // Variable flags
        case TokenType::STATIC:
            return "STATIC";
        case TokenType::VOLATILE:
            return "VOLATILE";
        case TokenType::ANCHOR:
            return "ANCHOR";
        case TokenType::WEAVER:
            return "WEAVER";
        case TokenType::FLUX:
            return "FLUX";
        case TokenType::ECHO:
            return "ECHO";

        // Types
        case TokenType::INT:
            return "INT";
        case TokenType::FLOAT:
            return "FLOAT";
        case TokenType::BOOLEAN:
            return "BOOLEAN";
        case TokenType::STRING:
            return "STRING";
        case TokenType::VOID:
            return "VOID";
        case TokenType::ARRAY:
            return "ARRAY";
        case TokenType::MAP:
            return "MAP";
        case TokenType::TUPLE:
            return "TUPLE";
        case TokenType::TIMESTAMP:
            return "TIMESTAMP";
        case TokenType::QUANTUM_STATE:
            return "QUANTUM_STATE";
        case TokenType::CHRONON:
            return "CHRONON";
        case TokenType::EVENT:
            return "EVENT";
        case TokenType::ENTITY:
            return "ENTITY";
        case TokenType::PATTERN:
            return "PATTERN";
        case TokenType::WEAVE:
            return "WEAVE";
        case TokenType::THREAD:
            return "THREAD";
        case TokenType::RESOURCE:
            return "RESOURCE";

        // Resource Management
        case TokenType::ALLOCATE:
            return "ALLOCATE";
        case TokenType::RELEASE:
            return "RELEASE";
        case TokenType::PRESERVE:
            return "PRESERVE";
        case TokenType::CONSUME:
            return "CONSUME";

        // Operators
        case TokenType::PLUS:
            return "+";
        case TokenType::MINUS:
            return "-";
        case TokenType::STAR:
            return "*";
        case TokenType::SLASH:
            return "/";
        case TokenType::PERCENT:
            return "%";
        case TokenType::QUESTION_MARK:
            return "?";
        case TokenType::AT:
            return "@";
        case TokenType::HASH:
            return "#";
        case TokenType::BITWISE_AND:
            return "&";
        case TokenType::BITWISE_OR:
            return "|";
        case TokenType::BITWISE_XOR:
            return "^";
        case TokenType::BITWISE_NOT:
            return "~";

        // Comparison operators
        case TokenType::EQUAL_EQUAL:
            return "==";
        case TokenType::BANG_EQUAL:
            return "!=";
        case TokenType::LESS:
            return "<";
        case TokenType::LESS_EQUAL:
            return "<=";
        case TokenType::GREATER:
            return ">";
        case TokenType::GREATER_EQUAL:
            return ">=";

        // Assignment operators
        case TokenType::EQUAL:
            return "=";
        case TokenType::PLUS_EQUAL:
            return "+=";
        case TokenType::MINUS_EQUAL:
            return "-=";
        case TokenType::STAR_EQUAL:
            return "*=";
        case TokenType::SLASH_EQUAL:
            return "/=";
        case TokenType::PERCENT_EQUAL:
            return "%=";

        // Logical operators
        case TokenType::AND:
            return "&&";
        case TokenType::OR:
            return "||";
        case TokenType::BANG:
            return "!";

        // Delimiters
        case TokenType::LEFT_PAREN:
            return "(";
        case TokenType::RIGHT_PAREN:
            return ")";
        case TokenType::LEFT_BRACE:
            return "{";
        case TokenType::RIGHT_BRACE:
            return "}";
        case TokenType::LEFT_BRACKET:
            return "[";
        case TokenType::RIGHT_BRACKET:
            return "]";
        case TokenType::COMMA:
            return ",";
        case TokenType::DOT:
            return ".";
        case TokenType::COLON:
            return ":";
        case TokenType::SEMICOLON:
            return ";";
        case TokenType::DOUBLE_COLON:
            return "::";

        // Temporal operators
        case TokenType::TIMELINE_BRANCH:
            return "->";
        case TokenType::TIMELINE_MERGE:
            return "<-";
        case TokenType::TEMPORAL_QUERY:
            return "?:";

        // Temporal control flow
        case TokenType::BREAK_CHRONON:
            return "BREAK_CHRONON";
        case TokenType::CONTINUE_WEAVE:
            return "CONTINUE_WEAVE";
        case TokenType::REVERSE_FLOW:
            return "REVERSE_FLOW";

        // Parallel execution tokens
        case TokenType::THREADS:
            return "THREADS";
        case TokenType::SYNC_POINT:
            return "SYNC_POINT";
        case TokenType::THREAD_ID:
            return "THREAD_ID";

        // Resource management
        case TokenType::EXPEND_RESOURCES:
            return "EXPEND_RESOURCES";
        case TokenType::ON_INSUFFICIENT_RESOURCES:
            return "ON_INSUFFICIENT_RESOURCES";

        // Custom type definitions
        case TokenType::STRUCT:
            return "STRUCT";
        case TokenType::ENUM:
            return "ENUM";
        case TokenType::UNION:
            return "UNION";
        case TokenType::ALIAS:
            return "ALIAS";

        // Resource tokens
        case TokenType::AETHEL:
            return "AETHEL";
        case TokenType::CHRONONS:
            return "CHRONONS";

        // TEMPORAL_LOOP parameters
        case TokenType::DURATION:
            return "DURATION";
        case TokenType::VARIANCE:
            return "VARIANCE";

        default:
            return "UNKNOWN";
    }
}

std::string Token::toString() const {
    std::stringstream ss;
    ss << typeToString() << " '" << lexeme << "'";

    if (location.isValid()) {
        ss << " at " << location.toString();
    }

    return ss.str();
}

const std::unordered_map<std::string, TokenType>& getKeywordMap() {
    static const std::unordered_map<std::string, TokenType> keywordMap = {
        // Program Structure
        {"TEMPORAL_PROGRAM", TokenType::TEMPORAL_PROGRAM},
        {"NAME", TokenType::NAME},
        {"TYPE", TokenType::TYPE},
        {"AUTHOR", TokenType::AUTHOR},
        {"VERSION", TokenType::VERSION},
        {"DEPENDENCIES", TokenType::DEPENDENCIES},
        {"TIMELINE", TokenType::TIMELINE},
        {"ENTRY", TokenType::ENTRY},
        {"EXIT", TokenType::EXIT},

        // Existing Keywords
        {"DECLARE", TokenType::DECLARE},
        {"IF", TokenType::IF},
        {"ELSE", TokenType::ELSE},
        {"FOR_CHRONON", TokenType::FOR_CHRONON},
        {"WHILE_EVENT", TokenType::WHILE_EVENT},
        {"REWIND_FLOW", TokenType::REWIND_FLOW},
        {"BRANCH_TIMELINE", TokenType::BRANCH_TIMELINE},
        {"MERGE_TIMELINES", TokenType::MERGE_TIMELINES},
        {"TEMPORAL_ECHO_LOOP", TokenType::TEMPORAL_ECHO_LOOP},
        {"TEMPORAL_LOOP", TokenType::TEMPORAL_LOOP},
        {"PARALLEL_EXECUTION", TokenType::PARALLEL_EXECUTION},
        {"PARADOX_HANDLER", TokenType::PARADOX_HANDLER},
        {"TRUE", TokenType::TRUE},
        {"FALSE", TokenType::FALSE},
        {"COLLAPSE", TokenType::COLLAPSE},
        {"VAR", TokenType::VAR},

        // Add lowercase versions of keywords for testing
        {"var", TokenType::VAR},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},

        // New Keywords
        {"DEFINE_PATTERN", TokenType::DEFINE_PATTERN},
        {"DEFINE_TYPE", TokenType::DEFINE_TYPE},
        {"ATTEMPT_WEAVE", TokenType::ATTEMPT_WEAVE},
        {"EXPORT", TokenType::EXPORT},
        {"IMPORT", TokenType::IMPORT},
        {"RETURN", TokenType::RETURN},
        {"BREAK", TokenType::BREAK},
        {"CONTINUE", TokenType::CONTINUE},
        {"CONST", TokenType::CONST},
        {"NATIVE", TokenType::NATIVE},
        {"PARADOX", TokenType::PARADOX},
        {"TRY", TokenType::TRY},
        {"CATCH", TokenType::CATCH},
        {"FINALLY", TokenType::FINALLY},
        {"THROW", TokenType::THROW},

        // Add lowercase versions of these keywords
        {"return", TokenType::RETURN},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"const", TokenType::CONST},
        {"try", TokenType::TRY},
        {"catch", TokenType::CATCH},
        {"finally", TokenType::FINALLY},
        {"throw", TokenType::THROW},

        // Variable modifiers
        {"CONF", TokenType::CONF},
        {"REB", TokenType::REB},
        {"MUTABLE", TokenType::MUTABLE},
        {"IMMUTABLE", TokenType::IMMUTABLE},

        // Variable flags
        {"STATIC", TokenType::STATIC},
        {"VOLATILE", TokenType::VOLATILE},
        {"ANCHOR", TokenType::ANCHOR},
        {"WEAVER", TokenType::WEAVER},
        {"FLUX", TokenType::FLUX},
        {"ECHO", TokenType::ECHO},

        // Types
        {"INT", TokenType::INT},
        {"FLOAT", TokenType::FLOAT},
        {"BOOLEAN", TokenType::BOOLEAN},
        {"STRING", TokenType::STRING},
        {"VOID", TokenType::VOID},
        {"ARRAY", TokenType::ARRAY},
        {"MAP", TokenType::MAP},
        {"TUPLE", TokenType::TUPLE},
        {"TIMESTAMP", TokenType::TIMESTAMP},
        {"QUANTUM_STATE", TokenType::QUANTUM_STATE},
        {"CHRONON", TokenType::CHRONON},
        {"EVENT", TokenType::EVENT},
        {"ENTITY", TokenType::ENTITY},
        {"PATTERN", TokenType::PATTERN},
        {"WEAVE", TokenType::WEAVE},
        {"THREAD", TokenType::THREAD},
        {"RESOURCE", TokenType::RESOURCE},

        // Resource Management
        {"ALLOCATE", TokenType::ALLOCATE},
        {"RELEASE", TokenType::RELEASE},
        {"PRESERVE", TokenType::PRESERVE},
        {"CONSUME", TokenType::CONSUME},

        // Added for completeness
        {"ON_DISSONANCE", TokenType::ON_DISSONANCE},
        {"ENSURE_HARMONY", TokenType::ENSURE_HARMONY},

        // Temporal control flow
        {"BREAK_CHRONON", TokenType::BREAK_CHRONON},
        {"CONTINUE_WEAVE", TokenType::CONTINUE_WEAVE},
        {"REVERSE_FLOW", TokenType::REVERSE_FLOW},

        // Parallel execution tokens
        {"THREADS", TokenType::THREADS},
        {"SYNC_POINT", TokenType::SYNC_POINT},
        {"THREAD_ID", TokenType::THREAD_ID},

        // Resource management
        {"EXPEND_RESOURCES", TokenType::EXPEND_RESOURCES},
        {"ON_INSUFFICIENT_RESOURCES", TokenType::ON_INSUFFICIENT_RESOURCES},

        // Type Keywords
        {"AS", TokenType::AS},
        {"IS", TokenType::IS},
        {"STRUCT", TokenType::STRUCT},
        {"ENUM", TokenType::ENUM},
        {"UNION", TokenType::UNION},
        {"ALIAS", TokenType::ALIAS},

        // Lowercase type keywords
        {"as", TokenType::AS},
        {"is", TokenType::IS},
        {"struct", TokenType::STRUCT},
        {"enum", TokenType::ENUM},
        {"union", TokenType::UNION},

        // Resource tokens
        {"AETHEL", TokenType::AETHEL},
        {"CHRONONS", TokenType::CHRONONS},

        // TEMPORAL_LOOP parameters
        {"DURATION", TokenType::DURATION},
        {"VARIANCE", TokenType::VARIANCE},
    };

    return keywordMap;
}

}  // namespace chronovyan