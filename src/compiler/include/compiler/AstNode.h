#ifndef CHRONOVYAN_AST_NODE_H
#define CHRONOVYAN_AST_NODE_H

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "compiler/AstVisitor.h"
#include "compiler/Token.h"

namespace chronovyan {
namespace compiler {

// Forward declarations
class ExprNode;
class StmtNode;
class DeclNode;
class TypeNode;

/**
 * AstNode is the base class for all nodes in the AST.
 */
class AstNode {
public:
    virtual ~AstNode() = default;

    /**
     * Returns the source location of the node.
     */
    virtual SourceLocation getLocation() const = 0;

    /**
     * Accepts a visitor.
     */
    virtual void accept(AstVisitor& visitor) = 0;
};

/**
 * ProgramNode represents the root of the AST.
 */
class ProgramNode : public AstNode {
public:
    ProgramNode(std::vector<std::unique_ptr<DeclNode>> declarations)
        : declarations(std::move(declarations)) {}

    SourceLocation getLocation() const override;
    void accept(AstVisitor& visitor) override;

    const std::vector<std::unique_ptr<DeclNode>>& getDeclarations() const { return declarations; }

private:
    std::vector<std::unique_ptr<DeclNode>> declarations;
};

/**
 * TypeNode is the base class for all type nodes.
 */
class TypeNode : public AstNode {
public:
    virtual ~TypeNode() = default;
};

/**
 * BasicTypeNode represents a basic type like int, float, etc.
 */
class BasicTypeNode : public TypeNode {
public:
    BasicTypeNode(Token token) : token(std::move(token)) {}

    SourceLocation getLocation() const override { return token.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const Token& getToken() const { return token; }

private:
    Token token;
};

/**
 * ExprNode is the base class for all expression nodes.
 */
class ExprNode : public AstNode {
public:
    virtual ~ExprNode() = default;
};

/**
 * LiteralExprNode represents a literal value like 42, "hello", etc.
 */
class LiteralExprNode : public ExprNode {
public:
    using ValueType = Token::ValueType;

    LiteralExprNode(Token token) : token(std::move(token)) {}

    SourceLocation getLocation() const override { return token.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const Token& getToken() const { return token; }
    Token& getToken() { return token; }

    const ValueType& getValue() const {
        if (!token.getValue().has_value()) {
            throw std::runtime_error("Token has no value");
        }
        return token.getValue().value();
    }

private:
    Token token;
};

/**
 * VariableExprNode represents a variable reference.
 */
class VariableExprNode : public ExprNode {
public:
    VariableExprNode(Token name) : name(std::move(name)) {}

    SourceLocation getLocation() const override { return name.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const Token& getName() const { return name; }
    Token& getName() { return name; }

private:
    Token name;
};

/**
 * UnaryExprNode represents a unary operation like -x, !x, etc.
 */
class UnaryExprNode : public ExprNode {
public:
    UnaryExprNode(Token op, std::unique_ptr<ExprNode> right)
        : op(std::move(op)), right(std::move(right)) {}

    SourceLocation getLocation() const override { return op.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const Token& getOperator() const { return op; }
    Token& getOperator() { return op; }

    const ExprNode& getRight() const { return *right; }
    ExprNode& getRight() { return *right; }

private:
    Token op;
    std::unique_ptr<ExprNode> right;
};

/**
 * BinaryExprNode represents a binary operation like x + y, x * y, etc.
 */
class BinaryExprNode : public ExprNode {
public:
    BinaryExprNode(std::unique_ptr<ExprNode> left, Token op, std::unique_ptr<ExprNode> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    SourceLocation getLocation() const override { return op.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const ExprNode& getLeft() const { return *left; }
    ExprNode& getLeft() { return *left; }

    const Token& getOperator() const { return op; }
    Token& getOperator() { return op; }

    const ExprNode& getRight() const { return *right; }
    ExprNode& getRight() { return *right; }

private:
    std::unique_ptr<ExprNode> left;
    Token op;
    std::unique_ptr<ExprNode> right;
};

/**
 * CallExprNode represents a function call.
 */
class CallExprNode : public ExprNode {
public:
    CallExprNode(std::unique_ptr<ExprNode> callee, Token paren,
                 std::vector<std::unique_ptr<ExprNode>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    SourceLocation getLocation() const override { return paren.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const ExprNode& getCallee() const { return *callee; }
    ExprNode& getCallee() { return *callee; }

    const std::vector<std::unique_ptr<ExprNode>>& getArguments() const { return arguments; }
    std::vector<std::unique_ptr<ExprNode>>& getArguments() { return arguments; }

private:
    std::unique_ptr<ExprNode> callee;
    Token paren;
    std::vector<std::unique_ptr<ExprNode>> arguments;
};

/**
 * AssignExprNode represents an assignment like x = 42.
 */
class AssignExprNode : public ExprNode {
public:
    AssignExprNode(Token name, std::unique_ptr<ExprNode> value)
        : name(std::move(name)), value(std::move(value)) {}

    SourceLocation getLocation() const override { return name.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const Token& getName() const { return name; }
    Token& getName() { return name; }

    const ExprNode& getValue() const { return *value; }
    ExprNode& getValue() { return *value; }

private:
    Token name;
    std::unique_ptr<ExprNode> value;
};

/**
 * StmtNode is the base class for all statement nodes.
 */
class StmtNode : public AstNode {
public:
    virtual ~StmtNode() = default;
};

/**
 * ExprStmtNode represents an expression statement.
 */
class ExprStmtNode : public StmtNode {
public:
    ExprStmtNode(std::unique_ptr<ExprNode> expression) : expression(std::move(expression)) {}

    SourceLocation getLocation() const override { return expression->getLocation(); }
    void accept(AstVisitor& visitor) override;

    const ExprNode& getExpression() const { return *expression; }
    ExprNode& getExpression() { return *expression; }

private:
    std::unique_ptr<ExprNode> expression;
};

/**
 * BlockStmtNode represents a block of statements.
 */
class BlockStmtNode : public StmtNode {
public:
    BlockStmtNode(Token leftBrace, std::vector<std::unique_ptr<StmtNode>> statements)
        : leftBrace(std::move(leftBrace)), statements(std::move(statements)) {}

    SourceLocation getLocation() const override { return leftBrace.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const std::vector<std::unique_ptr<StmtNode>>& getStatements() const { return statements; }
    std::vector<std::unique_ptr<StmtNode>>& getStatements() { return statements; }

private:
    Token leftBrace;
    std::vector<std::unique_ptr<StmtNode>> statements;
};

/**
 * IfStmtNode represents an if statement.
 */
class IfStmtNode : public StmtNode {
public:
    IfStmtNode(Token ifToken, std::unique_ptr<ExprNode> condition,
               std::unique_ptr<StmtNode> thenBranch, std::unique_ptr<StmtNode> elseBranch)
        : ifToken(std::move(ifToken))
        , condition(std::move(condition))
        , thenBranch(std::move(thenBranch))
        , elseBranch(std::move(elseBranch)) {}

    SourceLocation getLocation() const override { return ifToken.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const ExprNode& getCondition() const { return *condition; }
    ExprNode& getCondition() { return *condition; }

    const StmtNode& getThenBranch() const { return *thenBranch; }
    StmtNode& getThenBranch() { return *thenBranch; }

    const StmtNode* getElseBranch() const { return elseBranch.get(); }
    StmtNode* getElseBranch() { return elseBranch.get(); }

private:
    Token ifToken;
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StmtNode> thenBranch;
    std::unique_ptr<StmtNode> elseBranch;
};

/**
 * WhileStmtNode represents a while statement.
 */
class WhileStmtNode : public StmtNode {
public:
    WhileStmtNode(Token whileToken, std::unique_ptr<ExprNode> condition,
                  std::unique_ptr<StmtNode> body)
        : whileToken(std::move(whileToken))
        , condition(std::move(condition))
        , body(std::move(body)) {}

    SourceLocation getLocation() const override { return whileToken.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const ExprNode& getCondition() const { return *condition; }
    ExprNode& getCondition() { return *condition; }

    const StmtNode& getBody() const { return *body; }
    StmtNode& getBody() { return *body; }

private:
    Token whileToken;
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StmtNode> body;
};

/**
 * DeclNode is the base class for all declaration nodes.
 */
class DeclNode : public AstNode {
public:
    virtual ~DeclNode() = default;
};

/**
 * VarDeclNode represents a variable declaration.
 */
class VarDeclNode : public DeclNode {
public:
    VarDeclNode(Token name, std::unique_ptr<TypeNode> type, std::unique_ptr<ExprNode> initializer)
        : name(std::move(name)), type(std::move(type)), initializer(std::move(initializer)) {}

    SourceLocation getLocation() const override { return name.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const Token& getName() const { return name; }
    Token& getName() { return name; }

    const TypeNode* getType() const { return type.get(); }
    TypeNode* getType() { return type.get(); }

    const ExprNode* getInitializer() const { return initializer.get(); }
    ExprNode* getInitializer() { return initializer.get(); }

private:
    Token name;
    std::unique_ptr<TypeNode> type;
    std::unique_ptr<ExprNode> initializer;
};

/**
 * ResourceStmtNode represents a resource allocation or expenditure statement.
 */
class ResourceStmtNode : public StmtNode {
public:
    enum class ResourceOperation { ALLOCATE, EXPEND, BORROW, REPAY };

    ResourceStmtNode(Token keyword, ResourceOperation operation, std::unique_ptr<ExprNode> chronons,
                     std::unique_ptr<ExprNode> aethel, std::unique_ptr<StmtNode> body)
        : keyword(std::move(keyword))
        , operation(operation)
        , chronons(std::move(chronons))
        , aethel(std::move(aethel))
        , body(std::move(body)) {}

    SourceLocation getLocation() const override { return keyword.getLocation(); }
    void accept(AstVisitor& visitor) override;

    ResourceOperation getOperation() const { return operation; }

    const ExprNode* getChronons() const { return chronons.get(); }
    ExprNode* getChronons() { return chronons.get(); }

    const ExprNode* getAethel() const { return aethel.get(); }
    ExprNode* getAethel() { return aethel.get(); }

    const StmtNode& getBody() const { return *body; }
    StmtNode& getBody() { return *body; }

private:
    Token keyword;
    ResourceOperation operation;
    std::unique_ptr<ExprNode> chronons;
    std::unique_ptr<ExprNode> aethel;
    std::unique_ptr<StmtNode> body;
};

/**
 * StmtDeclNode is a wrapper for statements that need to be treated as declarations.
 * This is used for handling statement expressions at the top level of a program.
 */
class StmtDeclNode : public DeclNode {
public:
    StmtDeclNode(std::unique_ptr<StmtNode> stmt) : stmt(std::move(stmt)) {}

    SourceLocation getLocation() const override { return stmt->getLocation(); }
    void accept(AstVisitor& visitor) override;

    const StmtNode& getStmt() const { return *stmt; }
    StmtNode& getStmt() { return *stmt; }

private:
    std::unique_ptr<StmtNode> stmt;
};

/**
 * ParallelExecutionStmtNode represents a parallel execution statement.
 */
class ParallelExecutionStmtNode : public StmtNode {
public:
    ParallelExecutionStmtNode(Token keyword, std::unique_ptr<ExprNode> threadCount,
                              std::unique_ptr<StmtNode> body)
        : keyword(std::move(keyword)), threadCount(std::move(threadCount)), body(std::move(body)) {}

    SourceLocation getLocation() const override { return keyword.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const ExprNode& getThreadCount() const { return *threadCount; }
    ExprNode& getThreadCount() { return *threadCount; }

    const StmtNode& getBody() const { return *body; }
    StmtNode& getBody() { return *body; }

private:
    Token keyword;
    std::unique_ptr<ExprNode> threadCount;
    std::unique_ptr<StmtNode> body;
};

/**
 * BranchTimelineStmtNode represents a branch timeline statement.
 */
class BranchTimelineStmtNode : public StmtNode {
public:
    BranchTimelineStmtNode(Token keyword, std::unique_ptr<ExprNode> branchCount,
                           std::unique_ptr<StmtNode> body)
        : keyword(std::move(keyword)), branchCount(std::move(branchCount)), body(std::move(body)) {}

    SourceLocation getLocation() const override { return keyword.getLocation(); }
    void accept(AstVisitor& visitor) override;

    const ExprNode& getBranchCount() const { return *branchCount; }
    ExprNode& getBranchCount() { return *branchCount; }

    const StmtNode& getBody() const { return *body; }
    StmtNode& getBody() { return *body; }

private:
    Token keyword;
    std::unique_ptr<ExprNode> branchCount;
    std::unique_ptr<StmtNode> body;
};

}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_AST_NODE_H