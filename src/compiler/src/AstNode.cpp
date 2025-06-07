#include "compiler/AstNode.h"

namespace chronovyan {
namespace compiler {

// Implementation of ProgramNode methods
SourceLocation ProgramNode::getLocation() const {
    if (declarations.empty()) {
        return SourceLocation("", 0, 0);
    }
    return declarations.front()->getLocation();
}

void ProgramNode::accept(AstVisitor& visitor) { visitor.visitProgramNode(this); }

// Implementation of BasicTypeNode methods
void BasicTypeNode::accept(AstVisitor& visitor) { visitor.visitBasicTypeNode(this); }

// Implementation of LiteralExprNode methods
void LiteralExprNode::accept(AstVisitor& visitor) { visitor.visitLiteralExprNode(this); }

// Implementation of VariableExprNode methods
void VariableExprNode::accept(AstVisitor& visitor) { visitor.visitVariableExprNode(this); }

// Implementation of UnaryExprNode methods
void UnaryExprNode::accept(AstVisitor& visitor) { visitor.visitUnaryExprNode(this); }

// Implementation of BinaryExprNode methods
void BinaryExprNode::accept(AstVisitor& visitor) { visitor.visitBinaryExprNode(this); }

// Implementation of CallExprNode methods
void CallExprNode::accept(AstVisitor& visitor) { visitor.visitCallExprNode(this); }

// Implementation of AssignExprNode methods
void AssignExprNode::accept(AstVisitor& visitor) { visitor.visitAssignExprNode(this); }

// Implementation of ExprStmtNode methods
void ExprStmtNode::accept(AstVisitor& visitor) { visitor.visitExprStmtNode(this); }

// Implementation of BlockStmtNode methods
void BlockStmtNode::accept(AstVisitor& visitor) { visitor.visitBlockStmtNode(this); }

// Implementation of IfStmtNode methods
void IfStmtNode::accept(AstVisitor& visitor) { visitor.visitIfStmtNode(this); }

// Implementation of WhileStmtNode methods
void WhileStmtNode::accept(AstVisitor& visitor) { visitor.visitWhileStmtNode(this); }

// Implementation of VarDeclNode methods
void VarDeclNode::accept(AstVisitor& visitor) { visitor.visitVarDeclNode(this); }

// Implementation of ResourceStmtNode methods
void ResourceStmtNode::accept(AstVisitor& visitor) { visitor.visitResourceStmtNode(this); }

// Implementation of StmtDeclNode methods
void StmtDeclNode::accept(AstVisitor& visitor) { visitor.visitStmtDeclNode(this); }

}  // namespace compiler
}  // namespace chronovyan