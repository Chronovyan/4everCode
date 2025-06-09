#include <stdexcept>

#include "ast_nodes.h"

namespace chronovyan {

// ASTNode implementation
ASTNode::ASTNode(const SourceLocation &location) : m_location(location) {}

const SourceLocation &ASTNode::getLocation() const { return m_location; }

// LiteralExprNode
void LiteralExprNode::accept(ASTVisitor &visitor) const {
  visitor.visitLiteralExpr(*this);
}

// VariableExprNode

void VariableExprNode::accept(ASTVisitor &visitor) const {
  visitor.visitVariableExpr(*this);
}

// UnaryExprNode

UnaryExprNode::UnaryExprNode(Token op, std::unique_ptr<ExprNode> right)
    : ExprNode(op.location), m_operator(std::move(op)),
      m_right(std::move(right)) {}

void UnaryExprNode::accept(ASTVisitor &visitor) const {
  visitor.visitUnaryExpr(*this);
}

// BinaryExprNode

BinaryExprNode::BinaryExprNode(std::unique_ptr<ExprNode> left, Token op,
                               std::unique_ptr<ExprNode> right)
    : ExprNode(op.location), m_left(std::move(left)), m_operator(std::move(op)),
      m_right(std::move(right)) {}

void BinaryExprNode::accept(ASTVisitor &visitor) const {
  visitor.visitBinaryExpr(*this);
}

// GroupingExprNode

GroupingExprNode::GroupingExprNode(std::unique_ptr<ExprNode> expression)
    : ExprNode(expression ? expression->getLocation() : SourceLocation()),
      m_expression(std::move(expression)) {}

void GroupingExprNode::accept(ASTVisitor &visitor) const {
  visitor.visitGroupingExpr(*this);
}

// AssignExprNode

void AssignExprNode::accept(ASTVisitor &visitor) const {
  visitor.visitAssignExpr(*this);
}

// CallExprNode

CallExprNode::CallExprNode(std::unique_ptr<ExprNode> callee,
                           std::vector<std::unique_ptr<ExprNode>> arguments)
    : ExprNode(callee ? callee->getLocation() : SourceLocation()),
      m_callee(std::move(callee)), m_arguments(std::move(arguments)) {}

void CallExprNode::accept(ASTVisitor &visitor) const {
  visitor.visitCallExpr(*this);
}

// ExprStmtNode

ExprStmtNode::ExprStmtNode(std::unique_ptr<ExprNode> expression)
    : StmtNode(expression ? expression->getLocation() : SourceLocation()),
      m_expression(std::move(expression)) {}

void ExprStmtNode::accept(ASTVisitor &visitor) const {
  visitor.visitExprStmt(*this);
}

// BlockStmtNode

BlockStmtNode::BlockStmtNode(std::vector<std::unique_ptr<StmtNode>> statements)
    : StmtNode(statements.empty() ? SourceLocation()
                                  : statements[0]->getLocation()),
      m_statements(std::move(statements)) {}

void BlockStmtNode::accept(ASTVisitor &visitor) const {
  visitor.visitBlockStmt(*this);
}

// VariableDeclStmtNode

void VariableDeclStmtNode::accept(ASTVisitor &visitor) const {
  visitor.visitVariableDeclStmt(*this);
}

const ExprNode &VariableDeclStmtNode::getInitializer() const {
  if (!m_initializer)
    throw std::runtime_error("No initializer for variable");
  return *m_initializer;
}

ExprNode &VariableDeclStmtNode::getInitializer() {
  if (!m_initializer)
    throw std::runtime_error("No initializer for variable");
  return *m_initializer;
}

// IfStmtNode

void IfStmtNode::accept(ASTVisitor &visitor) const {
  visitor.visitIfStmt(*this);
}

const StmtNode &IfStmtNode::getElseBranch() const {
  if (!m_elseBranch)
    throw std::runtime_error("No else branch");
  return *m_elseBranch;
}

StmtNode &IfStmtNode::getElseBranch() {
  if (!m_elseBranch)
    throw std::runtime_error("No else branch");
  return *m_elseBranch;
}

// TemporalOpStmtNode

void TemporalOpStmtNode::accept(ASTVisitor &visitor) const {
  visitor.visitTemporalOpStmt(*this);
}

// ProgramNode

ProgramNode::ProgramNode(std::vector<std::unique_ptr<StmtNode>> statements)
    : ASTNode(SourceLocation()), m_statements(std::move(statements)) {}

void ProgramNode::accept(ASTVisitor &visitor) const {
  visitor.visitProgram(*this);
}

// TypeDefStmtNode implementation
TypeDefStmtNode::TypeDefStmtNode(const SourceLocation &location,
                                 const std::string &name,
                                 const std::vector<StructField> &fields,
                                 const std::vector<std::string> &typeParameters)
    : StmtNode(location), m_name(name), m_fields(fields),
      m_typeParameters(typeParameters), m_kind(TypeDefKind::STRUCT) {}

TypeDefStmtNode::TypeDefStmtNode(const SourceLocation &location,
                                 const std::string &name,
                                 const std::vector<EnumVariant> &variants,
                                 const std::vector<std::string> &typeParameters)
    : StmtNode(location), m_name(name), m_variants(variants),
      m_typeParameters(typeParameters), m_kind(TypeDefKind::ENUM) {}

TypeDefStmtNode::TypeDefStmtNode(
    const SourceLocation &location, const std::string &name,
    const std::vector<std::shared_ptr<TypeNode>> &unionTypes,
    const std::vector<std::string> &typeParameters)
    : StmtNode(location), m_name(name), m_unionTypes(unionTypes),
      m_typeParameters(typeParameters), m_kind(TypeDefKind::UNION) {}

TypeDefStmtNode::TypeDefStmtNode(const SourceLocation &location,
                                 const std::string &name,
                                 std::shared_ptr<TypeNode> aliasedType,
                                 const std::vector<std::string> &typeParameters)
    : StmtNode(location), m_name(name), m_aliasedType(aliasedType),
      m_typeParameters(typeParameters), m_kind(TypeDefKind::ALIAS) {}

void TypeDefStmtNode::accept(ASTVisitor &visitor) const {
  visitor.visitTypeDefStmt(*this);
}

const std::string &TypeDefStmtNode::getName() const { return m_name; }

const std::vector<StructField> &TypeDefStmtNode::getFields() const {
  return m_fields;
}

const std::vector<EnumVariant> &TypeDefStmtNode::getVariants() const {
  return m_variants;
}

const std::vector<std::shared_ptr<TypeNode>> &
TypeDefStmtNode::getUnionTypes() const {
  return m_unionTypes;
}

std::shared_ptr<TypeNode> TypeDefStmtNode::getAliasedType() const {
  return m_aliasedType;
}

const std::vector<std::string> &TypeDefStmtNode::getTypeParameters() const {
  return m_typeParameters;
}

bool TypeDefStmtNode::isStruct() const { return m_kind == TypeDefKind::STRUCT; }

bool TypeDefStmtNode::isEnum() const { return m_kind == TypeDefKind::ENUM; }

bool TypeDefStmtNode::isUnion() const { return m_kind == TypeDefKind::UNION; }

bool TypeDefStmtNode::isAlias() const { return m_kind == TypeDefKind::ALIAS; }

bool TypeDefStmtNode::isGeneric() const { return !m_typeParameters.empty(); }

} // namespace chronovyan