#ifndef CHRONOVYAN_CODE_GENERATOR_H
#define CHRONOVYAN_CODE_GENERATOR_H

#include <sstream>
#include <string>
#include <unordered_map>

#include "compiler/AstNode.h"
#include "compiler/AstVisitor.h"
#include "compiler/SymbolTable.h"
#include "compiler/TypeChecker.h"

namespace chronovyan {
namespace compiler {

/**
 * CodeGenerator is responsible for generating bytecode or target language code from the AST.
 * This simplified version generates C-like code for demonstration purposes.
 */
class CodeGenerator : public AstVisitor {
public:
    /**
     * Constructs a code generator.
     */
    CodeGenerator();

    /**
     * Generates code from the AST starting from the program node.
     *
     * @param programNode The root node of the AST
     * @return The generated code
     */
    std::string generate(ProgramNode* programNode);

    // Visit methods for each node type
    void visitProgramNode(ProgramNode* node) override;
    void visitBasicTypeNode(BasicTypeNode* node) override;
    void visitLiteralExprNode(LiteralExprNode* node) override;
    void visitVariableExprNode(VariableExprNode* node) override;
    void visitUnaryExprNode(UnaryExprNode* node) override;
    void visitBinaryExprNode(BinaryExprNode* node) override;
    void visitCallExprNode(CallExprNode* node) override;
    void visitAssignExprNode(AssignExprNode* node) override;
    void visitExprStmtNode(ExprStmtNode* node) override;
    void visitBlockStmtNode(BlockStmtNode* node) override;
    void visitIfStmtNode(IfStmtNode* node) override;
    void visitWhileStmtNode(WhileStmtNode* node) override;
    void visitVarDeclNode(VarDeclNode* node) override;
    void visitResourceStmtNode(ResourceStmtNode* node) override;
    void visitStmtDeclNode(StmtDeclNode* node) override;
    void visitParallelExecutionStmtNode(ParallelExecutionStmtNode* node) override;
    void visitBranchTimelineStmtNode(BranchTimelineStmtNode* node) override;

private:
    std::ostringstream output;
    int indentLevel;

    // Symbol table for tracking variables
    SymbolTable symbolTable;

    // Mapping of expressions to their generated code
    std::unordered_map<const ExprNode*, std::string> exprCode;

    // Helper methods
    void indent();
    void dedent();
    void printIndent();
    void printLine(const std::string& line);
    std::string getNodeCode(const ExprNode* node);
    void setNodeCode(const ExprNode* node, const std::string& code);
    std::string getCTypeForBasicType(const std::string& chronovyanType);
};

}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_CODE_GENERATOR_H