#include <iostream>
#include <memory>
#include <string>

// A simplified version of the visitor pattern for testing

// Forward declarations
class AstNode;
class ExprNode;
class LiteralExprNode;

// Visitor interface
class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(LiteralExprNode& node) = 0;
};

// Base AST node
class AstNode {
public:
    virtual ~AstNode() = default;
    virtual void accept(AstVisitor& visitor) = 0;
};

// Base expression node
class ExprNode : public AstNode {
public:
    virtual ~ExprNode() = default;
};

// Literal expression node
class LiteralExprNode : public ExprNode {
public:
    LiteralExprNode(int value) : value(value) {}

    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    int getValue() const { return value; }

private:
    int value;
};

// Concrete visitor
class PrintVisitor : public AstVisitor {
public:
    void visit(LiteralExprNode& node) override {
        std::cout << "Literal: " << node.getValue() << std::endl;
    }
};

int main() {
    // Create a simple AST
    auto expr = std::make_unique<LiteralExprNode>(42);

    // Create a visitor
    PrintVisitor visitor;

    // Accept the visitor
    expr->accept(visitor);

    return 0;
}