#ifndef CHRONOVYAN_AST_PRINTER_H
#define CHRONOVYAN_AST_PRINTER_H

#include <iostream>
#include <string>

#include "compiler/AstVisitor.h"
#include "compiler/Token.h"

namespace chronovyan {
namespace compiler {

/**
 * AstPrinter is a visitor that outputs a textual representation of the AST.
 * This is useful for debugging and visualizing the structure of the AST.
 */
class AstPrinter : public AstVisitor {
public:
    /**
     * Constructs an AST printer that outputs to the given stream.
     *
     * @param os The output stream to write to
     */
    explicit AstPrinter(std::ostream& os = std::cout);

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
    std::ostream& os;
    int indentLevel;

    // Helper methods
    void indent();
    void printIndent();
    void printLine(const std::string& text);

    // Helper for printing literal values
    std::string literalToString(const Token::ValueType& value);
};

}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_AST_PRINTER_H