#pragma once
#ifndef CHRONOVYAN_USER_TYPE_SYSTEM_H
#define CHRONOVYAN_USER_TYPE_SYSTEM_H

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../value.h"

namespace chronovyan {

// Forward declarations
class CustomTypeSystem;
class TemporalRuntime;

/**
 * @enum UserTypeKind
 * @brief Defines the kind of user-defined type
 */
enum class UserTypeKind {
    STRUCT,  // Struct type with fields
    ENUM,    // Enumeration type
    UNION,   // Union type (tagged union)
    ALIAS    // Type alias
};

/**
 * @enum VariableModifier
 * @brief Defines modifiers for fields in user-defined types
 */
enum class VariableModifier {
    NONE,      // No special modifier
    READONLY,  // Field cannot be modified after initialization
    TEMPORAL,  // Field value is tracked across temporal cycles
    VOLATILE   // Field value is not preserved across temporal cycles
};

/**
 * @class FieldDefinition
 * @brief Represents a field definition for user-defined types
 */
class FieldDefinition {
public:
    /**
     * @brief Construct a new Field Definition object
     *
     * @param name Field name
     * @param type_name Type name
     * @param modifier Field modifier
     */
    FieldDefinition(const std::string& name, const std::string& type_name,
                    VariableModifier modifier = VariableModifier::NONE);

    /**
     * @brief Construct a new Field Definition object with default value
     *
     * @param name Field name
     * @param type_name Type name
     * @param default_value Default value for the field
     * @param modifier Field modifier
     * @param required Whether the field is required
     */
    FieldDefinition(const std::string& name, const std::string& type_name,
                    const Value& default_value, VariableModifier modifier = VariableModifier::NONE,
                    bool required = true);

    // Getters
    std::string getName() const { return name_; }
    std::string getTypeName() const { return type_name_; }
    VariableModifier getModifier() const { return modifier_; }
    bool hasDefaultValue() const { return has_default_; }
    Value getDefaultValue() const { return default_value_; }
    bool isRequired() const { return required_; }

private:
    std::string name_;
    std::string type_name_;
    VariableModifier modifier_;
    bool has_default_ = false;
    Value default_value_;
    bool required_ = true;
};

/**
 * @class TypeBuilder
 * @brief Builder class for constructing user-defined types
 */
class TypeBuilder {
public:
    /**
     * @brief Construct a new Type Builder object
     *
     * @param name Type name
     * @param kind Type kind
     */
    TypeBuilder(const std::string& name, UserTypeKind kind);

    /**
     * @brief Add a field to the type definition
     *
     * @param field Field definition
     * @return TypeBuilder& Reference to this builder for method chaining
     */
    TypeBuilder& withField(const FieldDefinition& field);

    /**
     * @brief Set temporal support for the type
     *
     * @param track_history Whether to track field history
     * @param allow_time_travel Whether to allow time travel for instances
     * @return TypeBuilder& Reference to this builder for method chaining
     */
    TypeBuilder& withTemporalSupport(bool track_history = true, bool allow_time_travel = true);

    /**
     * @brief Specify the target type for an alias
     *
     * @param target_type_name Target type name
     * @return TypeBuilder& Reference to this builder for method chaining
     */
    TypeBuilder& asAliasFor(const std::string& target_type_name);

    /**
     * @brief Add an enum value
     *
     * @param name Enum value name
     * @param value Integer value
     * @return TypeBuilder& Reference to this builder for method chaining
     */
    TypeBuilder& withEnumValue(const std::string& name, int value);

    /**
     * @brief Add a union member type
     *
     * @param type_name Member type name
     * @return TypeBuilder& Reference to this builder for method chaining
     */
    TypeBuilder& withUnionMember(const std::string& type_name);

    /**
     * @brief Build the type definition and register it with the type system
     *
     * @return bool Whether the type was successfully registered
     */
    bool build();

    // Getters
    std::string getName() const { return name_; }
    UserTypeKind getKind() const { return kind_; }

private:
    std::string name_;
    UserTypeKind kind_;
    std::vector<FieldDefinition> fields_;
    std::map<std::string, int> enum_values_;
    std::vector<std::string> union_members_;
    std::string alias_target_;
    bool temporal_support_ = false;
    bool allow_time_travel_ = false;

    // Reference to the type system where the type will be registered
    std::weak_ptr<CustomTypeSystem> type_system_;
};

/**
 * @class UserTypeSystem
 * @brief System for managing user-defined types in the Chronovyan language
 */
class UserTypeSystem {
public:
    /**
     * @brief Construct a new User Type System object
     *
     * @param type_system Base type system
     * @param runtime Temporal runtime
     */
    UserTypeSystem(std::shared_ptr<CustomTypeSystem> type_system,
                   std::shared_ptr<TemporalRuntime> runtime);

    /**
     * @brief Create a new type builder
     *
     * @param name Type name
     * @param kind Type kind
     * @return TypeBuilder Type builder instance
     */
    TypeBuilder createTypeBuilder(const std::string& name, UserTypeKind kind);

    /**
     * @brief Create a new instance of a user-defined type
     *
     * @param type_name Type name
     * @param initial_values Initial field values (for structs)
     * @return Value Instance value
     */
    Value createInstance(const std::string& type_name,
                         const std::map<std::string, Value>& initial_values = {});

    /**
     * @brief Check if a value is a valid user-defined type instance
     *
     * @param value Value to check
     * @return bool Whether the value is a valid instance
     */
    bool isValidInstance(const Value& value);

    /**
     * @brief Check if a user-defined type instance is temporal
     *
     * @param value Instance value
     * @return bool Whether the instance is temporal
     */
    bool isInstanceTemporal(const Value& value);

    /**
     * @brief Get a field value from a user-defined type instance
     *
     * @param instance Instance value
     * @param field_name Field name
     * @return Value Field value
     */
    Value getInstanceField(const Value& instance, const std::string& field_name);

    /**
     * @brief Set a field value in a user-defined type instance
     *
     * @param instance Instance value
     * @param field_name Field name
     * @param value New field value
     * @return bool Whether the field was successfully set
     */
    bool setInstanceField(const Value& instance, const std::string& field_name, const Value& value);

    /**
     * @brief Set a field value in a temporal instance at a specific cycle
     *
     * @param instance Instance value
     * @param field_name Field name
     * @param value New field value
     * @param cycle Temporal cycle
     * @return bool Whether the field was successfully set
     */
    bool setInstanceFieldAt(const Value& instance, const std::string& field_name,
                            const Value& value, uint64_t cycle);

    /**
     * @brief Get the history of a temporal instance
     *
     * @param instance Instance value
     * @return Value History as a Chronovyan value (typically an array of structs)
     */
    Value getInstanceHistory(const Value& instance);

    /**
     * @brief Get a temporal instance state at a specific cycle
     *
     * @param instance Instance value
     * @param cycle Temporal cycle
     * @return Value Instance state at the specified cycle
     */
    Value getInstanceAtCycle(const Value& instance, uint64_t cycle);

private:
    std::shared_ptr<CustomTypeSystem> type_system_;
    std::shared_ptr<TemporalRuntime> runtime_;

    // Maps for tracking registered types and instances
    std::unordered_map<std::string, UserTypeKind> registered_types_;
    std::unordered_map<std::string, bool> temporal_types_;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_USER_TYPE_SYSTEM_H