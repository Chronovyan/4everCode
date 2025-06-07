#ifndef CHRONOVYAN_CUSTOM_TYPE_SYSTEM_H
#define CHRONOVYAN_CUSTOM_TYPE_SYSTEM_H

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "source_location.h"
#include "value.h"

namespace chronovyan {

/**
 * @enum CustomTypeKind
 * @brief Defines the different kinds of custom types available in Chronovyan
 */
enum class CustomTypeKind {
    STRUCT,           // Structure with named fields
    ENUM,             // Enumeration type
    UNION,            // Union type (can be one of several types)
    ALIAS,            // Type alias
    FUNCTION_TYPE,    // Function type signature
    MODULE_TYPE,      // Module type
    INTERFACE,        // Interface type (new)
    GENERIC_INSTANCE  // Instance of a generic type (new)
};

// Helper function to convert CustomTypeKind to string
std::string CustomTypeKindToString(CustomTypeKind kind);

/**
 * @struct TypeParameter
 * @brief Represents a type parameter for a generic type
 */
struct TypeParameter {
    std::string name;
    std::optional<std::string> constraint;  // Optional type constraint

    TypeParameter(const std::string& n, const std::optional<std::string>& c = std::nullopt)
        : name(n), constraint(c) {}
};

/**
 * @struct TypeExpression
 * @brief Represents a parsed type expression
 */
struct TypeExpression {
    std::string baseName;                       // Base type name
    std::vector<TypeExpression> typeArguments;  // For generic types
    bool isArray = false;                       // For array types
    bool isNullable = false;                    // For nullable types

    TypeExpression() = default;
    explicit TypeExpression(const std::string& name) : baseName(name) {}

    std::string toString() const;
};

/**
 * @class CustomTypeDefinition
 * @brief Represents a user-defined type in Chronovyan
 */
class CustomTypeDefinition {
public:
    CustomTypeDefinition(const std::string& name, CustomTypeKind kind,
                         const std::map<std::string, Value>& properties = {},
                         const std::vector<std::string>& typeParameters = {});

    // Enhanced constructor with TypeParameter objects
    CustomTypeDefinition(const std::string& name, CustomTypeKind kind,
                         const std::map<std::string, Value>& properties,
                         const std::vector<TypeParameter>& typeParameters);

    /**
     * @brief Get the name of the type
     * @return The type name
     */
    const std::string& getName() const;

    /**
     * @brief Get the kind of custom type
     * @return The type kind
     */
    CustomTypeKind getKind() const;

    /**
     * @brief Check if the type has a specific property
     * @param name The property name
     * @return True if the property exists
     */
    bool hasProperty(const std::string& name) const;

    /**
     * @brief Get a property value
     * @param name The property name
     * @return The property value
     */
    Value getProperty(const std::string& name) const;

    /**
     * @brief Set a property value
     * @param name The property name
     * @param value The property value
     */
    void setProperty(const std::string& name, const Value& value);

    /**
     * @brief Check if the type is generic (has type parameters)
     * @return True if the type is generic
     */
    bool isGeneric() const;

    /**
     * @brief Get the type parameters for a generic type
     * @return List of type parameter names
     */
    const std::vector<std::string>& getTypeParameters() const;

    /**
     * @brief Get the enhanced type parameters with constraints
     * @return List of type parameter objects
     */
    const std::vector<TypeParameter>& getTypeParametersWithConstraints() const;

    /**
     * @brief Create a string representation of the type
     * @return String representation
     */
    std::string toString() const;

private:
    std::string m_name;
    CustomTypeKind m_kind;
    std::map<std::string, Value> m_properties;
    std::vector<std::string> m_typeParameters;
    std::vector<TypeParameter> m_typeParametersWithConstraints;
};

/**
 * @class CustomTypeInstance
 * @brief Represents an instance of a custom type with specific values
 */
class CustomTypeInstance {
public:
    CustomTypeInstance(std::shared_ptr<CustomTypeDefinition> typeDefinition,
                       const std::map<std::string, Value>& values = {});

    /**
     * @brief Get the type definition
     * @return The type definition
     */
    std::shared_ptr<CustomTypeDefinition> getTypeDefinition() const;

    /**
     * @brief Check if the instance has a field
     * @param name The field name
     * @return True if the field exists
     */
    bool hasField(const std::string& name) const;

    /**
     * @brief Get a field value
     * @param name The field name
     * @return The field value
     */
    Value getField(const std::string& name) const;

    /**
     * @brief Set a field value
     * @param name The field name
     * @param value The field value
     */
    void setField(const std::string& name, const Value& value);

    /**
     * @brief Get all fields
     * @return Map of field names to values
     */
    const std::map<std::string, Value>& getAllFields() const;

    /**
     * @brief Create a string representation of the instance
     * @return String representation
     */
    std::string toString() const;

private:
    std::shared_ptr<CustomTypeDefinition> m_typeDefinition;
    std::map<std::string, Value> m_values;
};

/**
 * @class TypeExpressionParser
 * @brief Parses type expressions like "Array<int>" or "Map<string, Object>"
 */
class TypeExpressionParser {
public:
    /**
     * @brief Parse a type expression string
     * @param expr The type expression to parse
     * @param location Source location for error reporting
     * @return The parsed type expression
     */
    static TypeExpression parse(const std::string& expr,
                                const SourceLocation& location = SourceLocation());

private:
    // Helper functions for parsing
    static TypeExpression parseTypeExpression(const std::string& expr, size_t& pos,
                                              const SourceLocation& location);
    static std::vector<TypeExpression> parseTypeArguments(const std::string& expr, size_t& pos,
                                                          const SourceLocation& location);
    static void skipWhitespace(const std::string& expr, size_t& pos);
};

/**
 * @class CustomTypeSystem
 * @brief Manages custom type definitions and provides type checking functionality
 */
class CustomTypeSystem {
public:
    CustomTypeSystem();

    /**
     * @brief Register a new type definition
     * @param typeDef The type definition to register
     * @return True if registration was successful
     */
    bool registerType(std::shared_ptr<CustomTypeDefinition> typeDef);

    /**
     * @brief Check if a type is registered
     * @param name The type name
     * @return True if the type is registered
     */
    bool isTypeRegistered(const std::string& name) const;

    /**
     * @brief Get a type definition by name
     * @param name The type name
     * @return The type definition or nullptr if not found
     */
    std::shared_ptr<CustomTypeDefinition> getTypeDefinition(const std::string& name) const;

    /**
     * @brief Create an instance of a custom type
     * @param typeName The type name
     * @param values Initial field values
     * @return The created instance
     */
    std::shared_ptr<CustomTypeInstance> createInstance(
        const std::string& typeName, const std::map<std::string, Value>& values = {});

    /**
     * @brief Check if a value conforms to a type
     * @param value The value to check
     * @param typeName The expected type name
     * @return True if the value conforms to the type
     */
    bool checkType(const Value& value, const std::string& typeName) const;

    /**
     * @brief Create a generic type instance
     * @param baseType The base generic type name
     * @param typeArgs The type arguments
     * @param location Source location for error reporting
     * @return The name of the instantiated generic type
     */
    std::string createGenericTypeInstance(const std::string& baseType,
                                          const std::vector<std::string>& typeArgs,
                                          const SourceLocation& location = SourceLocation());

    /**
     * @brief Parse a type expression and validate it
     * @param expression The type expression string
     * @param location Source location for error reporting
     * @return The validated type expression
     */
    TypeExpression parseAndValidateType(const std::string& expression,
                                        const SourceLocation& location = SourceLocation());

    /**
     * @brief Check if a type is a subtype of another
     * @param subType The potential subtype
     * @param superType The potential supertype
     * @return True if subType is a subtype of superType
     */
    bool isSubtypeOf(const std::string& subType, const std::string& superType) const;

    /**
     * @brief Get all registered type definitions
     * @return Map of type names to definitions
     */
    const std::map<std::string, std::shared_ptr<CustomTypeDefinition>>& getAllTypes() const;

    /**
     * @brief Define a built-in type
     * @param name The type name
     * @param properties The type properties
     * @return The created type definition
     */
    std::shared_ptr<CustomTypeDefinition> defineBuiltInType(
        const std::string& name, const std::map<std::string, Value>& properties = {});

private:
    std::map<std::string, std::shared_ptr<CustomTypeDefinition>> m_types;

    // Type checking functions for different kinds of types
    using TypeCheckFunction = std::function<bool(const Value&, const CustomTypeDefinition&)>;
    std::map<CustomTypeKind, TypeCheckFunction> m_typeCheckers;

    // Initialize built-in type checkers
    void initializeTypeCheckers();

    // Initialize built-in types
    void initializeBuiltInTypes();
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_CUSTOM_TYPE_SYSTEM_H