#ifndef CHRONOVYAN_STATEMENT_VISITOR_H
#define CHRONOVYAN_STATEMENT_VISITOR_H

#include <memory>
#include <stack>
#include <string>

#include "ast_nodes.h"
#include "value.h"

namespace chronovyan {

// Forward declarations
class Interpreter;
class Environment;
class TemporalRuntime;
class ResourceOptimizer;

/**
 * @class StatementVisitor
 * @brief Handles the execution of statements in the AST
 */
class StatementVisitor {
public:
    /**
     * @brief Create a new statement visitor
     * @param interpreter Reference to the parent interpreter
     */
    explicit StatementVisitor(Interpreter& interpreter);

    /**
     * @brief Execute a statement
     * @param stmt The statement to execute
     */
    void execute(const StmtNode& stmt);

    // Statement visitor methods
    void visitExprStmt(const ExprStmtNode& stmt);
    void visitBlockStmt(const BlockStmtNode& stmt);
    void visitVariableDeclStmt(const VariableDeclStmtNode& stmt);
    void visitIfStmt(const IfStmtNode& stmt);
    void visitFunctionDeclStmt(const FunctionDeclStmtNode& stmt);
    void visitReturnStmt(const ReturnStmtNode& stmt);
    void visitTypeDefStmt(const TypeDefStmtNode& stmt);

    // Control flow state accessors
    bool isReturning() const;
    bool isBreaking() const;
    bool isContinuing() const;
    Value getReturnValue() const;

    // Control flow state modifiers
    void clearControlFlowFlags();
    void setReturning(bool flag, const Value& value = Value());
    void setBreaking(bool flag);
    void setContinuing(bool flag);

private:
    Interpreter& m_interpreter;

    // Control flow state
    bool m_isReturning = false;
    bool m_isBreaking = false;
    bool m_isContinuing = false;
    Value m_returnValue;

    // Helper methods
    void executeBlock(const BlockStmtNode& block, std::shared_ptr<Environment> environment);
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_STATEMENT_VISITOR_H