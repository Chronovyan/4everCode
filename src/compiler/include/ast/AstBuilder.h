#ifndef CHRONOVYAN_COMPILER_AST_BUILDER_H
#define CHRONOVYAN_COMPILER_AST_BUILDER_H

#include <memory>
#include <string>
#include <vector>

#include "compiler/DiagnosticReporter.h"

// Forward declarations
namespace chronovyan {
namespace compiler {
class AstNode;
class ExpressionNode;
class StatementNode;
class ProgramNode;
class Token;
}  // namespace compiler
}  // namespace chronovyan

namespace chronovyan {
namespace compiler {

/**
 * @class AstBuilder
 * @brief Utility class to build AST nodes programmatically
 *
 * This class provides utility methods to construct Abstract Syntax Tree (AST)
 * nodes programmatically. It is primarily used for testing and tools that
 * need to generate AST nodes without going through the parser.
 */
class AstBuilder {
public:
    /**
     * @brief Construct an AstBuilder
     */
    AstBuilder();

    /**
     * @brief Get the diagnostic reporter
     * @return Reference to the diagnostic reporter
     */
    DiagnosticReporter& getDiagnosticReporter();

    /**
     * @brief Create a literal expression node
     * @param value The literal value
     * @return A unique_ptr to the created expression node
     */
    std::unique_ptr<ExpressionNode> createLiteral(const std::string& value);

    /**
     * @brief Create a variable expression node
     * @param name The variable name
     * @return A unique_ptr to the created expression node
     */
    std::unique_ptr<ExpressionNode> createVariable(const std::string& name);

    /**
     * @brief Create a binary expression node
     * @param left The left operand
     * @param op The operator
     * @param right The right operand
     * @return A unique_ptr to the created expression node
     */
    std::unique_ptr<ExpressionNode> createBinaryExpr(std::unique_ptr<ExpressionNode> left,
                                                     const std::string& op,
                                                     std::unique_ptr<ExpressionNode> right);

    /**
     * @brief Create a function call expression node
     * @param callee The function being called
     * @param arguments The arguments to the function
     * @return A unique_ptr to the created expression node
     */
    std::unique_ptr<ExpressionNode> createCall(
        std::unique_ptr<ExpressionNode> callee,
        std::vector<std::unique_ptr<ExpressionNode>> arguments);

    /**
     * @brief Create a variable declaration statement
     * @param name The variable name
     * @param type The variable type
     * @param initializer The initial value (optional)
     * @return A unique_ptr to the created statement node
     */
    std::unique_ptr<StatementNode> createVariableDecl(
        const std::string& name, const std::string& type,
        std::unique_ptr<ExpressionNode> initializer = nullptr);

    /**
     * @brief Create a block statement
     * @param statements The statements in the block
     * @return A unique_ptr to the created statement node
     */
    std::unique_ptr<StatementNode> createBlock(
        std::vector<std::unique_ptr<StatementNode>> statements);

    /**
     * @brief Create an if statement
     * @param condition The condition expression
     * @param thenBranch The then branch
     * @param elseBranch The else branch (optional)
     * @return A unique_ptr to the created statement node
     */
    std::unique_ptr<StatementNode> createIf(std::unique_ptr<ExpressionNode> condition,
                                            std::unique_ptr<StatementNode> thenBranch,
                                            std::unique_ptr<StatementNode> elseBranch = nullptr);

    /**
     * @brief Create a for loop statement
     * @param initializer The initializer statement
     * @param condition The loop condition
     * @param increment The increment expression
     * @param body The loop body
     * @return A unique_ptr to the created statement node
     */
    std::unique_ptr<StatementNode> createFor(std::unique_ptr<StatementNode> initializer,
                                             std::unique_ptr<ExpressionNode> condition,
                                             std::unique_ptr<ExpressionNode> increment,
                                             std::unique_ptr<StatementNode> body);

    /**
     * @brief Create a while loop statement
     * @param condition The loop condition
     * @param body The loop body
     * @return A unique_ptr to the created statement node
     */
    std::unique_ptr<StatementNode> createWhile(std::unique_ptr<ExpressionNode> condition,
                                               std::unique_ptr<StatementNode> body);

    /**
     * @brief Create a function declaration
     * @param name The function name
     * @param params The parameter names
     * @param returnType The return type
     * @param body The function body
     * @return A unique_ptr to the created statement node
     */
    std::unique_ptr<StatementNode> createFunctionDecl(const std::string& name,
                                                      const std::vector<std::string>& params,
                                                      const std::string& returnType,
                                                      std::unique_ptr<StatementNode> body);

    /**
     * @brief Create a program node
     * @param statements The top-level statements
     * @return A unique_ptr to the created program node
     */
    std::unique_ptr<ProgramNode> createProgram(
        std::vector<std::unique_ptr<StatementNode>> statements);

private:
    DiagnosticReporter m_diagnostics;
};

}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_COMPILER_AST_BUILDER_H