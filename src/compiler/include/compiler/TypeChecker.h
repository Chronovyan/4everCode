#ifndef CHRONOVYAN_COMPILER_TYPE_CHECKER_H
#define CHRONOVYAN_COMPILER_TYPE_CHECKER_H

#include <string>
#include <vector>

#include "compiler/AstNode.h"
#include "compiler/Diagnostic.h"
#include "compiler/Token.h"

namespace chronovyan {
namespace compiler {

/**
 * Types supported by the Chronovyan language.
 */
enum class DataType {
    UNKNOWN,
    VOID,
    BOOLEAN,
    INTEGER,
    FLOAT,
    STRING,
    CHRONON,
    AETHEL,
    ARRAY,
    MAP,
    FUNCTION,
    CUSTOM
};

/**
 * Represents a type in the Chronovyan language.
 */
class Type {
public:
    /**
     * Default constructor that creates an UNKNOWN type.
     */
    Type() : baseType(DataType::UNKNOWN) {}

    /**
     * Constructs a type with only a base type.
     *
     * @param baseType The base type of the type
     */
    explicit Type(DataType baseType) : baseType(baseType) {}

    /**
     * Constructs a type with a base type and a custom type name.
     *
     * @param baseType The base type of the type
     * @param customTypeName The custom type name of the type
     */
    Type(DataType baseType, const std::string& customTypeName)
        : baseType(baseType), customTypeName(customTypeName) {}

    /**
     * Gets the base type of this type.
     *
     * @return The base type of this type
     */
    DataType getBaseType() const { return baseType; }

    /**
     * Gets the custom type name of this type.
     *
     * @return The custom type name of this type
     */
    const std::string& getCustomTypeName() const { return customTypeName; }

    /**
     * Checks if this type is compatible with the other type.
     *
     * @param other The other type to check against
     * @return True if this type is compatible with the other type, false otherwise
     */
    bool isCompatibleWith(const Type& other) const;

    /**
     * Gets a string representation of this type.
     *
     * @return A string representation of this type
     */
    std::string toString() const;

private:
    DataType baseType;
    std::string customTypeName;
};

/**
 * TypeChecker is responsible for performing type checking during semantic analysis.
 */
class TypeChecker {
public:
    /**
     * Constructs a type checker.
     *
     * @param diagnostics The diagnostic reporter to use for reporting errors
     */
    explicit TypeChecker(DiagnosticReporter& diagnostics);

    /**
     * Gets the type of a literal expression.
     *
     * @param node The literal expression node
     * @return The type of the literal expression
     */
    Type getLiteralType(const LiteralExprNode* node) const;

    /**
     * Checks if a binary operation is valid for the given operand types.
     *
     * @param op The operator token type
     * @param leftType The type of the left operand
     * @param rightType The type of the right operand
     * @return The resulting type of the operation
     */
    Type checkBinaryOperation(TokenType op, const Type& leftType, const Type& rightType) const;

    /**
     * Checks if a unary operation is valid for the given operand type.
     *
     * @param op The operator token type
     * @param operandType The type of the operand
     * @return The resulting type of the operation
     */
    Type checkUnaryOperation(TokenType op, const Type& operandType) const;

    /**
     * Checks if an assignment operation is valid for the given types.
     *
     * @param variableType The type of the variable
     * @param valueType The type of the value being assigned
     * @return True if the assignment is valid, false otherwise
     */
    bool checkAssignment(const Type& variableType, const Type& valueType) const;

    /**
     * Gets the type of a TypeNode.
     *
     * @param node The type node
     * @return The type represented by the type node
     */
    Type getTypeFromTypeNode(const TypeNode* node) const;

private:
    DiagnosticReporter& diagnostics;

    // Predefined types
    Type voidType;
    Type booleanType;
    Type integerType;
    Type floatType;
    Type stringType;
    Type chrononType;
    Type aethelType;

    // Helper methods
    void reportTypeError(const std::string& message, const Token& token) const;
};

}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_COMPILER_TYPE_CHECKER_H