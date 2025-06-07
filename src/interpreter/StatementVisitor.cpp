#include <functional>

#include "callable.h"
#include "custom_type_system.h"
#include "environment.h"
#include "error_handler.h"
#include "function.h"
#include "interpreter.h"
#include "interpreter/StatementVisitor.h"
#include "native_function.h"
#include "temporal_runtime.h"

namespace chronovyan {

StatementVisitor::StatementVisitor(Interpreter& interpreter) : m_interpreter(interpreter) {}

void StatementVisitor::execute(const StmtNode& stmt) { stmt.accept(*this); }

bool StatementVisitor::isReturning() const { return m_isReturning; }

bool StatementVisitor::isBreaking() const { return m_isBreaking; }

bool StatementVisitor::isContinuing() const { return m_isContinuing; }

Value StatementVisitor::getReturnValue() const { return m_returnValue; }

void StatementVisitor::clearControlFlowFlags() {
    m_isReturning = false;
    m_isBreaking = false;
    m_isContinuing = false;
    m_returnValue = Value();
}

void StatementVisitor::setReturning(bool flag, const Value& value) {
    m_isReturning = flag;
    if (flag) {
        m_returnValue = value;
    }
}

void StatementVisitor::setBreaking(bool flag) { m_isBreaking = flag; }

void StatementVisitor::setContinuing(bool flag) { m_isContinuing = flag; }

void StatementVisitor::visitExprStmt(const ExprStmtNode& stmt) {
    // Evaluate the expression and discard the result
    m_interpreter.evaluate(stmt.getExpression());
}

void StatementVisitor::visitBlockStmt(const BlockStmtNode& stmt) {
    // Create a new environment for the block
    auto blockEnv = std::make_shared<Environment>(m_interpreter.getEnvironment());

    // Execute the block with the new environment
    executeBlock(stmt, blockEnv);
}

void StatementVisitor::executeBlock(const BlockStmtNode& block,
                                    std::shared_ptr<Environment> environment) {
    // Save the current environment
    std::shared_ptr<Environment> previous = m_interpreter.getEnvironment();

    try {
        // Set the new environment
        m_interpreter.setEnvironment(environment);

        // Execute each statement in the block
        for (const auto& statement : block.getStatements()) {
            execute(*statement);

            // Check for control flow changes
            if (m_isReturning || m_isBreaking || m_isContinuing) {
                break;
            }
        }
    } catch (...) {
        // Restore the previous environment even if an exception occurs
        m_interpreter.setEnvironment(previous);
        throw;
    }

    // Restore the previous environment
    m_interpreter.setEnvironment(previous);
}

void StatementVisitor::visitVariableDeclStmt(const VariableDeclStmtNode& stmt) {
    // Evaluate the initializer if present
    Value value;
    if (stmt.hasInitializer()) {
        value = m_interpreter.evaluate(stmt.getInitializer());
    } else {
        // Default value based on type
        // This could be enhanced based on the type information
        value = Value();
    }

    // Add any relevant metadata based on variable modifier and flags
    if (stmt.getModifier() == VariableModifier::CONF) {
        value.addFlaggedMetadata("CONF", true);
    } else if (stmt.getModifier() == VariableModifier::REB) {
        value.addFlaggedMetadata("REB", true);
    }

    // Add any additional flags as metadata
    for (const auto& flag : stmt.getFlags()) {
        value.addFlaggedMetadata(variableFlagToString(flag), true);
    }

    // Define the variable in the current environment
    m_interpreter.getEnvironment()->define(stmt.getName(), value);
}

void StatementVisitor::visitIfStmt(const IfStmtNode& stmt) {
    // Evaluate the condition
    Value condition = m_interpreter.evaluate(stmt.getCondition());

    // Execute the appropriate branch
    if (condition.isTruthy()) {
        execute(stmt.getThenBranch());
    } else if (stmt.hasElseBranch()) {
        execute(stmt.getElseBranch());
    }
}

void StatementVisitor::visitFunctionDeclStmt(const FunctionDeclStmtNode& stmt) {
    // Create a function object
    auto function =
        std::make_shared<Function>(stmt.getName(), stmt.getBody(), m_interpreter.getEnvironment());

    // Define the function in the current environment
    m_interpreter.getEnvironment()->define(stmt.getName(), Value(function));
}

void StatementVisitor::visitReturnStmt(const ReturnStmtNode& stmt) {
    // Evaluate the return value if present
    Value value;
    if (stmt.hasValue()) {
        value = m_interpreter.evaluate(stmt.getValue());
    }

    // Set the return flag and value
    setReturning(true, value);
}

void StatementVisitor::visitTypeDefStmt(const TypeDefStmtNode& stmt) {
    // Get the type system
    auto typeSystem = m_interpreter.getTypeSystem();

    // Create a type definition based on the kind of type
    if (stmt.isStruct()) {
        // Create a struct type
        auto builder = typeSystem->createTypeBuilder(stmt.getName(), CustomTypeKind::STRUCT);

        // Add fields
        for (const auto& field : stmt.getFields()) {
            builder.addField(field.name, field.type);
        }

        // Register the type
        try {
            builder.register_();
        } catch (const std::exception& e) {
            throw RuntimeError("Failed to define struct type '" + stmt.getName() + "': " + e.what(),
                               stmt.getLocation());
        }
    } else if (stmt.isEnum()) {
        // Create an enum type
        auto builder = typeSystem->createTypeBuilder(stmt.getName(), CustomTypeKind::ENUM);

        // Add variants
        for (const auto& variant : stmt.getVariants()) {
            builder.addEnumVariant(variant.name, variant.value);
        }

        // Register the type
        try {
            builder.register_();
        } catch (const std::exception& e) {
            throw RuntimeError("Failed to define enum type '" + stmt.getName() + "': " + e.what(),
                               stmt.getLocation());
        }
    } else if (stmt.isUnion()) {
        // Create a union type
        auto builder = typeSystem->createTypeBuilder(stmt.getName(), CustomTypeKind::UNION);

        // Add member types
        for (const auto& type : stmt.getUnionTypes()) {
            builder.addUnionMember(type->getName());
        }

        // Register the type
        try {
            builder.register_();
        } catch (const std::exception& e) {
            throw RuntimeError("Failed to define union type '" + stmt.getName() + "': " + e.what(),
                               stmt.getLocation());
        }
    } else if (stmt.isAlias()) {
        // Create a type alias
        auto builder = typeSystem->createTypeBuilder(stmt.getName(), CustomTypeKind::ALIAS);

        // Set the target type
        builder.asAliasFor(stmt.getAliasedType()->getName());

        // Register the type
        try {
            builder.register_();
        } catch (const std::exception& e) {
            throw RuntimeError("Failed to define type alias '" + stmt.getName() + "': " + e.what(),
                               stmt.getLocation());
        }
    }
}

}  // namespace chronovyan