#ifndef CHRONOVYAN_AST_NODES_H
#define CHRONOVYAN_AST_NODES_H

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "enum_variant.h"
#include "error.h"
#include "literal_value.h"
#include "source_location.h"
#include "struct_field.h"
#include "token.h"
#include "variable_flag.h"
#include "variable_modifier.h"

namespace chronovyan {

// Forward declaration for Value
class Value;

// Forward declarations for all node types
class ASTNode;
class ExprNode;
class StmtNode;
class ProgramNode;
class TypeNode;
class LiteralExprNode;
class VariableExprNode;
class UnaryExprNode;
class BinaryExprNode;
class GroupingExprNode;
class AssignExprNode;
class CallExprNode;
class ExprStmtNode;
class BlockStmtNode;
class VariableDeclStmtNode;
class IfStmtNode;
class TemporalOpStmtNode;
class FunctionDeclStmtNode;
class TypeDefStmtNode;
class ReturnStmtNode;
class TemporalProgramNode;               // New node for temporal_program
class TemporalSectionNode;               // New node for temporal_program sections
class DeclareConstantNode;               // New node for DECLARE CONF::FLAG statements
class ForChrononStmtNode;                // New node for FOR_CHRONON loops
class WhileEventStmtNode;                // New node for WHILE_EVENT loops
class RewindFlowStmtNode;                // New node for REWIND_FLOW loops
class TemporalLoopStmtNode;              // New node for TEMPORAL_LOOP construct
class ParallelExecutionStmtNode;         // New node for PARALLEL_EXECUTION construct
class BranchTimelineStmtNode;            // New node for BRANCH_TIMELINE construct
class AttemptWeaveStmtNode;              // New node for ATTEMPT_WEAVE error handling
class DissonanceHandlerNode;             // New node for ON_DISSONANCE handler
class BreakChrononStmtNode;              // New node for BREAK_CHRONON statement
class ContinueWeaveStmtNode;             // New node for CONTINUE_WEAVE statement
class ReverseFlowStmtNode;               // New node for REVERSE_FLOW statement
class ExpendResourcesStmtNode;           // New node for EXPEND_RESOURCES block
class ResourceInsufficiencyHandlerNode;  // New node for ON_INSUFFICIENT_RESOURCES handler
class HaltRewindStmtNode;                // New node for HALT_REWIND statement
class StabilizeTimelineStmtNode;         // New node for STABILIZE_TIMELINE statement
class PreventModificationStmtNode;       // New node for PREVENT_MODIFICATION statement

/**
 * @class ASTVisitor
 * @brief Visitor interface for AST nodes
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    // Expressions
    virtual void visitLiteralExpr(const LiteralExprNode& node) = 0;
    virtual void visitVariableExpr(const VariableExprNode& node) = 0;
    virtual void visitUnaryExpr(const UnaryExprNode& node) = 0;
    virtual void visitBinaryExpr(const BinaryExprNode& node) = 0;
    virtual void visitGroupingExpr(const GroupingExprNode& node) = 0;
    virtual void visitAssignExpr(const AssignExprNode& node) = 0;
    virtual void visitCallExpr(const CallExprNode& node) = 0;

    // Statements
    virtual void visitExprStmt(const ExprStmtNode& node) = 0;
    virtual void visitBlockStmt(const BlockStmtNode& node) = 0;
    virtual void visitVariableDeclStmt(const VariableDeclStmtNode& node) = 0;
    virtual void visitIfStmt(const IfStmtNode& node) = 0;
    virtual void visitTemporalOpStmt(const TemporalOpStmtNode& node) = 0;
    virtual void visitFunctionDeclStmt(const FunctionDeclStmtNode& node) = 0;
    virtual void visitTypeDefStmt(const TypeDefStmtNode& node) = 0;
    virtual void visitReturnStmt(const ReturnStmtNode& node) = 0;

    virtual void visitForChrononStmt(const ForChrononStmtNode& node) = 0;
    virtual void visitWhileEventStmt(const WhileEventStmtNode& node) = 0;
    virtual void visitRewindFlowStmt(const RewindFlowStmtNode& node) = 0;
    virtual void visitReverseFlowStmt(const ReverseFlowStmtNode& node) = 0;
    virtual void visitTemporalLoopStmt(const TemporalLoopStmtNode& node) = 0;
    virtual void visitParallelExecutionStmt(const ParallelExecutionStmtNode& node) = 0;
    virtual void visitBranchTimelineStmt(const BranchTimelineStmtNode& node) = 0;

    // REWIND_FLOW control mechanisms
    virtual void visitHaltRewindStmt(const HaltRewindStmtNode& node) = 0;
    virtual void visitStabilizeTimelineStmt(const StabilizeTimelineStmtNode& node) = 0;
    virtual void visitPreventModificationStmt(const PreventModificationStmtNode& node) = 0;

    // Other statement types - provide default implementations
    virtual void visitTemporalProgram(const TemporalProgramNode& node [[maybe_unused]]) {
        // Default empty implementation
    }

    virtual void visitTemporalSection(const TemporalSectionNode& node [[maybe_unused]]) {
        // Default empty implementation
    }

    virtual void visitDeclareConstant(const DeclareConstantNode& node [[maybe_unused]]) {
        // Default empty implementation
    }

    // Error handling - provide default implementations
    virtual void visitAttemptWeaveStmt(const AttemptWeaveStmtNode& node [[maybe_unused]]) {
        // Default empty implementation
    }

    virtual void visitDissonanceHandler(const DissonanceHandlerNode& node [[maybe_unused]]) {
        // Default empty implementation
    }

    // Temporal control flow - provide default implementations
    virtual void visitBreakChrononStmt(const BreakChrononStmtNode& node [[maybe_unused]]) {
        // Default empty implementation
    }

    virtual void visitContinueWeaveStmt(const ContinueWeaveStmtNode& node [[maybe_unused]]) {
        // Default empty implementation
    }

    // Resource management - provide default implementations
    virtual void visitExpendResourcesStmt(const ExpendResourcesStmtNode& node [[maybe_unused]]) {
        // Default empty implementation
    }

    virtual void visitResourceInsufficiencyHandler(const ResourceInsufficiencyHandlerNode& node
                                                   [[maybe_unused]]) {
        // Default empty implementation
    }

    // Other nodes
    virtual void visitType(const TypeNode& node) = 0;
    virtual void visitProgram(const ProgramNode& node) = 0;
};

/**
 * @enum TemporalOpType
 * @brief Defines the types of temporal operations
 */
enum class TemporalOpType {
    FOR_CHRONON,        // Temporal loop over chronons
    WHILE_EVENT,        // Loop until a timeline event occurs
    REWIND_FLOW,        // Rewind execution to a previous state
    BRANCH_TIMELINE,    // Create a new timeline branch
    MERGE_TIMELINES,    // Merge multiple timeline branches
    TEMPORAL_ECHO_LOOP  // Loop with access to previous iterations
};

/**
 * @class ASTNode
 * @brief Base class for all AST nodes
 */
class ASTNode {
public:
    explicit ASTNode(const SourceLocation& location);
    virtual ~ASTNode() = default;

    /**
     * @brief Get the location of the node in the source code
     * @return The source location
     */
    const SourceLocation& getLocation() const;

    /**
     * @brief Accept a visitor
     * @param visitor The visitor to accept
     */
    virtual void accept(ASTVisitor& visitor) const = 0;

private:
    SourceLocation m_location;
};

// Expression nodes

/**
 * @class ExprNode
 * @brief Base class for all expression nodes
 */
class ExprNode : public ASTNode {
public:
    explicit ExprNode(const SourceLocation& location) : ASTNode(location) {}
    virtual ~ExprNode() = default;

    /**
     * @brief Check if this is a literal expression
     * @return True if this is a literal expression, false otherwise
     */
    virtual bool isLiteral() const { return false; }

    // Virtual clone method for deep copying expressions
    virtual std::unique_ptr<ExprNode> clone() const = 0;
};

/**
 * @class LiteralExprNode
 * @brief Node for literal expressions (e.g., 42, "hello", true)
 */
class LiteralExprNode : public ExprNode {
public:
    /**
     * @brief Variant to hold different literal types
     */
    using LiteralValue = std::variant<int64_t, double, std::string, bool>;

    /**
     * @brief Construct a literal expression
     * @param location The source location
     * @param value The literal value
     */
    LiteralExprNode(const SourceLocation& location, LiteralValue value)
        : ExprNode(location), m_value(std::move(value)) {}

    /**
     * @brief Backward-compatible constructor (uses a default source location)
     * @param value The literal value
     */
    explicit LiteralExprNode(LiteralValue value)
        : LiteralExprNode(SourceLocation(), std::move(value)) {}

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the literal value
     */
    const LiteralValue& getValue() const { return m_value; }

    /**
     * @brief Check if this is a literal expression
     */
    bool isLiteral() const override { return true; }

    // Implement clone for deep copying
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<LiteralExprNode>(getLocation(), m_value);
    }

private:
    LiteralValue m_value;
};

/**
 * @class VariableExprNode
 * @brief Node for variable expressions (e.g., a, myVar)
 */
class VariableExprNode : public ExprNode {
public:
    /**
     * @brief Construct a variable expression
     * @param location The source location
     * @param name The variable name
     */
    VariableExprNode(const SourceLocation& location, std::string name)
        : ExprNode(location), m_name(std::move(name)) {}

    /**
     * @brief Backward-compatible constructor (uses a default source location)
     * @param name The variable name
     */
    explicit VariableExprNode(std::string name)
        : VariableExprNode(SourceLocation(), std::move(name)) {}

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the variable name
     */
    const std::string& getName() const { return m_name; }

    // Implement clone for deep copying
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<VariableExprNode>(getLocation(), m_name);
    }

private:
    std::string m_name;
};

/**
 * @class UnaryExprNode
 * @brief Node for unary expressions (e.g., -x, !y)
 */
class UnaryExprNode : public ExprNode {
public:
    /**
     * @brief Construct a unary expression
     * @param op The operator token
     * @param right The operand
     */
    UnaryExprNode(Token op, std::unique_ptr<ExprNode> right);

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the operator token
     */
    const Token& getOperator() const { return m_operator; }

    /**
     * @brief Get the operand
     */
    const ExprNode& getRight() const { return *m_right; }

    /**
     * @brief Get the operand (mutable version)
     */
    ExprNode& getRight() { return *m_right; }

    // Implement clone for deep copying
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<UnaryExprNode>(m_operator, m_right->clone());
    }

private:
    Token m_operator;
    std::unique_ptr<ExprNode> m_right;
};

/**
 * @class BinaryExprNode
 * @brief Node for binary expressions (e.g., a + b, x == y)
 */
class BinaryExprNode : public ExprNode {
public:
    /**
     * @brief Construct a binary expression
     * @param left The left operand
     * @param op The operator token
     * @param right The right operand
     */
    BinaryExprNode(std::unique_ptr<ExprNode> left, Token op, std::unique_ptr<ExprNode> right);

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the left operand
     */
    const ExprNode& getLeft() const { return *m_left; }

    /**
     * @brief Get the left operand (mutable version)
     */
    ExprNode& getLeft() { return *m_left; }

    /**
     * @brief Get the operator token
     */
    const Token& getOperator() const { return m_operator; }

    /**
     * @brief Get the right operand
     */
    const ExprNode& getRight() const { return *m_right; }

    /**
     * @brief Get the right operand (mutable version)
     */
    ExprNode& getRight() { return *m_right; }

    // Implement clone for deep copying
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<BinaryExprNode>(m_left->clone(), m_operator, m_right->clone());
    }

private:
    std::unique_ptr<ExprNode> m_left;
    Token m_operator;
    std::unique_ptr<ExprNode> m_right;
};

/**
 * @class GroupingExprNode
 * @brief Node for grouped expressions with parentheses
 */
class GroupingExprNode : public ExprNode {
public:
    /**
     * @brief Construct a grouping expression
     * @param expression The contained expression
     */
    explicit GroupingExprNode(std::unique_ptr<ExprNode> expression);

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the contained expression
     */
    const ExprNode& getExpression() const { return *m_expression; }

    /**
     * @brief Get the contained expression (mutable version)
     */
    ExprNode& getExpression() { return *m_expression; }

    // Implement clone for deep copying
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<GroupingExprNode>(m_expression->clone());
    }

private:
    std::unique_ptr<ExprNode> m_expression;
};

/**
 * @class AssignExprNode
 * @brief Node for assignment expressions (e.g., a = b)
 */
class AssignExprNode : public ExprNode {
public:
    /**
     * @brief Construct an assignment expression
     * @param location The source location
     * @param name The variable name
     * @param op The assignment operator token (=, +=, -=, etc.)
     * @param value The value to assign
     */
    AssignExprNode(const SourceLocation& location, std::string name, Token op,
                   std::unique_ptr<ExprNode> value)
        : ExprNode(location)
        , m_name(std::move(name))
        , m_operator(std::move(op))
        , m_value(std::move(value)) {}

    /**
     * @brief Construct an assignment expression using the operator's location
     * @param name The variable name
     * @param op The assignment operator token (=, +=, -=, etc.)
     * @param value The value to assign
     */
    AssignExprNode(std::string name, Token op, std::unique_ptr<ExprNode> value)
        : AssignExprNode(op.location, std::move(name), std::move(op), std::move(value)) {}

    /**
     * @brief Backward-compatible constructor (uses a default source location and operator)
     * @param name The variable name
     * @param value The value to assign
     */
    AssignExprNode(std::string name, std::unique_ptr<ExprNode> value)
        : AssignExprNode(SourceLocation(), std::move(name), Token(), std::move(value)) {}

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the variable name
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief Get the operator token
     */
    const Token& getOperator() const { return m_operator; }

    /**
     * @brief Get the value to assign
     */
    const ExprNode& getValue() const { return *m_value; }

    /**
     * @brief Get the value to assign (mutable version)
     */
    ExprNode& getValue() { return *m_value; }

    // Implement clone for deep copying
    std::unique_ptr<ExprNode> clone() const override {
        return std::make_unique<AssignExprNode>(getLocation(), m_name, m_operator,
                                                m_value->clone());
    }

private:
    std::string m_name;
    Token m_operator;
    std::unique_ptr<ExprNode> m_value;
};

/**
 * @class CallExprNode
 * @brief Node for function call expressions (e.g., print(x), foo())
 */
class CallExprNode : public ExprNode {
public:
    /**
     * @brief Construct a call expression
     * @param callee The function to call
     * @param arguments The arguments to pass
     */
    CallExprNode(std::unique_ptr<ExprNode> callee,
                 std::vector<std::unique_ptr<ExprNode>> arguments);

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the callee
     */
    const ExprNode& getCallee() const { return *m_callee; }

    /**
     * @brief Get the callee (mutable version)
     */
    ExprNode& getCallee() { return *m_callee; }

    /**
     * @brief Get the arguments
     */
    const std::vector<std::unique_ptr<ExprNode>>& getArguments() const { return m_arguments; }

    // Implement clone for deep copying
    std::unique_ptr<ExprNode> clone() const override {
        std::vector<std::unique_ptr<ExprNode>> clonedArgs;
        for (const auto& arg : m_arguments) {
            clonedArgs.push_back(arg->clone());
        }
        return std::make_unique<CallExprNode>(m_callee->clone(), std::move(clonedArgs));
    }

private:
    std::unique_ptr<ExprNode> m_callee;
    std::vector<std::unique_ptr<ExprNode>> m_arguments;
};

// Statement nodes

/**
 * @class StmtNode
 * @brief Base class for all statement nodes
 */
class StmtNode : public ASTNode {
public:
    explicit StmtNode(const SourceLocation& location) : ASTNode(location) {}
    virtual ~StmtNode() = default;
};

/**
 * @class ExprStmtNode
 * @brief Node for expression statements
 */
class ExprStmtNode : public StmtNode {
public:
    /**
     * @brief Construct an expression statement
     * @param expression The expression
     */
    explicit ExprStmtNode(std::unique_ptr<ExprNode> expression);

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the expression
     */
    const ExprNode& getExpression() const { return *m_expression; }

    /**
     * @brief Get the expression (mutable version)
     */
    ExprNode& getExpression() { return *m_expression; }

private:
    std::unique_ptr<ExprNode> m_expression;
};

/**
 * @class BlockStmtNode
 * @brief Node for blocks of statements
 */
class BlockStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a block statement
     * @param statements The statements in the block
     */
    explicit BlockStmtNode(std::vector<std::unique_ptr<StmtNode>> statements);

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the statements
     */
    const std::vector<std::unique_ptr<StmtNode>>& getStatements() const { return m_statements; }

    /**
     * @brief Get the statements (mutable version)
     */
    std::vector<std::unique_ptr<StmtNode>>& getStatements() { return m_statements; }

private:
    std::vector<std::unique_ptr<StmtNode>> m_statements;
};

/**
 * @class TypeNode
 * @brief Node for a type reference
 */
class TypeNode : public ASTNode {
public:
    /**
     * @brief Construct a type node
     * @param location The source location
     * @param typeName The type name or expression
     */
    TypeNode(const SourceLocation& location, const std::string& typeName)
        : ASTNode(location), m_typeName(typeName) {}

    /**
     * @brief Accept a visitor
     * @param visitor The visitor to accept
     */
    void accept(ASTVisitor& visitor) const override { visitor.visitType(*this); }

    /**
     * @brief Get the type name
     * @return The type name
     */
    const std::string& getName() const { return m_typeName; }

    /**
     * @brief Get the type name as a token
     * @return A token containing the type name
     */
    Token getNameToken() const {
        Token token;
        token.type = TokenType::IDENTIFIER;
        token.lexeme = m_typeName;
        token.location = getLocation();
        return token;
    }

    /**
     * @brief Create a string representation
     * @return String representation
     */
    std::string toString() const { return m_typeName; }

    /**
     * @brief Create a deep copy of this node
     * @return A unique pointer to the copied node
     */
    std::unique_ptr<TypeNode> clone() const {
        return std::make_unique<TypeNode>(getLocation(), m_typeName);
    }

private:
    std::string m_typeName;
};

/**
 * @class VariableDeclStmtNode
 * @brief Node for variable declarations
 */
class VariableDeclStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a variable declaration statement
     * @param location The source location
     * @param name The variable name
     * @param type The variable type
     * @param modifier The variable modifier (CONF or REB)
     * @param flags The variable flags
     * @param initializer The initializer (optional)
     */
    VariableDeclStmtNode(const SourceLocation& location, std::string name,
                         std::unique_ptr<TypeNode> type, VariableModifier modifier,
                         std::vector<VariableFlag> flags, std::unique_ptr<ExprNode> initializer)
        : StmtNode(location)
        , m_name(std::move(name))
        , m_type(std::move(type))
        , m_modifier(modifier)
        , m_flags(std::move(flags))
        , m_initializer(std::move(initializer)) {}

    /**
     * @brief Construct a variable declaration statement without a source location
     * @param name The variable name
     * @param type The variable type
     * @param modifier The variable modifier (CONF or REB)
     * @param flags The variable flags
     * @param initializer The initializer (optional)
     */
    VariableDeclStmtNode(std::string name, std::unique_ptr<TypeNode> type,
                         VariableModifier modifier, std::vector<VariableFlag> flags,
                         std::unique_ptr<ExprNode> initializer)
        : StmtNode(SourceLocation())
        , m_name(std::move(name))
        , m_type(std::move(type))
        , m_modifier(modifier)
        , m_flags(std::move(flags))
        , m_initializer(std::move(initializer)) {}

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the variable name
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief Get the variable type
     */
    const TypeNode& getType() const { return *m_type; }

    /**
     * @brief Get the variable type (mutable version)
     */
    TypeNode& getType() { return *m_type; }

    /**
     * @brief Get the variable modifier
     */
    VariableModifier getModifier() const { return m_modifier; }

    /**
     * @brief Get the variable flags
     */
    const std::vector<VariableFlag>& getFlags() const { return m_flags; }

    /**
     * @brief Check if the variable has an initializer
     */
    bool hasInitializer() const { return m_initializer != nullptr; }

    /**
     * @brief Get the initializer
     * @throws std::runtime_error if there is no initializer
     */
    const ExprNode& getInitializer() const;

    /**
     * @brief Get the initializer (mutable version)
     * @throws std::runtime_error if there is no initializer
     */
    ExprNode& getInitializer();

private:
    std::string m_name;
    std::unique_ptr<TypeNode> m_type;
    VariableModifier m_modifier;
    std::vector<VariableFlag> m_flags;
    std::unique_ptr<ExprNode> m_initializer;
};

/**
 * @class ReturnStmtNode
 * @brief Node for return statements
 */
class ReturnStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a return statement
     * @param location The source location
     * @param keyword The return keyword token
     * @param value The value to return (can be nullptr for void returns)
     */
    ReturnStmtNode(const SourceLocation& location, Token keyword, std::unique_ptr<ExprNode> value)
        : StmtNode(location), m_keyword(std::move(keyword)), m_value(std::move(value)) {}

    /**
     * @brief Construct a return statement using the keyword's location
     * @param keyword The return keyword token
     * @param value The value to return (can be nullptr for void returns)
     */
    ReturnStmtNode(Token keyword, std::unique_ptr<ExprNode> value)
        : ReturnStmtNode(keyword.location, std::move(keyword), std::move(value)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitReturnStmt(*this); }

    const Token& getKeyword() const { return m_keyword; }

    bool hasValue() const { return m_value != nullptr; }

    const ExprNode& getValue() const {
        if (!m_value) {
            throw std::runtime_error("Return statement has no value");
        }
        return *m_value;
    }

private:
    Token m_keyword;
    std::unique_ptr<ExprNode> m_value;
};

/**
 * @class IfStmtNode
 * @brief Node for if statements
 */
class IfStmtNode : public StmtNode {
public:
    /**
     * @brief Construct an if statement with a specific source location
     * @param location The source location
     * @param condition The condition expression
     * @param thenBranch The then branch
     * @param elseBranch The else branch (optional)
     */
    IfStmtNode(const SourceLocation& location, std::unique_ptr<ExprNode> condition,
               std::unique_ptr<StmtNode> thenBranch, std::unique_ptr<StmtNode> elseBranch = nullptr)
        : StmtNode(location)
        , m_condition(std::move(condition))
        , m_thenBranch(std::move(thenBranch))
        , m_elseBranch(std::move(elseBranch)) {}

    /**
     * @brief Construct an if statement using the condition's location
     * @param condition The condition expression
     * @param thenBranch The then branch
     * @param elseBranch The else branch (optional)
     */
    IfStmtNode(std::unique_ptr<ExprNode> condition, std::unique_ptr<StmtNode> thenBranch,
               std::unique_ptr<StmtNode> elseBranch = nullptr)
        : IfStmtNode(condition ? condition->getLocation() : SourceLocation(), std::move(condition),
                     std::move(thenBranch), std::move(elseBranch)) {}

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the condition expression
     */
    const ExprNode& getCondition() const { return *m_condition; }

    /**
     * @brief Get the condition expression (mutable version)
     */
    ExprNode& getCondition() { return *m_condition; }

    /**
     * @brief Get the then branch
     */
    const StmtNode& getThenBranch() const { return *m_thenBranch; }

    /**
     * @brief Get the then branch (mutable version)
     */
    StmtNode& getThenBranch() { return *m_thenBranch; }

    /**
     * @brief Check if there is an else branch
     */
    bool hasElseBranch() const { return m_elseBranch != nullptr; }

    /**
     * @brief Get the else branch
     * @throws std::runtime_error if there is no else branch
     */
    const StmtNode& getElseBranch() const;

    /**
     * @brief Get the else branch (mutable version)
     * @throws std::runtime_error if there is no else branch
     */
    StmtNode& getElseBranch();

private:
    std::unique_ptr<ExprNode> m_condition;
    std::unique_ptr<StmtNode> m_thenBranch;
    std::unique_ptr<StmtNode> m_elseBranch;
};

/**
 * @class TemporalOpStmtNode
 * @brief Node for temporal operations
 */
class TemporalOpStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a temporal operation statement with a specific source location
     * @param location The source location
     * @param opType The type of temporal operation
     * @param arguments The arguments to the operation
     * @param body The body of the operation
     */
    TemporalOpStmtNode(const SourceLocation& location, TemporalOpType opType,
                       std::vector<std::unique_ptr<ExprNode>> arguments,
                       std::unique_ptr<BlockStmtNode> body)
        : StmtNode(location)
        , m_opType(opType)
        , m_arguments(std::move(arguments))
        , m_body(std::move(body)) {}

    /**
     * @brief Construct a temporal operation statement using the body's location
     * @param opType The type of temporal operation
     * @param arguments The arguments to the operation
     * @param body The body of the operation
     */
    TemporalOpStmtNode(TemporalOpType opType, std::vector<std::unique_ptr<ExprNode>> arguments,
                       std::unique_ptr<BlockStmtNode> body)
        : TemporalOpStmtNode(body ? body->getLocation() : SourceLocation(), opType,
                             std::move(arguments), std::move(body)) {}

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the operation type
     */
    TemporalOpType getOpType() const { return m_opType; }

    /**
     * @brief Get the arguments
     */
    const std::vector<std::unique_ptr<ExprNode>>& getArguments() const { return m_arguments; }

    /**
     * @brief Get the arguments (mutable version)
     */
    std::vector<std::unique_ptr<ExprNode>>& getArguments() { return m_arguments; }

    /**
     * @brief Get the body
     */
    const BlockStmtNode& getBody() const { return *m_body; }

    /**
     * @brief Get the body (mutable version)
     */
    BlockStmtNode& getBody() { return *m_body; }

private:
    TemporalOpType m_opType;
    std::vector<std::unique_ptr<ExprNode>> m_arguments;
    std::unique_ptr<BlockStmtNode> m_body;
};

/**
 * @class FunctionDeclStmtNode
 * @brief Node for function declarations
 */
class FunctionDeclStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a function declaration statement
     * @param location The source location
     * @param name The function name
     * @param body The function body
     */
    FunctionDeclStmtNode(const SourceLocation& location, std::string name,
                         std::unique_ptr<BlockStmtNode> body)
        : StmtNode(location), m_name(std::move(name)), m_body(std::move(body)) {}

    /**
     * @brief Backward-compatible constructor (uses a default source location)
     * @param name The function name
     * @param body The function body
     */
    FunctionDeclStmtNode(std::string name, std::unique_ptr<BlockStmtNode> body)
        : FunctionDeclStmtNode(SourceLocation(), std::move(name), std::move(body)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitFunctionDeclStmt(*this); }

    const std::string& getName() const { return m_name; }

    const BlockStmtNode& getBody() const { return *m_body; }

    BlockStmtNode& getBody() { return *m_body; }

    /**
     * @brief Get the parameter names
     * @return Vector of parameter names
     */
    const std::vector<std::string>& getParams() const {
        static const std::vector<std::string> empty;
        return empty;
    }

private:
    std::string m_name;
    std::unique_ptr<BlockStmtNode> m_body;
};

/**
 * @class ProgramNode
 * @brief The root node of the AST
 */
class ProgramNode : public ASTNode {
public:
    /**
     * @brief Construct a program node
     * @param statements The top-level statements
     */
    explicit ProgramNode(std::vector<std::unique_ptr<StmtNode>> statements);

    void accept(ASTVisitor& visitor) const override;

    /**
     * @brief Get the statements
     */
    const std::vector<std::unique_ptr<StmtNode>>& getStatements() const { return m_statements; }

    /**
     * @brief Get the statements (mutable version)
     */
    std::vector<std::unique_ptr<StmtNode>>& getStatements() { return m_statements; }

private:
    std::vector<std::unique_ptr<StmtNode>> m_statements;
};

/**
 * @class TypeDefStmtNode
 * @brief Represents a type definition statement
 */
class TypeDefStmtNode : public StmtNode {
public:
    // Constructor for struct type definition
    TypeDefStmtNode(const SourceLocation& location, const std::string& name,
                    const std::vector<StructField>& fields,
                    const std::vector<std::string>& typeParameters = {});

    // Constructor for enum type definition
    TypeDefStmtNode(const SourceLocation& location, const std::string& name,
                    const std::vector<EnumVariant>& variants,
                    const std::vector<std::string>& typeParameters = {});

    // Constructor for union type definition
    TypeDefStmtNode(const SourceLocation& location, const std::string& name,
                    const std::vector<std::shared_ptr<TypeNode>>& unionTypes,
                    const std::vector<std::string>& typeParameters = {});

    // Constructor for type alias
    TypeDefStmtNode(const SourceLocation& location, const std::string& name,
                    std::shared_ptr<TypeNode> aliasedType,
                    const std::vector<std::string>& typeParameters = {});

    void accept(ASTVisitor& visitor) const override;

    // Getters
    const std::string& getName() const;
    const std::vector<StructField>& getFields() const;
    const std::vector<EnumVariant>& getVariants() const;
    const std::vector<std::shared_ptr<TypeNode>>& getUnionTypes() const;
    std::shared_ptr<TypeNode> getAliasedType() const;
    const std::vector<std::string>& getTypeParameters() const;

    // Type checkers
    bool isStruct() const;
    bool isEnum() const;
    bool isUnion() const;
    bool isAlias() const;
    bool isGeneric() const;

private:
    std::string m_name;
    std::vector<StructField> m_fields;
    std::vector<EnumVariant> m_variants;
    std::vector<std::shared_ptr<TypeNode>> m_unionTypes;
    std::shared_ptr<TypeNode> m_aliasedType;
    std::vector<std::string> m_typeParameters;

    enum class TypeDefKind { STRUCT, ENUM, UNION, ALIAS };

    TypeDefKind m_kind;
};

/**
 * @enum TemporalSectionType
 * @brief Defines the types of sections within a temporal_program block
 */
enum class TemporalSectionType {
    NAME,                  // name { ... }
    TYPE,                  // type { ... }
    AUTHOR,                // author { ... }
    VERSION,               // version { ... }
    DESCRIPTION,           // description { ... }
    RESOURCES_REQUIRED,    // resources_required { ... }
    TIMELINE_BEHAVIOR,     // timeline_behavior { ... }
    STABILITY_TARGET,      // stability_target { ... }
    DEFAULT_FLOW,          // default_flow { ... }
    PARADOX_HANDLING,      // paradox_handling { ... }
    VARIABLES,             // variables { ... }
    INITIALIZATION_PHASE,  // initialization_phase { ... }
    EXECUTION_WEAVE,       // execution_weave { ... }
    CLEANUP_PHASE          // cleanup_phase { ... }
};

/**
 * @class TemporalSectionNode
 * @brief Node for a section within a temporal_program block
 */
class TemporalSectionNode : public ASTNode {
public:
    /**
     * @brief Construct a temporal section node
     * @param location The source location
     * @param type The section type
     * @param content The section content (a statement or expression)
     */
    TemporalSectionNode(const SourceLocation& location, TemporalSectionType type,
                        std::unique_ptr<ASTNode> content)
        : ASTNode(location), m_type(type), m_content(std::move(content)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitTemporalSection(*this); }

    /**
     * @brief Get the section type
     */
    TemporalSectionType getType() const { return m_type; }

    /**
     * @brief Get the section content
     */
    const ASTNode& getContent() const { return *m_content; }

    /**
     * @brief Get the section content (mutable version)
     */
    ASTNode& getContent() { return *m_content; }

private:
    TemporalSectionType m_type;
    std::unique_ptr<ASTNode> m_content;
};

/**
 * @class TemporalProgramNode
 * @brief Node for a temporal_program block
 */
class TemporalProgramNode : public StmtNode {
public:
    /**
     * @brief Construct a temporal program node
     * @param location The source location
     * @param sections The program sections
     */
    TemporalProgramNode(
        const SourceLocation& location,
        std::map<TemporalSectionType, std::unique_ptr<TemporalSectionNode>> sections)
        : StmtNode(location), m_sections(std::move(sections)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitTemporalProgram(*this); }

    /**
     * @brief Check if a section exists
     * @param type The section type to check for
     * @return True if the section exists
     */
    bool hasSection(TemporalSectionType type) const {
        return m_sections.find(type) != m_sections.end();
    }

    /**
     * @brief Get a section by type
     * @param type The section type to get
     * @return The section node
     * @throws std::runtime_error if the section doesn't exist
     */
    const TemporalSectionNode& getSection(TemporalSectionType type) const {
        auto it = m_sections.find(type);
        if (it == m_sections.end()) {
            throw std::runtime_error("Section does not exist in temporal program");
        }
        return *it->second;
    }

    /**
     * @brief Get all sections
     * @return The map of all sections
     */
    const std::map<TemporalSectionType, std::unique_ptr<TemporalSectionNode>>& getSections() const {
        return m_sections;
    }

private:
    std::map<TemporalSectionType, std::unique_ptr<TemporalSectionNode>> m_sections;
};

/**
 * @class DeclareConstantNode
 * @brief Node for DECLARE CONF::FLAG statements
 */
class DeclareConstantNode : public StmtNode {
public:
    /**
     * @brief Construct a declare constant node
     * @param location The source location
     * @param name The constant name
     * @param type The constant type
     * @param flags The constant flags
     * @param initializer The initializer expression
     */
    DeclareConstantNode(const SourceLocation& location, std::string name,
                        std::unique_ptr<TypeNode> type, std::vector<VariableFlag> flags,
                        std::unique_ptr<ExprNode> initializer)
        : StmtNode(location)
        , m_name(std::move(name))
        , m_type(std::move(type))
        , m_flags(std::move(flags))
        , m_initializer(std::move(initializer)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitDeclareConstant(*this); }

    /**
     * @brief Get the constant name
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief Get the constant type
     */
    const TypeNode& getType() const { return *m_type; }

    /**
     * @brief Get the constant flags
     */
    const std::vector<VariableFlag>& getFlags() const { return m_flags; }

    /**
     * @brief Check if the constant has an initializer
     */
    bool hasInitializer() const { return m_initializer != nullptr; }

    /**
     * @brief Get the initializer
     * @throws std::runtime_error if there is no initializer
     */
    const ExprNode& getInitializer() const {
        if (!m_initializer) {
            throw std::runtime_error("Constant has no initializer");
        }
        return *m_initializer;
    }

private:
    std::string m_name;
    std::unique_ptr<TypeNode> m_type;
    std::vector<VariableFlag> m_flags;
    std::unique_ptr<ExprNode> m_initializer;
};

/**
 * @class ForChrononStmtNode
 * @brief Node for FOR_CHRONON loop statements
 */
class ForChrononStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a FOR_CHRONON loop statement
     * @param location The source location
     * @param iterator The loop iterator variable name
     * @param rangeStart The start of the range expression
     * @param rangeEnd The end of the range expression
     * @param stepExpr The step expression (optional)
     * @param body The loop body
     */
    ForChrononStmtNode(const SourceLocation& location, std::string iterator,
                       std::unique_ptr<ExprNode> rangeStart, std::unique_ptr<ExprNode> rangeEnd,
                       std::unique_ptr<ExprNode> stepExpr, std::unique_ptr<BlockStmtNode> body)
        : StmtNode(location)
        , m_iterator(std::move(iterator))
        , m_rangeStart(std::move(rangeStart))
        , m_rangeEnd(std::move(rangeEnd))
        , m_stepExpr(std::move(stepExpr))
        , m_body(std::move(body)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitForChrononStmt(*this); }

    /**
     * @brief Get the iterator variable name
     */
    const std::string& getIterator() const { return m_iterator; }

    /**
     * @brief Get the range start expression
     */
    const ExprNode& getRangeStart() const { return *m_rangeStart; }

    /**
     * @brief Get the range end expression
     */
    const ExprNode& getRangeEnd() const { return *m_rangeEnd; }

    /**
     * @brief Check if the loop has a step expression
     */
    bool hasStep() const { return m_stepExpr != nullptr; }

    /**
     * @brief Get the step expression
     * @throws std::runtime_error if there is no step expression
     */
    const ExprNode& getStepExpr() const {
        if (!m_stepExpr) {
            throw std::runtime_error("FOR_CHRONON loop has no step expression");
        }
        return *m_stepExpr;
    }

    /**
     * @brief Get the loop body
     */
    const BlockStmtNode& getBody() const { return *m_body; }

    /**
     * @brief Get the loop body (mutable version)
     */
    BlockStmtNode& getBody() { return *m_body; }

private:
    std::string m_iterator;
    std::unique_ptr<ExprNode> m_rangeStart;
    std::unique_ptr<ExprNode> m_rangeEnd;
    std::unique_ptr<ExprNode> m_stepExpr;
    std::unique_ptr<BlockStmtNode> m_body;
};

/**
 * @class WhileEventStmtNode
 * @brief Node for WHILE_EVENT loop statements
 */
class WhileEventStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a WHILE_EVENT loop statement
     * @param location The source location
     * @param condition The loop condition expression
     * @param body The loop body
     */
    WhileEventStmtNode(const SourceLocation& location, std::unique_ptr<ExprNode> condition,
                       std::unique_ptr<BlockStmtNode> body)
        : StmtNode(location), m_condition(std::move(condition)), m_body(std::move(body)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitWhileEventStmt(*this); }

    /**
     * @brief Get the condition expression
     */
    const ExprNode& getCondition() const { return *m_condition; }

    /**
     * @brief Get the condition expression (mutable version)
     */
    ExprNode& getCondition() { return *m_condition; }

    /**
     * @brief Get the loop body
     */
    const BlockStmtNode& getBody() const { return *m_body; }

    /**
     * @brief Get the loop body (mutable version)
     */
    BlockStmtNode& getBody() { return *m_body; }

private:
    std::unique_ptr<ExprNode> m_condition;
    std::unique_ptr<BlockStmtNode> m_body;
};

/**
 * @class RewindFlowStmtNode
 * @brief Node for REWIND_FLOW loop statements
 */
class RewindFlowStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a REWIND_FLOW loop statement
     * @param location The source location
     * @param condition The rewind condition expression
     * @param maxRewinds The maximum number of rewinds (optional)
     * @param body The loop body
     */
    RewindFlowStmtNode(const SourceLocation& location, std::unique_ptr<ExprNode> condition,
                       std::unique_ptr<ExprNode> maxRewinds, std::unique_ptr<BlockStmtNode> body)
        : StmtNode(location)
        , m_condition(std::move(condition))
        , m_maxRewinds(std::move(maxRewinds))
        , m_body(std::move(body)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitRewindFlowStmt(*this); }

    /**
     * @brief Get the condition expression
     */
    const ExprNode& getCondition() const { return *m_condition; }

    /**
     * @brief Get the condition expression (mutable version)
     */
    ExprNode& getCondition() { return *m_condition; }

    /**
     * @brief Check if the loop has a max rewinds limit
     */
    bool hasMaxRewinds() const { return m_maxRewinds != nullptr; }

    /**
     * @brief Get the max rewinds expression
     * @throws std::runtime_error if there is no max rewinds expression
     */
    const ExprNode& getMaxRewinds() const {
        if (!m_maxRewinds) {
            throw std::runtime_error("REWIND_FLOW loop has no max rewinds expression");
        }
        return *m_maxRewinds;
    }

    /**
     * @brief Get the loop body
     */
    const BlockStmtNode& getBody() const { return *m_body; }

    /**
     * @brief Get the loop body (mutable version)
     */
    BlockStmtNode& getBody() { return *m_body; }

private:
    std::unique_ptr<ExprNode> m_condition;
    std::unique_ptr<ExprNode> m_maxRewinds;
    std::unique_ptr<BlockStmtNode> m_body;
};

/**
 * @class DissonanceHandlerNode
 * @brief Node for ON_DISSONANCE handlers in ATTEMPT_WEAVE statements
 */
class DissonanceHandlerNode : public ASTNode {
public:
    /**
     * @brief Construct a dissonance handler node with a specific dissonance type
     * @param location The source location
     * @param dissonanceType The dissonance type to catch (nullptr for catch-all)
     * @param errorVar The error variable name
     * @param body The handler body
     */
    DissonanceHandlerNode(const SourceLocation& location, std::unique_ptr<TypeNode> dissonanceType,
                          std::string errorVar, std::unique_ptr<BlockStmtNode> body)
        : ASTNode(location)
        , m_dissonanceType(std::move(dissonanceType))
        , m_errorVar(std::move(errorVar))
        , m_body(std::move(body)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitDissonanceHandler(*this); }

    /**
     * @brief Check if the handler has a specific dissonance type
     */
    bool hasDissonanceType() const { return m_dissonanceType != nullptr; }

    /**
     * @brief Get the dissonance type
     * @throws std::runtime_error if there is no dissonance type
     */
    const TypeNode& getDissonanceType() const {
        if (!m_dissonanceType) {
            throw std::runtime_error("Dissonance handler has no dissonance type");
        }
        return *m_dissonanceType;
    }

    /**
     * @brief Get the error variable name
     */
    const std::string& getErrorVar() const { return m_errorVar; }

    /**
     * @brief Get the handler body
     */
    const BlockStmtNode& getBody() const { return *m_body; }

    /**
     * @brief Get the handler body (mutable version)
     */
    BlockStmtNode& getBody() { return *m_body; }

private:
    std::unique_ptr<TypeNode> m_dissonanceType;
    std::string m_errorVar;
    std::unique_ptr<BlockStmtNode> m_body;
};

/**
 * @class AttemptWeaveStmtNode
 * @brief Node for ATTEMPT_WEAVE statements (try-catch-finally)
 */
class AttemptWeaveStmtNode : public StmtNode {
public:
    /**
     * @brief Construct an ATTEMPT_WEAVE statement
     * @param location The source location
     * @param tryBlock The try block
     * @param handlers The dissonance handlers (ON_DISSONANCE blocks)
     * @param finallyBlock The finally block (ENSURE_HARMONY)
     */
    AttemptWeaveStmtNode(const SourceLocation& location, std::unique_ptr<BlockStmtNode> tryBlock,
                         std::vector<std::unique_ptr<DissonanceHandlerNode>> handlers,
                         std::unique_ptr<BlockStmtNode> finallyBlock)
        : StmtNode(location)
        , m_tryBlock(std::move(tryBlock))
        , m_handlers(std::move(handlers))
        , m_finallyBlock(std::move(finallyBlock)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitAttemptWeaveStmt(*this); }

    /**
     * @brief Get the try block
     */
    const BlockStmtNode& getTryBlock() const { return *m_tryBlock; }

    /**
     * @brief Get the try block (mutable version)
     */
    BlockStmtNode& getTryBlock() { return *m_tryBlock; }

    /**
     * @brief Get the dissonance handlers
     */
    const std::vector<std::unique_ptr<DissonanceHandlerNode>>& getHandlers() const {
        return m_handlers;
    }

    /**
     * @brief Check if the statement has a finally block
     */
    bool hasFinallyBlock() const { return m_finallyBlock != nullptr; }

    /**
     * @brief Get the finally block
     * @throws std::runtime_error if there is no finally block
     */
    const BlockStmtNode& getFinallyBlock() const {
        if (!m_finallyBlock) {
            throw std::runtime_error("ATTEMPT_WEAVE statement has no finally block");
        }
        return *m_finallyBlock;
    }

    /**
     * @brief Get the finally block (mutable version)
     * @throws std::runtime_error if there is no finally block
     */
    BlockStmtNode& getFinallyBlock() {
        if (!m_finallyBlock) {
            throw std::runtime_error("ATTEMPT_WEAVE statement has no finally block");
        }
        return *m_finallyBlock;
    }

private:
    std::unique_ptr<BlockStmtNode> m_tryBlock;
    std::vector<std::unique_ptr<DissonanceHandlerNode>> m_handlers;
    std::unique_ptr<BlockStmtNode> m_finallyBlock;
};

/**
 * @class BreakChrononStmtNode
 * @brief Node for BREAK_CHRONON statement that prematurely exits a FOR_CHRONON loop
 */
class BreakChrononStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a BREAK_CHRONON statement
     * @param location The source location
     */
    explicit BreakChrononStmtNode(const SourceLocation& location) : StmtNode(location) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitBreakChrononStmt(*this); }
};

/**
 * @class ContinueWeaveStmtNode
 * @brief Node for CONTINUE_WEAVE statement that skips to the next iteration of any loop
 */
class ContinueWeaveStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a CONTINUE_WEAVE statement
     * @param location The source location
     */
    explicit ContinueWeaveStmtNode(const SourceLocation& location) : StmtNode(location) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitContinueWeaveStmt(*this); }
};

/**
 * @class ReverseFlowStmtNode
 * @brief Node for REVERSE_FLOW statement that inverts the iteration direction in reversible loops
 */
class ReverseFlowStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a REVERSE_FLOW statement
     * @param location The source location
     */
    explicit ReverseFlowStmtNode(const SourceLocation& location) : StmtNode(location) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitReverseFlowStmt(*this); }
};

/**
 * @class HaltRewindStmtNode
 * @brief Node for HALT_REWIND statement that terminates a REWIND_FLOW operation immediately
 */
class HaltRewindStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a HALT_REWIND statement
     * @param location The source location
     */
    explicit HaltRewindStmtNode(const SourceLocation& location) : StmtNode(location) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitHaltRewindStmt(*this); }
};

/**
 * @class StabilizeTimelineStmtNode
 * @brief Node for STABILIZE_TIMELINE statement that reduces paradox risk in REWIND_FLOW
 */
class StabilizeTimelineStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a STABILIZE_TIMELINE statement
     * @param location The source location
     * @param intensity The optional stabilization intensity expression (0.0-1.0)
     */
    StabilizeTimelineStmtNode(const SourceLocation& location,
                              std::unique_ptr<ExprNode> intensity = nullptr)
        : StmtNode(location), m_intensity(std::move(intensity)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitStabilizeTimelineStmt(*this); }

    /**
     * @brief Check if the statement has an intensity expression
     */
    bool hasIntensity() const { return m_intensity != nullptr; }

    /**
     * @brief Get the intensity expression
     * @throws std::runtime_error if there is no intensity expression
     */
    const ExprNode& getIntensity() const {
        if (!m_intensity) {
            throw std::runtime_error("STABILIZE_TIMELINE has no intensity expression");
        }
        return *m_intensity;
    }

private:
    std::unique_ptr<ExprNode> m_intensity;
};

/**
 * @class PreventModificationStmtNode
 * @brief Node for PREVENT_MODIFICATION statement that makes REWIND_FLOW read-only
 */
class PreventModificationStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a PREVENT_MODIFICATION statement
     * @param location The source location
     * @param scope The optional scope expression (local, global, or timeline)
     */
    PreventModificationStmtNode(const SourceLocation& location,
                                std::unique_ptr<ExprNode> scope = nullptr)
        : StmtNode(location), m_scope(std::move(scope)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitPreventModificationStmt(*this); }

    /**
     * @brief Check if the statement has a scope expression
     */
    bool hasScope() const { return m_scope != nullptr; }

    /**
     * @brief Get the scope expression
     * @throws std::runtime_error if there is no scope expression
     */
    const ExprNode& getScope() const {
        if (!m_scope) {
            throw std::runtime_error("PREVENT_MODIFICATION has no scope expression");
        }
        return *m_scope;
    }

private:
    std::unique_ptr<ExprNode> m_scope;
};

/**
 * @class ExpendResourcesStmtNode
 * @brief Node for EXPEND_RESOURCES blocks that conditionally execute based on resource availability
 */
class ExpendResourcesStmtNode : public StmtNode {
public:
    /**
     * @brief Construct an EXPEND_RESOURCES statement
     * @param location The source location
     * @param aethelCostExpr Optional expression for Aethel cost
     * @param chrononCostExpr Optional expression for Chronon cost
     * @param mainBlock Block to execute if resources are sufficient
     * @param insufficientHandlerBlock Optional block for when resources are insufficient
     */
    ExpendResourcesStmtNode(
        const SourceLocation& location, std::unique_ptr<ExprNode> aethelCostExpr,
        std::unique_ptr<ExprNode> chrononCostExpr, std::unique_ptr<BlockStmtNode> mainBlock,
        std::unique_ptr<ResourceInsufficiencyHandlerNode> insufficientHandlerBlock)
        : StmtNode(location)
        , m_aethelCostExpr(std::move(aethelCostExpr))
        , m_chrononCostExpr(std::move(chrononCostExpr))
        , m_mainBlock(std::move(mainBlock))
        , m_insufficientHandlerBlock(std::move(insufficientHandlerBlock))
        , m_cachedAethelCost(0.0)
        , m_cachedChrononCost(0.0)
        , m_hasCalculatedCosts(false) {
        // Validate that at least one resource cost is specified
        if (!m_aethelCostExpr && !m_chrononCostExpr) {
            throw std::runtime_error("EXPEND_RESOURCES must specify at least one resource cost");
        }
    }

    void accept(ASTVisitor& visitor) const override { visitor.visitExpendResourcesStmt(*this); }

    /**
     * @brief Check if there is an Aethel cost expression
     */
    bool hasAethelCost() const { return m_aethelCostExpr != nullptr; }

    /**
     * @brief Get the Aethel cost expression
     * @throws std::runtime_error if there is no Aethel cost expression
     */
    const ExprNode& getAethelCostExpr() const {
        if (!m_aethelCostExpr) {
            throw std::runtime_error("EXPEND_RESOURCES has no Aethel cost expression");
        }
        return *m_aethelCostExpr;
    }

    /**
     * @brief Check if there is a Chronon cost expression
     */
    bool hasChrononCost() const { return m_chrononCostExpr != nullptr; }

    /**
     * @brief Get the Chronon cost expression
     * @throws std::runtime_error if there is no Chronon cost expression
     */
    const ExprNode& getChrononCostExpr() const {
        if (!m_chrononCostExpr) {
            throw std::runtime_error("EXPEND_RESOURCES has no Chronon cost expression");
        }
        return *m_chrononCostExpr;
    }

    /**
     * @brief Get the main block to execute if resources are sufficient
     */
    const BlockStmtNode& getMainBlock() const { return *m_mainBlock; }

    /**
     * @brief Get the main block to execute if resources are sufficient (mutable version)
     */
    BlockStmtNode& getMainBlock() { return *m_mainBlock; }

    /**
     * @brief Check if there is an insufficient resources handler
     */
    bool hasInsufficientHandler() const { return m_insufficientHandlerBlock != nullptr; }

    /**
     * @brief Get the insufficient resources handler
     * @throws std::runtime_error if there is no insufficient resources handler
     */
    const ResourceInsufficiencyHandlerNode& getInsufficientHandler() const {
        if (!m_insufficientHandlerBlock) {
            throw std::runtime_error("EXPEND_RESOURCES has no insufficient resources handler");
        }
        return *m_insufficientHandlerBlock;
    }

    /**
     * @brief Try to precalculate resource costs for optimization
     * @return True if costs were successfully calculated
     *
     * This is an optimization that allows literal resource costs to be
     * precalculated during parsing rather than during execution.
     */
    bool tryPrecalculateResourceCosts() {
        if (hasAethelCost()) {
            const ExprNode& aethelExpr = getAethelCostExpr();
            if (aethelExpr.isLiteral()) {
                const LiteralExprNode* aethelLiteral =
                    dynamic_cast<const LiteralExprNode*>(&aethelExpr);
                if (aethelLiteral) {
                    const LiteralExprNode::LiteralValue& val = aethelLiteral->getValue();
                    if (std::holds_alternative<double>(val)) {
                        m_cachedAethelCost = std::get<double>(val);
                    } else if (std::holds_alternative<int64_t>(val)) {
                        m_cachedAethelCost = static_cast<double>(std::get<int64_t>(val));
                    } else {
                        return false;  // Not a number
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            m_cachedAethelCost = 0.0;  // No aethel cost specified
        }

        if (hasChrononCost()) {
            const ExprNode& chrononExpr = getChrononCostExpr();
            if (chrononExpr.isLiteral()) {
                const LiteralExprNode* chrononLiteral =
                    dynamic_cast<const LiteralExprNode*>(&chrononExpr);
                if (chrononLiteral) {
                    const LiteralExprNode::LiteralValue& val = chrononLiteral->getValue();
                    if (std::holds_alternative<double>(val)) {
                        m_cachedChrononCost = std::get<double>(val);
                    } else if (std::holds_alternative<int64_t>(val)) {
                        m_cachedChrononCost = static_cast<double>(std::get<int64_t>(val));
                    } else {
                        return false;  // Not a number
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            m_cachedChrononCost = 0.0;  // No chronon cost specified
        }

        m_hasCalculatedCosts = true;
        return true;
    }

    /**
     * @brief Get the cached Aethel cost if pre-calculated
     * @throws std::runtime_error if costs haven't been pre-calculated
     */
    double getCachedAethelCost() const {
        if (!m_hasCalculatedCosts) {
            throw std::runtime_error("Resource costs have not been pre-calculated");
        }
        return m_cachedAethelCost;
    }

    /**
     * @brief Get the cached Chronon cost if pre-calculated
     * @throws std::runtime_error if costs haven't been pre-calculated
     */
    double getCachedChrononCost() const {
        if (!m_hasCalculatedCosts) {
            throw std::runtime_error("Resource costs have not been pre-calculated");
        }
        return m_cachedChrononCost;
    }

    /**
     * @brief Check if resource costs have been pre-calculated
     */
    bool hasCalculatedCosts() const { return m_hasCalculatedCosts; }

    /**
     * @brief Estimate the combined resource cost as a single efficiency metric
     * @return A value representing the relative resource cost (higher is more expensive)
     *
     * This is useful for optimization algorithms that need to compare the
     * resource costs of different EXPEND_RESOURCES statements.
     */
    double estimateCombinedCost() const {
        // If costs are pre-calculated, use the cached values
        if (m_hasCalculatedCosts) {
            // Apply a weighted formula that considers Aethel more valuable than Chronons
            return m_cachedAethelCost * 2.5 + m_cachedChrononCost;
        }

        // Otherwise, make a simple estimate based on the presence of cost expressions
        double estimate = 0.0;
        if (m_aethelCostExpr)
            estimate += 50.0;  // Assume moderate Aethel cost
        if (m_chrononCostExpr)
            estimate += 20.0;  // Assume moderate Chronon cost
        return estimate;
    }

private:
    std::unique_ptr<ExprNode> m_aethelCostExpr;
    std::unique_ptr<ExprNode> m_chrononCostExpr;
    std::unique_ptr<BlockStmtNode> m_mainBlock;
    std::unique_ptr<ResourceInsufficiencyHandlerNode> m_insufficientHandlerBlock;

    // Cache for pre-calculated resource costs (optimization)
    mutable double m_cachedAethelCost;
    mutable double m_cachedChrononCost;
    mutable bool m_hasCalculatedCosts;
};

/**
 * @class ResourceInsufficiencyHandlerNode
 * @brief Node for handling insufficient resources in an EXPEND_RESOURCES block
 */
class ResourceInsufficiencyHandlerNode : public ASTNode {
public:
    /**
     * @brief Construct a resource insufficiency handler node
     * @param location The source location
     * @param errorVar Variable name to hold the error information
     * @param body The handler body
     */
    ResourceInsufficiencyHandlerNode(const SourceLocation& location, std::string errorVar,
                                     std::unique_ptr<BlockStmtNode> body)
        : ASTNode(location), m_errorVar(std::move(errorVar)), m_body(std::move(body)) {}

    void accept(ASTVisitor& visitor) const override {
        visitor.visitResourceInsufficiencyHandler(*this);
    }

    /**
     * @brief Get the error variable name
     * @return The error variable name
     */
    const std::string& getErrorVar() const { return m_errorVar; }

    /**
     * @brief Get the handler body
     * @return The handler body
     */
    const BlockStmtNode& getBody() const { return *m_body; }

private:
    std::string m_errorVar;
    std::unique_ptr<BlockStmtNode> m_body;
};

/**
 * @class TemporalLoopStmtNode
 * @brief Node for TEMPORAL_LOOP construct
 *
 * The TEMPORAL_LOOP construct is a time-variant iteration mechanism that adapts
 * to temporal conditions. It provides features for time-variant iterations,
 * conditional temporal loops, and loop optimization based on temporal metrics.
 */
class TemporalLoopStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a TEMPORAL_LOOP statement
     * @param location The source location
     * @param duration The temporal duration expression (optional)
     * @param condition The continuation condition (optional)
     * @param varianceExpr Expression controlling temporal variance (optional)
     * @param body The loop body
     */
    TemporalLoopStmtNode(const SourceLocation& location, std::unique_ptr<ExprNode> duration,
                         std::unique_ptr<ExprNode> condition,
                         std::unique_ptr<ExprNode> varianceExpr,
                         std::unique_ptr<BlockStmtNode> body)
        : StmtNode(location)
        , m_duration(std::move(duration))
        , m_condition(std::move(condition))
        , m_varianceExpr(std::move(varianceExpr))
        , m_body(std::move(body)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitTemporalLoopStmt(*this); }

    /**
     * @brief Check if the loop has a duration expression
     */
    bool hasDuration() const { return m_duration != nullptr; }

    /**
     * @brief Get the duration expression
     * @throws std::runtime_error if there is no duration expression
     */
    const ExprNode& getDuration() const {
        if (!m_duration) {
            throw std::runtime_error("TEMPORAL_LOOP has no duration expression");
        }
        return *m_duration;
    }

    /**
     * @brief Get the duration expression (mutable version)
     * @throws std::runtime_error if there is no duration expression
     */
    ExprNode& getDuration() {
        if (!m_duration) {
            throw std::runtime_error("TEMPORAL_LOOP has no duration expression");
        }
        return *m_duration;
    }

    /**
     * @brief Check if the loop has a condition expression
     */
    bool hasCondition() const { return m_condition != nullptr; }

    /**
     * @brief Get the condition expression
     * @throws std::runtime_error if there is no condition expression
     */
    const ExprNode& getCondition() const {
        if (!m_condition) {
            throw std::runtime_error("TEMPORAL_LOOP has no condition expression");
        }
        return *m_condition;
    }

    /**
     * @brief Get the condition expression (mutable version)
     * @throws std::runtime_error if there is no condition expression
     */
    ExprNode& getCondition() {
        if (!m_condition) {
            throw std::runtime_error("TEMPORAL_LOOP has no condition expression");
        }
        return *m_condition;
    }

    /**
     * @brief Check if the loop has a variance expression
     */
    bool hasVariance() const { return m_varianceExpr != nullptr; }

    /**
     * @brief Get the variance expression
     * @throws std::runtime_error if there is no variance expression
     */
    const ExprNode& getVariance() const {
        if (!m_varianceExpr) {
            throw std::runtime_error("TEMPORAL_LOOP has no variance expression");
        }
        return *m_varianceExpr;
    }

    /**
     * @brief Get the variance expression (mutable version)
     * @throws std::runtime_error if there is no variance expression
     */
    ExprNode& getVariance() {
        if (!m_varianceExpr) {
            throw std::runtime_error("TEMPORAL_LOOP has no variance expression");
        }
        return *m_varianceExpr;
    }

    /**
     * @brief Get the loop body
     */
    const BlockStmtNode& getBody() const { return *m_body; }

    /**
     * @brief Get the loop body (mutable version)
     */
    BlockStmtNode& getBody() { return *m_body; }

private:
    std::unique_ptr<ExprNode> m_duration;      // How long the loop runs temporally
    std::unique_ptr<ExprNode> m_condition;     // Condition for continuing the loop
    std::unique_ptr<ExprNode> m_varianceExpr;  // Controls temporal variance of the loop
    std::unique_ptr<BlockStmtNode> m_body;     // The loop body
};

/**
 * @class ParallelExecutionStmtNode
 * @brief Node for PARALLEL_EXECUTION construct
 *
 * The PARALLEL_EXECUTION construct enables multi-threaded temporal operations,
 * with synchronization mechanisms and resource allocation for parallel execution.
 */
class ParallelExecutionStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a PARALLEL_EXECUTION statement
     * @param location The source location
     * @param threadCount Expression specifying the number of threads to use (optional)
     * @param condition Expression evaluated in parallel across threads (optional)
     * @param body The block to execute in parallel
     * @param syncPoints Optional sync point blocks within the parallel execution
     */
    ParallelExecutionStmtNode(const SourceLocation& location, std::unique_ptr<ExprNode> threadCount,
                              std::unique_ptr<ExprNode> condition,
                              std::unique_ptr<BlockStmtNode> body,
                              std::vector<std::unique_ptr<ExprNode>> syncPoints = {})
        : StmtNode(location)
        , m_threadCount(std::move(threadCount))
        , m_condition(std::move(condition))
        , m_body(std::move(body))
        , m_syncPoints(std::move(syncPoints)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitParallelExecutionStmt(*this); }

    /**
     * @brief Check if thread count is specified
     */
    bool hasThreadCount() const { return m_threadCount != nullptr; }

    /**
     * @brief Get the thread count expression
     * @throws std::runtime_error if there is no thread count expression
     */
    const ExprNode& getThreadCount() const {
        if (!m_threadCount) {
            throw std::runtime_error("PARALLEL_EXECUTION has no thread count expression");
        }
        return *m_threadCount;
    }

    /**
     * @brief Get the thread count expression (mutable version)
     * @throws std::runtime_error if there is no thread count expression
     */
    ExprNode& getThreadCount() {
        if (!m_threadCount) {
            throw std::runtime_error("PARALLEL_EXECUTION has no thread count expression");
        }
        return *m_threadCount;
    }

    /**
     * @brief Check if condition is specified
     */
    bool hasCondition() const { return m_condition != nullptr; }

    /**
     * @brief Get the condition expression
     * @throws std::runtime_error if there is no condition expression
     */
    const ExprNode& getCondition() const {
        if (!m_condition) {
            throw std::runtime_error("PARALLEL_EXECUTION has no condition expression");
        }
        return *m_condition;
    }

    /**
     * @brief Get the condition expression (mutable version)
     * @throws std::runtime_error if there is no condition expression
     */
    ExprNode& getCondition() {
        if (!m_condition) {
            throw std::runtime_error("PARALLEL_EXECUTION has no condition expression");
        }
        return *m_condition;
    }

    /**
     * @brief Get the block body
     */
    const BlockStmtNode& getBody() const { return *m_body; }

    /**
     * @brief Get the block body (mutable version)
     */
    BlockStmtNode& getBody() { return *m_body; }

    /**
     * @brief Check if synchronization points are specified
     */
    bool hasSyncPoints() const { return !m_syncPoints.empty(); }

    /**
     * @brief Get the synchronization points
     */
    const std::vector<std::unique_ptr<ExprNode>>& getSyncPoints() const { return m_syncPoints; }

private:
    std::unique_ptr<ExprNode> m_threadCount;              // Number of threads to use
    std::unique_ptr<ExprNode> m_condition;                // Condition for parallel execution
    std::unique_ptr<BlockStmtNode> m_body;                // Body to execute in parallel
    std::vector<std::unique_ptr<ExprNode>> m_syncPoints;  // Synchronization points
};

/**
 * @class BranchTimelineStmtNode
 * @brief Node for BRANCH_TIMELINE construct
 *
 * The BRANCH_TIMELINE construct enables speculative execution through timeline branching,
 * allowing for exploration of different execution paths and merging of results.
 */
class BranchTimelineStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a BRANCH_TIMELINE statement
     * @param location The source location
     * @param branchCount Expression specifying the number of branches to create (optional)
     * @param identifier Expression for the branch identifier (optional)
     * @param mergeStrategy Expression specifying the merge strategy (optional)
     * @param body The block to execute in the branch
     */
    BranchTimelineStmtNode(const SourceLocation& location, std::unique_ptr<ExprNode> branchCount,
                           std::unique_ptr<ExprNode> identifier,
                           std::unique_ptr<ExprNode> mergeStrategy,
                           std::unique_ptr<BlockStmtNode> body)
        : StmtNode(location)
        , m_branchCount(std::move(branchCount))
        , m_identifier(std::move(identifier))
        , m_mergeStrategy(std::move(mergeStrategy))
        , m_body(std::move(body)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visitBranchTimelineStmt(*this); }

    /**
     * @brief Check if branch count is specified
     */
    bool hasBranchCount() const { return m_branchCount != nullptr; }

    /**
     * @brief Get the branch count expression
     * @throws std::runtime_error if there is no branch count expression
     */
    const ExprNode& getBranchCount() const {
        if (!m_branchCount) {
            throw std::runtime_error("BRANCH_TIMELINE has no branch count expression");
        }
        return *m_branchCount;
    }

    /**
     * @brief Get the branch count expression (mutable version)
     * @throws std::runtime_error if there is no branch count expression
     */
    ExprNode& getBranchCount() {
        if (!m_branchCount) {
            throw std::runtime_error("BRANCH_TIMELINE has no branch count expression");
        }
        return *m_branchCount;
    }

    /**
     * @brief Check if identifier is specified
     */
    bool hasIdentifier() const { return m_identifier != nullptr; }

    /**
     * @brief Get the identifier expression
     * @throws std::runtime_error if there is no identifier expression
     */
    const ExprNode& getIdentifier() const {
        if (!m_identifier) {
            throw std::runtime_error("BRANCH_TIMELINE has no identifier expression");
        }
        return *m_identifier;
    }

    /**
     * @brief Get the identifier expression (mutable version)
     * @throws std::runtime_error if there is no identifier expression
     */
    ExprNode& getIdentifier() {
        if (!m_identifier) {
            throw std::runtime_error("BRANCH_TIMELINE has no identifier expression");
        }
        return *m_identifier;
    }

    /**
     * @brief Check if merge strategy is specified
     */
    bool hasMergeStrategy() const { return m_mergeStrategy != nullptr; }

    /**
     * @brief Get the merge strategy expression
     * @throws std::runtime_error if there is no merge strategy expression
     */
    const ExprNode& getMergeStrategy() const {
        if (!m_mergeStrategy) {
            throw std::runtime_error("BRANCH_TIMELINE has no merge strategy expression");
        }
        return *m_mergeStrategy;
    }

    /**
     * @brief Get the merge strategy expression (mutable version)
     * @throws std::runtime_error if there is no merge strategy expression
     */
    ExprNode& getMergeStrategy() {
        if (!m_mergeStrategy) {
            throw std::runtime_error("BRANCH_TIMELINE has no merge strategy expression");
        }
        return *m_mergeStrategy;
    }

    /**
     * @brief Get the block body
     */
    const BlockStmtNode& getBody() const { return *m_body; }

    /**
     * @brief Get the block body (mutable version)
     */
    BlockStmtNode& getBody() { return *m_body; }

private:
    std::unique_ptr<ExprNode> m_branchCount;    // Number of branches to create
    std::unique_ptr<ExprNode> m_identifier;     // Branch identifier
    std::unique_ptr<ExprNode> m_mergeStrategy;  // Merge strategy
    std::unique_ptr<BlockStmtNode> m_body;      // Body to execute in the branch
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_AST_NODES_H