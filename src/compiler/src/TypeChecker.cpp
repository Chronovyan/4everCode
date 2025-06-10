#include <cassert>
#include <sstream>

#include "compiler/TypeChecker.h"

namespace chronovyan {
namespace compiler {

// Type implementation

bool Type::isCompatibleWith(const Type &other) const {
  // Same types are always compatible
  if (baseType == other.baseType) {
    // For custom types, check the name
    if (baseType == DataType::CUSTOM) {
      return customTypeName == other.customTypeName;
    }
    return true;
  }

  // Special rules for numeric types
  if ((baseType == DataType::INTEGER && other.baseType == DataType::FLOAT) ||
      (baseType == DataType::FLOAT && other.baseType == DataType::INTEGER)) {
    return true;
  }

  // Unknown is compatible with anything
  if (baseType == DataType::UNKNOWN || other.baseType == DataType::UNKNOWN) {
    return true;
  }

  // Otherwise, types are not compatible
  return false;
}

std::string Type::toString() const {
  switch (baseType) {
  case DataType::UNKNOWN:
    return "unknown";
  case DataType::VOID:
    return "void";
  case DataType::BOOLEAN:
    return "boolean";
  case DataType::INTEGER:
    return "int";
  case DataType::FLOAT:
    return "float";
  case DataType::STRING:
    return "string";
  case DataType::CHRONON:
    return "chronon";
  case DataType::AETHEL:
    return "aethel";
  case DataType::ARRAY:
    return "array";
  case DataType::MAP:
    return "map";
  case DataType::FUNCTION:
    return "function";
  case DataType::CUSTOM:
    return customTypeName;
  default:
    return "unknown";
  }
}

// TypeChecker implementation

TypeChecker::TypeChecker(DiagnosticReporter &diagnostics)
    : diagnostics(diagnostics), voidType(DataType::VOID),
      booleanType(DataType::BOOLEAN), integerType(DataType::INTEGER),
      floatType(DataType::FLOAT), stringType(DataType::STRING),
      chrononType(DataType::CHRONON), aethelType(DataType::AETHEL) {}

Type TypeChecker::getLiteralType(const LiteralExprNode *node) const {
  if (!node->getToken().hasValue()) {
    return Type(DataType::UNKNOWN);
  }

  const auto &value = node->getValue();

  if (std::holds_alternative<int>(value)) {
    return integerType;
  } else if (std::holds_alternative<double>(value)) {
    return floatType;
  } else if (std::holds_alternative<std::string>(value)) {
    return stringType;
  } else if (std::holds_alternative<bool>(value)) {
    return booleanType;
  } else {
    return Type(DataType::UNKNOWN);
  }
}

Type TypeChecker::checkBinaryOperation(TokenType op, const Type &leftType,
                                       const Type &rightType) const {
  // Handle arithmetic operators
  if (op == TokenType::PLUS || op == TokenType::MINUS ||
      op == TokenType::STAR || op == TokenType::SLASH ||
      op == TokenType::PERCENT) {
    // String concatenation
    if (op == TokenType::PLUS &&
        (leftType.getBaseType() == DataType::STRING ||
         rightType.getBaseType() == DataType::STRING)) {
      return stringType;
    }

    // Numeric operations
    if ((leftType.getBaseType() == DataType::INTEGER ||
         leftType.getBaseType() == DataType::FLOAT) &&
        (rightType.getBaseType() == DataType::INTEGER ||
         rightType.getBaseType() == DataType::FLOAT)) {
      // If either operand is a float, the result is a float
      if (leftType.getBaseType() == DataType::FLOAT ||
          rightType.getBaseType() == DataType::FLOAT) {
        return floatType;
      } else {
        return integerType;
      }
    }

    // Invalid operand types
    return Type(DataType::UNKNOWN);
  }

  // Handle comparison operators
  if (op == TokenType::EQUAL_EQUAL || op == TokenType::BANG_EQUAL ||
      op == TokenType::LESS || op == TokenType::LESS_EQUAL ||
      op == TokenType::GREATER || op == TokenType::GREATER_EQUAL) {
    // Types must be compatible for comparison
    if (leftType.isCompatibleWith(rightType)) {
      return booleanType;
    }

    // Invalid operand types
    return Type(DataType::UNKNOWN);
  }

  // Handle logical operators
  if (op == TokenType::AND || op == TokenType::OR) {
    if (leftType.getBaseType() == DataType::BOOLEAN &&
        rightType.getBaseType() == DataType::BOOLEAN) {
      return booleanType;
    }

    // Invalid operand types
    return Type(DataType::UNKNOWN);
  }

  // Unknown operator
  return Type(DataType::UNKNOWN);
}

Type TypeChecker::checkUnaryOperation(TokenType op,
                                      const Type &operandType) const {
  // Handle negation
  if (op == TokenType::MINUS) {
    if (operandType.getBaseType() == DataType::INTEGER) {
      return integerType;
    } else if (operandType.getBaseType() == DataType::FLOAT) {
      return floatType;
    }

    // Invalid operand type
    return Type(DataType::UNKNOWN);
  }

  // Handle logical NOT
  if (op == TokenType::BANG) {
    if (operandType.getBaseType() == DataType::BOOLEAN) {
      return booleanType;
    }

    // Invalid operand type
    return Type(DataType::UNKNOWN);
  }

  // Unknown operator
  return Type(DataType::UNKNOWN);
}

bool TypeChecker::checkAssignment(const Type &variableType,
                                  const Type &valueType) const {
  return variableType.isCompatibleWith(valueType);
}

Type TypeChecker::getTypeFromTypeNode(const TypeNode *node) const {
  if (!node) {
    return Type(DataType::UNKNOWN);
  }

  if (auto basicType = dynamic_cast<const BasicTypeNode *>(node)) {
    const std::string &typeName = basicType->getToken().getLexeme();

    if (typeName == "void") {
      return voidType;
    } else if (typeName == "boolean") {
      return booleanType;
    } else if (typeName == "int" || typeName == "integer") {
      return integerType;
    } else if (typeName == "float") {
      return floatType;
    } else if (typeName == "string") {
      return stringType;
    } else if (typeName == "chronon") {
      return chrononType;
    } else if (typeName == "aethel") {
      return aethelType;
    } else {
      // Assume it's a custom type
      return Type(DataType::CUSTOM, typeName);
    }
  }

  // For array, map, and function types, we would need to handle them separately
  // For now, return unknown
  return Type(DataType::UNKNOWN);
}

void TypeChecker::reportTypeError(const std::string &message,
                                  const Token &token) const {
  diagnostics.addError(token, message);
}

} // namespace compiler
} // namespace chronovyan