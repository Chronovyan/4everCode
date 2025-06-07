// parser.h
#ifndef CHRONOVYAN_PARSER_H
#define CHRONOVYAN_PARSER_H

#include <map>
#include <memory>
#include <vector>

#include "ast_nodes.h"
#include "compiler/Lexer.h"
#include "loot.h"

namespace chronovyan {

class Parser {
public:
    Parser(std::shared_ptr<Lexer> lexer);
    ~Parser();

    /**
     * @brief Parse the entire program
     * @return A program node representing the entire program
     */
    std::unique_ptr<ProgramNode> parse();

    /**
     * @brief Parse a loot table declaration
     * @return A LootTable object containing the parsed loot entries
     */
    LootTable parseLootTable();

    /**
     * @brief Parse a TEMPORAL_LOOP statement
     * @return The parsed TEMPORAL_LOOP statement node
     */
    std::unique_ptr<TemporalLoopStmtNode> parseTemporalLoop();

    /**
     * @brief Parse a PARALLEL_EXECUTION statement
     * @return The parsed PARALLEL_EXECUTION statement node
     */
    std::unique_ptr<ParallelExecutionStmtNode> parseParallelExecution();

    /**
     * @brief Parse a BRANCH_TIMELINE statement
     * @return The parsed BRANCH_TIMELINE statement node
     */
    std::unique_ptr<BranchTimelineStmtNode> parseBranchTimeline();

    /**
     * @brief Parse a HALT_REWIND statement
     * @return The parsed HALT_REWIND statement node
     */
    std::unique_ptr<StmtNode> parseHaltRewindStatement();

private:
    std::shared_ptr<Lexer> m_lexer;
    Token m_current;
    Token m_previous;

    // Helper methods for token handling
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    void advance();
    bool check(TokenType type);
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    void consume(TokenType type, const std::string& message);
    bool checkNext(TokenType type) const;
    void synchronize();

    // Parsing methods
    std::unique_ptr<StmtNode> declaration();
    std::unique_ptr<StmtNode> varDeclaration();
    std::unique_ptr<StmtNode> statement();
    std::unique_ptr<BlockStmtNode> block();

    // Statement parsers
    std::unique_ptr<StmtNode> printStatement();
    std::unique_ptr<StmtNode> expressionStatement();
    std::unique_ptr<StmtNode> ifStatement();
    std::unique_ptr<StmtNode> whileEventStatement();
    std::unique_ptr<StmtNode> forChrononStatement();
    std::unique_ptr<StmtNode> returnStatement();
    std::unique_ptr<StmtNode> breakChrononStatement();
    std::unique_ptr<StmtNode> continueWeaveStatement();
    std::unique_ptr<StmtNode> switchCaseStatement();
    std::unique_ptr<StmtNode> rewindFlowStatement();
    std::unique_ptr<StmtNode> reverseFlowStatement();
    std::unique_ptr<StmtNode> temporalLoopStatement();
    std::unique_ptr<StmtNode> haltRewindStatement();
    std::unique_ptr<StmtNode> stabilizeTimelineStatement();
    std::unique_ptr<StmtNode> preventModificationStatement();

    // Expression parsers
    std::unique_ptr<ExprNode> expression();
    std::unique_ptr<ExprNode> assignment();
    std::unique_ptr<ExprNode> logicalOr();
    std::unique_ptr<ExprNode> logicalAnd();

    // Parse methods for different grammar rules
    std::unique_ptr<ExprNode> parseExpression();
    std::unique_ptr<StmtNode> parseStatement();
    std::unique_ptr<StmtNode> parseDeclaration();
    std::unique_ptr<VariableDeclStmtNode> parseVarDeclaration();
    std::unique_ptr<StmtNode> parseFunctionDeclaration();
    std::unique_ptr<BlockStmtNode> parseBlock();
    std::unique_ptr<IfStmtNode> parseIfStatement();
    std::unique_ptr<StmtNode> parseWhileStatement();
    std::unique_ptr<StmtNode> parseForStatement();
    std::unique_ptr<StmtNode> parseReturnStatement();
    std::unique_ptr<StmtNode> parsePrintStatement();
    std::unique_ptr<StmtNode> parseExpressionStatement();
    std::unique_ptr<ExprNode> parseAssignment();
    std::unique_ptr<ExprNode> parseLogicalOr();
    std::unique_ptr<ExprNode> parseLogicalAnd();
    std::unique_ptr<ExprNode> parseEquality();
    std::unique_ptr<ExprNode> parseComparison();
    std::unique_ptr<ExprNode> parseTerm();
    std::unique_ptr<ExprNode> parseFactor();
    std::unique_ptr<ExprNode> parseUnary();
    std::unique_ptr<ExprNode> parseCall();
    std::unique_ptr<ExprNode> parsePrimary();

    // Additional parse methods for compound types
    std::unique_ptr<ExprNode> finishCall(std::unique_ptr<ExprNode> callee);
    std::unique_ptr<ExprNode> parseObjectLiteral();
    std::unique_ptr<ExprNode> parseArrayLiteral();

    /**
     * @brief Parse a type definition statement
     * @return A TypeDefStmtNode representing the type definition
     */
    std::unique_ptr<TypeDefStmtNode> parseTypeDefinition();

    /**
     * @brief Parse a struct type definition
     * @param location The source location
     * @param name The type name
     * @param typeParameters The type parameters for generic types
     * @return The parsed struct type definition statement
     */
    std::unique_ptr<TypeDefStmtNode> parseStructDefinition(
        const SourceLocation& location, const std::string& name,
        const std::vector<std::string>& typeParameters = {});

    /**
     * @brief Parse an enum type definition
     * @param location The source location
     * @param name The type name
     * @param typeParameters The type parameters for generic types
     * @return The parsed enum type definition statement
     */
    std::unique_ptr<TypeDefStmtNode> parseEnumDefinition(
        const SourceLocation& location, const std::string& name,
        const std::vector<std::string>& typeParameters = {});

    /**
     * @brief Parse a union type definition
     * @param location The source location
     * @param name The type name
     * @param typeParameters The type parameters for generic types
     * @return The parsed union type definition statement
     */
    std::unique_ptr<TypeDefStmtNode> parseUnionDefinition(
        const SourceLocation& location, const std::string& name,
        const std::vector<std::string>& typeParameters = {});

    /**
     * @brief Parse an alias type definition
     * @param location The source location
     * @param name The type name
     * @param typeParameters The type parameters for generic types
     * @return The parsed alias type definition statement
     */
    std::unique_ptr<TypeDefStmtNode> parseAliasDefinition(
        const SourceLocation& location, const std::string& name,
        const std::vector<std::string>& typeParameters = {});

    /**
     * @brief Parse type parameters (for generic types)
     * @return A vector of type parameter names
     */
    std::vector<std::string> parseTypeParameters();

    // New parse methods for loop constructs

    /**
     * @brief Parse a FOR_CHRONON loop statement
     * @return A ForChrononStmtNode representing the loop
     */
    std::unique_ptr<ForChrononStmtNode> parseForChrononLoop();

    /**
     * @brief Parse a WHILE_EVENT loop statement
     * @return A WhileEventStmtNode representing the loop
     */
    std::unique_ptr<WhileEventStmtNode> parseWhileEventLoop();

    /**
     * @brief Parse a REWIND_FLOW loop statement
     * @return A RewindFlowStmtNode representing the loop
     */
    std::unique_ptr<RewindFlowStmtNode> parseRewindFlowLoop();

    // New parse methods for error handling constructs

    /**
     * @brief Parse an ATTEMPT_WEAVE statement with optional ON_DISSONANCE and ENSURE_HARMONY blocks
     * @return An AttemptWeaveStmtNode representing the error handling structure
     */
    std::unique_ptr<AttemptWeaveStmtNode> parseAttemptWeave();

    /**
     * @brief Parse a DISSONANCE_HANDLER block
     * @return A DissonanceHandlerNode representing the error handler
     */
    std::unique_ptr<DissonanceHandlerNode> parseDissonanceHandler();

    /**
     * @brief Parse a BREAK_CHRONON statement
     * @return A BreakChrononStmtNode representing the break statement
     */
    std::unique_ptr<BreakChrononStmtNode> parseBreakChrononStatement();

    /**
     * @brief Parse a CONTINUE_WEAVE statement
     * @return A ContinueWeaveStmtNode representing the continue statement
     */
    std::unique_ptr<ContinueWeaveStmtNode> parseContinueWeaveStatement();

    /**
     * @brief Parse a REVERSE_FLOW statement
     * @return A ReverseFlowStmtNode representing the statement
     */
    std::unique_ptr<ReverseFlowStmtNode> parseReverseFlowStatement();

    /**
     * @brief Parse a STABILIZE_TIMELINE statement
     * @return A StmtNode representing the stabilize timeline statement
     */
    std::unique_ptr<StmtNode> parseStabilizeTimelineStatement();

    /**
     * @brief Parse a PREVENT_MODIFICATION statement
     * @return A StmtNode representing the prevent modification statement
     */
    std::unique_ptr<StmtNode> parsePreventModificationStatement();

    /**
     * @brief Parse an EXPEND_RESOURCES statement
     * @return An ExpendResourcesStmtNode representing the resource expenditure
     */
    std::unique_ptr<ExpendResourcesStmtNode> parseExpendResourcesStatement();

    /**
     * @brief Parse a RESOURCE_INSUFFICIENCY_HANDLER block
     * @return A ResourceInsufficiencyHandlerNode representing the handler
     */
    std::unique_ptr<ResourceInsufficiencyHandlerNode> parseResourceInsufficiencyHandler();

    /**
     * @brief Parse a TEMPORAL_PROGRAM block
     * @return A TemporalProgramNode representing the temporal program
     */
    std::unique_ptr<TemporalProgramNode> parseTemporalProgram();

    /**
     * @brief Parse a temporal section of a specific type
     * @param sectionType The type of temporal section
     * @return A TemporalSectionNode representing the section
     */
    std::unique_ptr<TemporalSectionNode> parseTemporalSection(TemporalSectionType sectionType);

    /**
     * @brief Parse a VARIABLES section
     * @return A TemporalSectionNode representing the variables section
     */
    std::unique_ptr<TemporalSectionNode> parseVariablesSection();

    /**
     * @brief Parse an INITIALIZATION section
     * @return A TemporalSectionNode representing the initialization section
     */
    std::unique_ptr<TemporalSectionNode> parseInitializationSection();

    /**
     * @brief Parse an EXECUTION_WEAVE section
     * @return A TemporalSectionNode representing the execution weave section
     */
    std::unique_ptr<TemporalSectionNode> parseExecutionWeaveSection();

    /**
     * @brief Parse a CLEANUP section
     * @return A TemporalSectionNode representing the cleanup section
     */
    std::unique_ptr<TemporalSectionNode> parseCleanupSection();

    /**
     * @brief Parse a metadata section of a specific type
     * @param sectionType The type of metadata section
     * @return A TemporalSectionNode representing the metadata section
     */
    std::unique_ptr<TemporalSectionNode> parseMetadataSection(TemporalSectionType sectionType);

    /**
     * @brief Parse a variable declaration with a modifier
     * @param modifier The variable modifier
     * @return A VariableDeclStmtNode representing the declaration
     */
    std::unique_ptr<VariableDeclStmtNode> parseModifiedVarDeclaration(VariableModifier modifier);

    /**
     * @brief Parse a DECLARE_CONSTANT statement
     * @return A DeclareConstantNode representing the constant declaration
     */
    std::unique_ptr<DeclareConstantNode> parseDeclareConstant();

    /**
     * @brief Parse variable flags
     * @return A vector of variable flags
     */
    std::vector<VariableFlag> parseVariableFlags();

    /**
     * @brief Convert a token to a variable flag
     * @param token The token to convert
     * @return The corresponding variable flag
     */
    VariableFlag tokenToVariableFlag(const Token& token);

    /**
     * @brief Parse a type
     * @return A pointer to a type node
     */
    std::shared_ptr<TypeNode> parseType();
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_PARSER_H