#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "compiler/AstNode.h"
#include "compiler/Parser.h"
#include "compiler/ParserUtils.h"

namespace chronovyan {
namespace compiler {

using namespace ErrorMessages;

Parser::Parser(Lexer& lexer, DiagnosticReporter& reporter)
    : lexer_(lexer), reporter_(reporter), current_(lexer_.nextToken()) {
    // The first token is already read during initialization
}

std::unique_ptr<AstNode> Parser::parse() {
    // Remove unnecessary try-catch and rely on more structured error handling
    auto program = parseProgram();

    // Return null only if we couldn't create any program node
    if (!program) {
        // Add a diagnostic message about catastrophic parsing failure
        reporter_.addError(current_,
                           "Catastrophic parsing error: Unable to parse program structure");
    }

    return program;
}

void Parser::advance() {
    previous_ = current_;

    // Keep consuming tokens until we get a non-error token or reach the end
    do {
        current_ = lexer_.nextToken();
    } while (current_.getType() == TokenType::ERROR && !isAtEnd());
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return current_.getType() == type;
}

bool Parser::isAtEnd() const { return current_.getType() == TokenType::END_OF_FILE; }

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        Token token = current_;
        advance();
        return token;
    }

    reporter_.addError(current_, message);
    Token errorToken = current_;
    synchronize();
    return errorToken;
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (current_.getType() == TokenType::SEMICOLON) {
            advance();
            return;
        }

        switch (current_.getType()) {
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::FUNCTION:
            case TokenType::LET:
            case TokenType::CONST:
            case TokenType::RETURN:
            case TokenType::ALLOCATE_RESOURCES:
            case TokenType::EXPEND_RESOURCES:
            case TokenType::BORROW_RESOURCES:
            case TokenType::REPAY_DEBT:
            case TokenType::BRANCH_TIMELINE:
            case TokenType::MERGE_TIMELINES:
            case TokenType::REWIND_FLOW:
            case TokenType::REVERSE_FLOW:
                return;
            default:
                break;
        }

        advance();
    }
}

// Modified to handle errors without exceptions
std::unique_ptr<AstNode> Parser::parseProgram() {
    std::vector<std::unique_ptr<DeclNode>> declarations;

    while (!isAtEnd()) {
        try {
            auto decl = parseDeclaration();
            if (decl) {
                declarations.push_back(std::move(decl));
            } else {
                // Skip to the next declaration on error
                synchronize();
            }
        } catch (const std::exception& e) {
            reporter_.addError(current_, std::string("Unexpected error: ") + e.what());
            synchronize();
        }
    }

    return std::make_unique<ProgramNode>(std::move(declarations));
}

std::unique_ptr<DeclNode> Parser::parseDeclaration() {
    if (match(TokenType::LET) || match(TokenType::CONST)) {
        try {
            return parseVariableDeclaration();
        } catch (const std::exception& e) {
            reporter_.addError(current_, std::string("Error in variable declaration: ") + e.what());
            synchronize();
            return nullptr;
        }
    }

    // Wrap the statement in a StmtDeclNode
    auto stmt = parseStatement();
    if (stmt) {
        return std::make_unique<StmtDeclNode>(std::move(stmt));
    }

    // If the statement is null (due to error recovery), return nullptr
    return nullptr;
}

std::unique_ptr<DeclNode> Parser::parseVariableDeclaration() {
    Token name = current_;

    if (!check(TokenType::IDENTIFIER)) {
        reporter_.addError(current_, "Expect variable name after 'let' or 'const'");
        synchronize();
        return nullptr;
    }

    advance();  // Consume the identifier

    // Check for type annotations like ::CONF, ::REB
    std::unique_ptr<TypeNode> type = nullptr;
    if (match(TokenType::COLON_COLON)) {
        if (!check(TokenType::IDENTIFIER)) {
            reporter_.addError(current_, "Expect type name after '::'");
        } else {
            // Here we would parse the type annotations
            advance();  // Consume the type identifier
        }
    }

    // Check for initializer
    std::unique_ptr<ExprNode> initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = parseExpression();
        if (!initializer) {
            reporter_.addError(current_, "Invalid or missing initializer expression");
        }
    }

    // Look for semicolon, but continue even if missing
    if (!check(TokenType::SEMICOLON)) {
        reporter_.addError(current_, "Expect ';' after variable declaration");
    } else {
        advance();  // Consume the semicolon
    }

    return std::make_unique<VarDeclNode>(std::move(name), std::move(type), std::move(initializer));
}

std::unique_ptr<StmtNode> Parser::parseStatement() {
    try {
        if (match(TokenType::LEFT_BRACE)) {
            return parseBlockStatement();
        }

        if (match(TokenType::IF)) {
            return parseIfStatement();
        }

        if (match(TokenType::WHILE)) {
            return parseWhileStatement();
        }

        if (match(TokenType::FOR)) {
            return parseForStatement();
        }

        if (match(TokenType::RETURN)) {
            return parseReturnStatement();
        }

        if (match({TokenType::ALLOCATE_RESOURCES, TokenType::EXPEND_RESOURCES,
                   TokenType::BORROW_RESOURCES, TokenType::REPAY_DEBT})) {
            return parseResourceStatement();
        }

        if (match({TokenType::BRANCH_TIMELINE, TokenType::MERGE_TIMELINES, TokenType::REWIND_FLOW,
                   TokenType::REVERSE_FLOW})) {
            return parseTemporalStatement();
        }

        return parseExpressionStatement();
    } catch (const std::exception& e) {
        reporter_.addError(current_, std::string("Error in statement: ") + e.what());
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<StmtNode> Parser::parseExpressionStatement() {
    auto expr = parseExpression();

    if (!expr) {
        // If expression parsing failed, try to recover
        reporter_.addError(current_, "Invalid or missing expression");
        synchronize();
        return nullptr;
    }

    // Look for semicolon, but continue even if missing
    if (!check(TokenType::SEMICOLON)) {
        reporter_.addError(current_, "Expect ';' after expression");
    } else {
        advance();  // Consume the semicolon
    }

    return std::make_unique<ExprStmtNode>(std::move(expr));
}

std::unique_ptr<StmtNode> Parser::parseBlockStatement() {
    Token leftBrace = current_;
    std::vector<std::unique_ptr<StmtNode>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        } else {
            // Statement parsing failed, synchronize to next statement
            synchronize();
        }
    }

    if (!check(TokenType::RIGHT_BRACE)) {
        reporter_.addError(current_, "Expect '}' after block");
    } else {
        advance();  // Consume the right brace
    }

    return std::make_unique<BlockStmtNode>(std::move(leftBrace), std::move(statements));
}

std::unique_ptr<StmtNode> Parser::parseIfStatement() {
    Token ifToken = current_;

    // Parse condition in parentheses
    if (!check(TokenType::LEFT_PAREN)) {
        reporter_.addError(current_, "Expect '(' after 'if'");
        synchronize();
        return nullptr;
    }

    advance();  // Consume the left paren

    auto condition = parseExpression();
    if (!condition) {
        // If condition parsing failed, try to recover
        reporter_.addError(current_, "Invalid or missing condition in if statement");
        condition = std::make_unique<LiteralExprNode>(
            Token(TokenType::TRUE, "true", ifToken.getLocation()));
    }

    if (!check(TokenType::RIGHT_PAREN)) {
        reporter_.addError(current_, "Expect ')' after if condition");
        lexer.getDiagnosticReporter().addError(currentToken, "Expect ')' after if condition");
    } else {
        advance();  // Consume the right paren
    }

    // Parse the then branch
    auto thenBranch = parseStatement();
    if (!thenBranch) {
        // If then branch parsing failed, create an empty block
        lexer.getDiagnosticReporter().addError(currentToken,
                                               "Invalid or missing then branch in if statement");
        thenBranch = std::make_unique<BlockStmtNode>(
            Token(TokenType::LEFT_BRACE, "{", ifToken.getLocation()),
            std::vector<std::unique_ptr<StmtNode>>());
    }

    // Parse the else branch if present
    std::unique_ptr<StmtNode> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = parseStatement();
        if (!elseBranch) {
            // If else branch parsing failed, create an empty block
            lexer.getDiagnosticReporter().addError(
                currentToken, "Invalid or missing else branch in if statement");
            elseBranch = std::make_unique<BlockStmtNode>(
                Token(TokenType::LEFT_BRACE, "{", currentToken.getLocation()),
                std::vector<std::unique_ptr<StmtNode>>());
        }
    }

    return std::make_unique<IfStmtNode>(std::move(ifToken), std::move(condition),
                                        std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<StmtNode> Parser::parseWhileStatement() {
    Token whileToken = currentToken;

    // Parse condition in parentheses
    if (!check(TokenType::LEFT_PAREN)) {
        lexer.getDiagnosticReporter().addError(currentToken, "Expect '(' after 'while'");
        synchronize();
        return nullptr;
    }

    advance();  // Consume the left paren

    auto condition = parseExpression();
    if (!condition) {
        // If condition parsing failed, try to recover
        lexer.getDiagnosticReporter().addError(currentToken,
                                               "Invalid or missing condition in while statement");
        condition = std::make_unique<LiteralExprNode>(
            Token(TokenType::TRUE, "true", whileToken.getLocation()));
    }

    if (!check(TokenType::RIGHT_PAREN)) {
        lexer.getDiagnosticReporter().addError(currentToken, "Expect ')' after while condition");
    } else {
        advance();  // Consume the right paren
    }

    // Parse the body
    auto body = parseStatement();
    if (!body) {
        // If body parsing failed, create an empty block
        lexer.getDiagnosticReporter().addError(currentToken,
                                               "Invalid or missing body in while statement");
        body = std::make_unique<BlockStmtNode>(
            Token(TokenType::LEFT_BRACE, "{", whileToken.getLocation()),
            std::vector<std::unique_ptr<StmtNode>>());
    }

    return std::make_unique<WhileStmtNode>(std::move(whileToken), std::move(condition),
                                           std::move(body));
}

std::unique_ptr<StmtNode> Parser::parseResourceStatement() {
    Token keyword = currentToken;
    ResourceStmtNode::ResourceOperation operation;

    if (keyword.getType() == TokenType::ALLOCATE_RESOURCES) {
        operation = ResourceStmtNode::ResourceOperation::ALLOCATE;
    } else if (keyword.getType() == TokenType::EXPEND_RESOURCES) {
        operation = ResourceStmtNode::ResourceOperation::EXPEND;
    } else if (keyword.getType() == TokenType::BORROW_RESOURCES) {
        operation = ResourceStmtNode::ResourceOperation::BORROW;
    } else {
        operation = ResourceStmtNode::ResourceOperation::REPAY;
    }

    // Parse resource specifications
    if (!check(TokenType::LEFT_BRACE)) {
        lexer.getDiagnosticReporter().addError(currentToken, "Expect '{' after resource statement");
        synchronize();
        return nullptr;
    }

    advance();  // Consume the left brace

    // Parse resource specifications
    std::unique_ptr<ExprNode> chronons = nullptr;
    std::unique_ptr<ExprNode> aethel = nullptr;

    // Expect CHRONON: <value>
    if (match(TokenType::CHRONON)) {
        if (!check(TokenType::COLON)) {
            lexer.getDiagnosticReporter().addError(currentToken, "Expect ':' after 'CHRONON'");
        } else {
            advance();  // Consume the colon
            chronons = parseExpression();
            if (!chronons) {
                lexer.getDiagnosticReporter().addError(
                    currentToken, "Invalid or missing expression for CHRONON value");
            }
        }
    }

    // If there's a comma, expect AETHEL: <value>
    if (match(TokenType::COMMA)) {
        if (!check(TokenType::AETHEL)) {
            lexer.getDiagnosticReporter().addError(currentToken, "Expect 'AETHEL' after ','");
        } else {
            advance();  // Consume AETHEL

            if (!check(TokenType::COLON)) {
                lexer.getDiagnosticReporter().addError(currentToken, "Expect ':' after 'AETHEL'");
            } else {
                advance();  // Consume the colon
                aethel = parseExpression();
                if (!aethel) {
                    lexer.getDiagnosticReporter().addError(
                        currentToken, "Invalid or missing expression for AETHEL value");
                }
            }
        }
    }

    if (!check(TokenType::RIGHT_BRACE)) {
        lexer.getDiagnosticReporter().addError(currentToken,
                                               "Expect '}' after resource specifications");
    } else {
        advance();  // Consume the right brace
    }

    // Parse the body block
    if (!check(TokenType::LEFT_BRACE)) {
        lexer.getDiagnosticReporter().addError(currentToken,
                                               "Expect '{' after resource specifications");
        synchronize();

        // Create an empty block as a fallback
        auto emptyBody = std::make_unique<BlockStmtNode>(
            Token(TokenType::LEFT_BRACE, "{", currentToken.getLocation()),
            std::vector<std::unique_ptr<StmtNode>>());

        return std::make_unique<ResourceStmtNode>(std::move(keyword), operation,
                                                  std::move(chronons), std::move(aethel),
                                                  std::move(emptyBody));
    }

    auto body = parseBlockStatement();
    if (!body) {
        // If body parsing failed, create an empty block
        body = std::make_unique<BlockStmtNode>(
            Token(TokenType::LEFT_BRACE, "{", currentToken.getLocation()),
            std::vector<std::unique_ptr<StmtNode>>());
    }

    return std::make_unique<ResourceStmtNode>(std::move(keyword), operation, std::move(chronons),
                                              std::move(aethel), std::move(body));
}

std::unique_ptr<StmtNode> Parser::parseTemporalStatement() {
    // This would implement temporal statements like BRANCH_TIMELINE, MERGE_TIMELINES, etc.
    // For now, we'll return a placeholder
    return nullptr;
}

// Improved expression parsing without excessive try-catch blocks
std::unique_ptr<ExprNode> Parser::parseExpression() { return parseAssignment(); }

std::unique_ptr<ExprNode> Parser::parseAssignment() {
    auto expr = parseLogicalOr();

    if (!expr) {
        return nullptr;  // Error occurred in sub-expression
    }

    if (match(TokenType::EQUAL)) {
        auto value = parseAssignment();

        if (!value) {
            // Error in right-hand side of assignment
            lexer.getDiagnosticReporter().addError(
                currentToken, "Invalid or missing expression on right side of assignment");
            return expr;  // Return left side as fallback
        }

        if (auto* varExpr = dynamic_cast<VariableExprNode*>(expr.get())) {
            auto& name = varExpr->getName();
            return std::make_unique<AssignExprNode>(name, std::move(value));
        }

        lexer.getDiagnosticReporter().addError(
            currentToken, "Invalid assignment target. Left side must be a variable.");
    }

    return expr;
}

std::unique_ptr<ExprNode> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();

    while (match(TokenType::OR)) {
        Token op = currentToken;
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<ExprNode> Parser::parseLogicalAnd() {
    auto expr = parseEquality();

    while (match(TokenType::AND)) {
        Token op = currentToken;
        auto right = parseEquality();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<ExprNode> Parser::parseEquality() {
    auto expr = parseComparison();

    while (match(TokenType::BANG_EQUAL) || match(TokenType::EQUAL_EQUAL)) {
        Token op = currentToken;
        auto right = parseComparison();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<ExprNode> Parser::parseComparison() {
    auto expr = parseTerm();

    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS) ||
           match(TokenType::LESS_EQUAL)) {
        Token op = currentToken;
        auto right = parseTerm();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<ExprNode> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match(TokenType::MINUS) || match(TokenType::PLUS)) {
        Token op = currentToken;
        auto right = parseFactor();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<ExprNode> Parser::parseFactor() {
    auto expr = parseUnary();

    while (match(TokenType::SLASH) || match(TokenType::STAR) || match(TokenType::PERCENT)) {
        Token op = currentToken;
        auto right = parseUnary();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<ExprNode> Parser::parseUnary() {
    if (match(TokenType::BANG) || match(TokenType::MINUS)) {
        Token op = currentToken;
        auto right = parseUnary();
        return std::make_unique<UnaryExprNode>(std::move(op), std::move(right));
    }

    return parsePostfix();
}

std::unique_ptr<ExprNode> Parser::parsePostfix() {
    auto expr = parsePrimary();

    while (match(TokenType::PLUS_PLUS) || match(TokenType::MINUS_MINUS)) {
        // This would handle postfix operations like x++, x--
        // For now, we'll just return the base expression
    }

    return expr;
}

std::unique_ptr<ExprNode> Parser::parsePrimary() {
    if (match(TokenType::TRUE) || match(TokenType::FALSE) || match(TokenType::INTEGER) ||
        match(TokenType::FLOAT) || match(TokenType::STRING) || match(TokenType::CHRONON_LITERAL) ||
        match(TokenType::AETHEL_LITERAL)) {
        return std::make_unique<LiteralExprNode>(currentToken);
    }

    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExprNode>(currentToken);
    }

    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();

        if (!expr) {
            // Error in sub-expression, try to recover by synchronizing
            lexer.getDiagnosticReporter().addError(
                currentToken, "Invalid or missing expression within parentheses");
            synchronize();
            // Create a placeholder literal to continue parsing
            return std::make_unique<LiteralExprNode>(
                Token(TokenType::INTEGER, "0", currentToken.getLocation()));
        }

        if (!check(TokenType::RIGHT_PAREN)) {
            lexer.getDiagnosticReporter().addError(currentToken, "Expect ')' after expression");
            synchronize();
        } else {
            advance();  // Consume the right paren
        }

        return expr;
    }

    // Handle unexpected tokens gracefully
    lexer.getDiagnosticReporter().addError(currentToken, "Expected expression, got '" +
                                                             currentToken.getLexeme() + "'");

    // Create a placeholder literal to continue parsing
    auto errorToken = currentToken;
    synchronize();
    return std::make_unique<LiteralExprNode>(
        Token(TokenType::INTEGER, "0", errorToken.getLocation()));
}

std::unique_ptr<TypeNode> Parser::parseType() {
    // This would parse type annotations
    // For now, we'll return a placeholder
    return nullptr;
}

}  // namespace compiler
}  // namespace chronovyan