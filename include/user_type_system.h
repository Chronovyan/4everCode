#ifndef CHRONOVYAN_USER_TYPE_SYSTEM_H
#define CHRONOVYAN_USER_TYPE_SYSTEM_H

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast_nodes.h"
#include "custom_type_system.h"
#include "enum_variant.h"
#include "struct_field.h"
#include "temporal_runtime.h"
#include "value.h"
#include "variable_flag.h"

namespace chronovyan {

// Forward declarations
class UserDefinedType;
class ExprNode;
class UserTypeSystem;  // Added forward declaration

/**
 * @enum UserTypeKind
 * @brief Defines the different kinds of user types available in Chronovyan
 */
enum class UserTypeKind {
    STRUCT,  // Structure with named fields
    ENUM,    // Enumeration type
    UNION,   // Union type (can be one of several types)
    ALIAS    // Type alias
};

/**
 * @class FieldDefinition
 * @brief Represents a field definition for a user-defined type
 */
class FieldDefinition {
public:
    FieldDefinition(const std::string& name, const std::string& type,
                    const Value& defaultValue = Value(),
                    VariableModifier modifier = VariableModifier::CONF, bool required = false);

    FieldDefinition(const std::string& name, const std::string& type, VariableModifier modifier);

    FieldDefinition(const std::string& name, const std::string& type);

    /**
     * @brief Get the field name
     */
    const std::string& getName() const;

    /**
     * @brief Get the field type
     */
    const std::string& getType() const;

    /**
     * @brief Get the default value
     */
    const Value& getDefaultValue() const;

    /**
     * @brief Get the variable modifier
     */
    VariableModifier getModifier() const;

    /**
     * @brief Check if the field is required
     */
    bool isRequired() const;

    /**
     * @brief Check if the field has a default value
     */
    bool hasDefaultValue() const;

private:
    std::string m_name;
    std::string m_type;
    Value m_defaultValue;
    VariableModifier m_modifier;
    bool m_required;
    bool m_hasDefaultValue;
};

/**
 * @class EnumValueDefinition
 * @brief Represents a value definition for a user-defined enum type
 */
class EnumValueDefinition {
public:
    /**
     * @brief Create a new enum value definition
     * @param name The enum value name
     * @param value Optional explicit value (defaults to auto-increment)
     */
    EnumValueDefinition(const std::string& name, const Value& value = Value());

    /**
     * @brief Get the enum value name
     */
    const std::string& getName() const;

    /**
     * @brief Check if the enum value has an explicit value
     */
    bool hasExplicitValue() const;

    /**
     * @brief Get the explicit value
     */
    const Value& getExplicitValue() const;

private:
    std::string m_name;
    Value m_value;
    bool m_hasExplicitValue;
};

/**
 * @class MethodDefinition
 * @brief Represents a method definition for a user-defined type
 */
class MethodDefinition {
public:
    /**
     * @brief Create a new method definition
     * @param name The method name
     * @param paramTypes The parameter type names
     * @param returnType The return type name
     * @param implementation The method implementation
     * @param isStatic Whether the method is static
     */
    MethodDefinition(const std::string& name, const std::vector<std::string>& paramTypes,
                     const std::string& returnType,
                     std::function<Value(const std::vector<Value>&, std::shared_ptr<ObjectValue>)>
                         implementation,
                     bool isStatic = false);

    /**
     * @brief Get the method name
     */
    const std::string& getName() const;

    /**
     * @brief Get the parameter type names
     */
    const std::vector<std::string>& getParamTypes() const;

    /**
     * @brief Get the return type name
     */
    const std::string& getReturnType() const;

    /**
     * @brief Get the method implementation
     */
    const std::function<Value(const std::vector<Value>&, std::shared_ptr<ObjectValue>)>&
    getImplementation() const;

    /**
     * @brief Check if the method is static
     */
    bool isStatic() const;

private:
    std::string m_name;
    std::vector<std::string> m_paramTypes;
    std::string m_returnType;
    std::function<Value(const std::vector<Value>&, std::shared_ptr<ObjectValue>)> m_implementation;
    bool m_isStatic;
};

/**
 * @class TypeBuilder
 * @brief Builder for creating and registering user-defined types
 */
class TypeBuilder {
public:
    TypeBuilder(UserTypeSystem& typeSystem, const std::string& name);

    TypeBuilder& addTypeParameter(const std::string& paramName);
    TypeBuilder& asUnion();
    TypeBuilder& withTemporalSupport(bool predictive);
    TypeBuilder& addField(const std::string& fieldName, const std::string& typeName);
    TypeBuilder& addVariant(const std::string& variantName);
    TypeBuilder& addVariantWithValue(const std::string& variantName,
                                     std::unique_ptr<ExprNode> value);

    std::shared_ptr<UserDefinedType> register_();

private:
    std::shared_ptr<UserDefinedType> registerStructType();
    std::shared_ptr<UserDefinedType> registerEnumType();
    std::shared_ptr<UserDefinedType> registerUnionType();
    std::shared_ptr<UserDefinedType> registerAliasType();

    UserTypeSystem* m_typeSystem;
    std::string m_name;
    bool m_isUnion;
    bool m_temporalEnabled;
    bool m_predictiveEnabled;
    std::vector<StructField> m_fields;
    std::vector<EnumVariant> m_variants;
    std::vector<std::string> m_typeParameters;
};

/**
 * @struct InstanceState
 * @brief Represents a state of an instance at a specific cycle
 */
struct InstanceState {
    uint64_t cycle;
    std::map<std::string, Value> fields;
};

/**
 * @class UserTypeSystem
 * @brief High-level API for defining and using custom types
 */
class UserTypeSystem : public std::enable_shared_from_this<UserTypeSystem> {
public:
    /**
     * @brief Default constructor
     * Creates a UserTypeSystem with a new CustomTypeSystem instance
     */
    UserTypeSystem();

    /**
     * @brief Create a new user type system
     * @param typeSystem The underlying custom type system
     * @param runtime Optional temporal runtime for resource tracking
     */
    UserTypeSystem(std::shared_ptr<CustomTypeSystem> typeSystem,
                   std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Create a type builder with the specified kind
     * @param name The type name
     * @param kind The kind of type to create
     * @return A type builder for the new type
     */
    TypeBuilder createTypeBuilder(const std::string& name, UserTypeKind kind);

    /**
     * @brief Begin defining a new struct type
     * @param name The type name
     * @return A type builder for the new type
     */
    TypeBuilder defineStruct(const std::string& name);

    /**
     * @brief Begin defining a new enum type
     * @param name The type name
     * @return A type builder for the new type
     */
    TypeBuilder defineEnum(const std::string& name);

    /**
     * @brief Begin defining a new union type
     * @param name The type name
     * @return A type builder for the new type
     */
    TypeBuilder defineUnion(const std::string& name);

    /**
     * @brief Begin defining a new type alias
     * @param name The type name
     * @return A type builder for the new type
     */
    TypeBuilder defineAlias(const std::string& name);

    /**
     * @brief Create an instance of a user-defined type
     * @param typeName The type name
     * @param values The initial values for the instance fields
     * @return The created instance
     */
    std::shared_ptr<Value> createInstance(const std::string& typeName,
                                          const std::map<std::string, Value>& values = {});

    /**
     * @brief Create a generic type instance
     * @param baseTypeName The base generic type name
     * @param typeArgs The type arguments
     * @return The instantiated generic type name
     */
    std::string instantiateGenericType(const std::string& baseTypeName,
                                       const std::vector<std::string>& typeArgs);

    /**
     * @brief Check if a value conforms to a type
     * @param value The value to check
     * @param typeName The type name
     * @return Whether the value conforms to the type
     */
    bool isOfType(const Value& value, const std::string& typeName);

    /**
     * @brief Check if a type is registered
     * @param name The type name
     * @return Whether the type is registered
     */
    bool isTypeRegistered(const std::string& name);

    /**
     * @brief Get the underlying custom type system
     */
    std::shared_ptr<CustomTypeSystem> getTypeSystem() const;

    /**
     * @brief Get the temporal runtime
     */
    std::shared_ptr<TemporalRuntime> getRuntime() const;

    /**
     * @brief Check if an instance is valid
     * @param instance The instance to check
     * @return Whether the instance is valid
     */
    bool isValidInstance(const Value& instance) const;

    /**
     * @brief Get a field value from an instance
     * @param instance The instance
     * @param fieldName The field name
     * @return The field value
     */
    Value getInstanceField(const Value& instance, const std::string& fieldName) const;

    /**
     * @brief Check if an instance is temporal
     * @param instance The instance to check
     * @return Whether the instance is temporal
     */
    bool isInstanceTemporal(const Value& instance) const;

    /**
     * @brief Set a field value at a specific cycle
     * @param instance The instance
     * @param fieldName The field name
     * @param value The new value
     * @param cycle The cycle
     * @return Whether the operation was successful
     */
    bool setInstanceFieldAt(const Value& instance, const std::string& fieldName, const Value& value,
                            uint64_t cycle);

    /**
     * @brief Set a field value
     * @param instance The instance
     * @param fieldName The field name
     * @param value The new value
     * @return Whether the operation was successful
     */
    bool setInstanceField(const Value& instance, const std::string& fieldName, const Value& value);

    /**
     * @brief Get the history of a temporal instance
     * @param instance The instance
     * @return The history as a Value
     */
    Value getInstanceHistory(const Value& instance) const;

    /**
     * @brief Get an instance at a specific cycle
     * @param instance The instance
     * @param cycle The cycle
     * @return The instance at the specified cycle
     */
    Value getInstanceAtCycle(const Value& instance, uint64_t cycle) const;

    /**
     * @brief Get a type by name
     * @param name The type name
     * @return The type, or nullptr if not found
     */
    std::shared_ptr<UserDefinedType> getType(const std::string& name) const;

    /**
     * @brief Register a type
     * @param name The type name
     * @param type The type to register
     * @return The registered type
     */
    std::shared_ptr<UserDefinedType> registerType(const std::string& name,
                                                  std::shared_ptr<UserDefinedType> type);

private:
    std::shared_ptr<CustomTypeSystem> m_typeSystem;
    std::shared_ptr<TemporalRuntime> m_runtime;
    std::unordered_map<std::string, std::shared_ptr<UserDefinedType>> m_types;
    std::map<std::string, std::shared_ptr<ObjectValue>> m_instances;
    std::map<std::string, std::vector<InstanceState>> m_history;
    uint64_t m_instanceCount;

    /**
     * @brief Check if a type has temporal support
     * @param typeName The type name
     * @return Whether the type has temporal support
     */
    bool isTypeTemporal(const std::string& typeName) const;

    /**
     * @brief Get the type name of an instance
     * @param instance The instance
     * @return The type name
     */
    std::string getInstanceType(std::shared_ptr<ObjectValue> instance) const;
};

/**
 * @class UserDefinedType
 * @brief Represents a user-defined type in the Chronovyan type system
 */
class UserDefinedType {
public:
    // Use the same enum as in TypeBuilder but with public access
    enum class Kind {
        STRUCT,  // Structure with named fields
        ENUM,    // Enumeration type
        UNION,   // Union type (can be one of several types)
        ALIAS    // Type alias
    };

    /**
     * @brief Create a new user-defined type
     * @param name The type name
     * @param kind The kind of type
     */
    UserDefinedType(const std::string& name, Kind kind);

    /**
     * @brief Get the type name
     * @return The type name
     */
    const std::string& getName() const;

    /**
     * @brief Get the type kind
     * @return The type kind
     */
    Kind getKind() const;

    /**
     * @brief Add a field to a struct type
     * @param field The field to add
     */
    void addField(const StructField& field);

    /**
     * @brief Add a variant to an enum type
     * @param variant The variant to add
     */
    void addVariant(const EnumVariant& variant);

    /**
     * @brief Add a type parameter to a generic type
     * @param param The parameter name
     */
    void addTypeParameter(const std::string& param);

    /**
     * @brief Set the target type for an alias
     * @param target The target type name
     */
    void setAliasTarget(const std::string& target);

    /**
     * @brief Enable temporal support for this type
     * @param predictive Whether to enable predictive temporal features
     */
    void setTemporalSupport(bool predictive);

    /**
     * @brief Check if this type has temporal support
     * @return True if temporal support is enabled
     */
    bool hasTemporalSupport() const;

    /**
     * @brief Check if this type has predictive temporal support
     * @return True if predictive temporal support is enabled
     */
    bool hasPredictiveSupport() const;

    /**
     * @brief Get the fields for a struct type
     * @return Vector of struct fields
     */
    const std::vector<StructField>& getFields() const;

    /**
     * @brief Get the variants for an enum type
     * @return Vector of enum variants
     */
    const std::vector<EnumVariant>& getVariants() const;

    /**
     * @brief Get the type parameters for a generic type
     * @return Vector of parameter names
     */
    const std::vector<std::string>& getTypeParameters() const;

    /**
     * @brief Get the target type name for an alias
     * @return The target type name
     */
    const std::string& getAliasTarget() const;

private:
    std::string m_name;
    Kind m_kind;
    bool m_temporalSupport;
    bool m_predictiveSupport;
    std::vector<StructField> m_fields;
    std::vector<EnumVariant> m_variants;
    std::vector<std::string> m_typeParameters;
    std::string m_aliasTarget;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_USER_TYPE_SYSTEM_H