#ifndef CHRONOVYAN_COMPILER_PARSER_H
#define CHRONOVYAN_COMPILER_PARSER_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "compiler/AstNode.h"
#include "compiler/DiagnosticReporter.h"
#include "compiler/Lexer.h"
#include "compiler/ParserUtils.h"

namespace chronovyan {
namespace compiler {

// Forward declarations
class Lexer;
class DiagnosticReporter;

/**
 * @class Parser
 * @brief Recursive descent parser for the Chronovyan language
 *
 * The parser converts a sequence of tokens into an Abstract Syntax Tree (AST).
 * It uses a recursive descent parsing approach with error recovery.
 */
class Parser {
public:
    /**
     * @brief Construct a new Parser object
     * @param lexer The lexer to get tokens from
     * @param reporter The diagnostic reporter for error messages
     */
    explicit Parser(Lexer& lexer, DiagnosticReporter& reporter);
    ~Parser() = default;

    // Delete copy/move constructors and assignment operators
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(Parser&&) = delete;

    /**
     * @brief Parse the entire program
     * @return A program node representing the entire program, or nullptr on error
     */
    std::unique_ptr<ProgramNode> parse();

    /**
     * @brief Parse a temporal loop statement
     * @return The parsed temporal loop statement node, or nullptr on error
     */
    std::unique_ptr<TemporalLoopStmtNode> parseTemporalLoop();

    /**
     * @brief Parse a parallel execution statement
     * @return The parsed parallel execution statement node, or nullptr on error
     */
    std::unique_ptr<ParallelExecutionStmtNode> parseParallelExecution();

    /**
     * @brief Parse a branch timeline statement
     * @return The parsed branch timeline statement node, or nullptr on error
     */
    std::unique_ptr<BranchTimelineStmtNode> parseBranchTimeline();

    /**
     * @brief Parse a temporal statement
     * @return The parsed temporal statement node, or nullptr on error
     */
    std::unique_ptr<StmtNode> parseTemporalStatement();

    /**
     * @brief Parse a declaration
     * @return The parsed declaration node, or nullptr on error
     */
    std::unique_ptr<DeclNode> parseDeclaration();

    /**
     * @brief Parse a variable declaration
     * @return The parsed variable declaration node, or nullptr on error
     */
    std::unique_ptr<DeclNode> parseVariableDeclaration();

    /**
     * @brief Parse a function declaration
     * @return The parsed function declaration node, or nullptr on error
     */
    std::unique_ptr<DeclNode> parseFunctionDeclaration();

    /**
     * @brief Parse a statement
     * @return The parsed statement node, or nullptr on error
     */
    std::unique_ptr<StmtNode> parseStatement();

    /**
     * @brief Parse an expression statement
     * @return The parsed expression statement node, or nullptr on error
     */
    std::unique_ptr<StmtNode> parseExpressionStatement();

    /**
     * @brief Parse a block statement
     * @return The parsed block statement node, or nullptr on error
     */
    std::unique_ptr<StmtNode> parseBlockStatement();

    /**
     * @brief Parse an if statement
     * @return The parsed if statement node, or nullptr on error
     */
    std::unique_ptr<StmtNode> parseIfStatement();

    /**
     * @brief Parse a while statement
     * @return The parsed while statement node, or nullptr on error
     */
    std::unique_ptr<StmtNode> parseWhileStatement();

    /**
     * @brief Parse a for statement
     * @return The parsed for statement node, or nullptr on error
     */
    std::unique_ptr<StmtNode> parseForStatement();

    /**
     * @brief Parse a return statement
     * @return The parsed return statement node, or nullptr on error
     */
    std::unique_ptr<StmtNode> parseReturnStatement();

    /**
     * @brief Parse a resource statement
     * @return The parsed resource statement node, or nullptr on error
     */
    std::unique_ptr<StmtNode> parseResourceStatement();

    /**
     * @brief Parse an expression
     * @return The parsed expression node, or nullptr on error
     */
    std::unique_ptr<ExprNode> parseExpression();

    /**
     * @brief Parse an assignment
     * @return The parsed assignment node, or nullptr on error
     */
    std::unique_ptr<ExprNode> parseAssignment();

    /**
     * @brief Parse a logical or expression
     * @return The parsed logical or expression node, or nullptr on error
     */
    std::unique_ptr<ExprNode> parseLogicalOr();

    /**
     * @brief Parse a logical and expression
     * @return The parsed logical and expression node, or nullptr on error
     */
    std::unique_ptr<ExprNode> parseLogicalAnd();
    std::unique_ptr<ExprNode> parseEquality();
    std::unique_ptr<ExprNode> parseComparison();
    std::unique_ptr<ExprNode> parseTerm();
    std::unique_ptr<ExprNode> parseFactor();
    std::unique_ptr<ExprNode> parseUnary();
    std::unique_ptr<ExprNode> parsePostfix();
    std::unique_ptr<ExprNode> parsePrimary();

    // Type parsing
    std::unique_ptr<TypeNode> parseType();
};

}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_PARSER_H