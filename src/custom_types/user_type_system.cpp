#include <stdexcept>

#include "../custom_type_system.h"
#include "../temporal/temporal_runtime.h"
#include "user_type_system.h"

namespace chronovyan {

// FieldDefinition implementation
FieldDefinition::FieldDefinition(const std::string& name, const std::string& type_name,
                                 VariableModifier modifier)
    : name_(name)
    , type_name_(type_name)
    , modifier_(modifier)
    , has_default_(false)
    , required_(true) {}

FieldDefinition::FieldDefinition(const std::string& name, const std::string& type_name,
                                 const Value& default_value, VariableModifier modifier,
                                 bool required)
    : name_(name)
    , type_name_(type_name)
    , modifier_(modifier)
    , has_default_(true)
    , default_value_(default_value)
    , required_(required) {}

// TypeBuilder implementation
TypeBuilder::TypeBuilder(const std::string& name, UserTypeKind kind) : name_(name), kind_(kind) {}

TypeBuilder& TypeBuilder::withField(const FieldDefinition& field) {
    fields_.push_back(field);
    return *this;
}

TypeBuilder& TypeBuilder::withTemporalSupport(bool track_history, bool allow_time_travel) {
    temporal_support_ = track_history;
    allow_time_travel_ = allow_time_travel;
    return *this;
}

TypeBuilder& TypeBuilder::asAliasFor(const std::string& target_type_name) {
    alias_target_ = target_type_name;
    return *this;
}

TypeBuilder& TypeBuilder::withEnumValue(const std::string& name, int value) {
    enum_values_[name] = value;
    return *this;
}

TypeBuilder& TypeBuilder::withUnionMember(const std::string& type_name) {
    union_members_.push_back(type_name);
    return *this;
}

bool TypeBuilder::build() {
    // In a real implementation, this would register the type with the type system
    // For now, just return success
    return true;
}

// UserTypeSystem implementation
UserTypeSystem::UserTypeSystem(std::shared_ptr<CustomTypeSystem> type_system,
                               std::shared_ptr<TemporalRuntime> runtime)
    : type_system_(type_system), runtime_(runtime) {
    // Initialize the user type system
}

TypeBuilder UserTypeSystem::createTypeBuilder(const std::string& name, UserTypeKind kind) {
    return TypeBuilder(name, kind);
}

Value UserTypeSystem::createInstance(const std::string& type_name,
                                     const std::map<std::string, Value>& initial_values) {
    // In a real implementation, this would create an instance of the specified type
    // For now, just create a placeholder object
    std::map<std::string, Value> instance_data;

    // Copy initial values
    for (const auto& pair : initial_values) {
        instance_data[pair.first] = pair.second;
    }

    // Add type information
    instance_data["__type"] = Value(type_name);
    instance_data["__kind"] = Value("INSTANCE");

    // Return as an object
    return Value(instance_data);
}

bool UserTypeSystem::isValidInstance(const Value& value) {
    // Check if the value is an object with the expected type information
    if (!value.isObject()) {
        return false;
    }

    auto obj = value.asObject();
    return obj.find("__type") != obj.end() && obj.find("__kind") != obj.end() &&
           obj["__kind"].isString() && obj["__kind"].asString() == "INSTANCE";
}

bool UserTypeSystem::isInstanceTemporal(const Value& value) {
    // Check if the instance is temporal
    if (!isValidInstance(value)) {
        return false;
    }

    auto obj = value.asObject();
    return obj.find("__temporal") != obj.end() && obj["__temporal"].isBool() &&
           obj["__temporal"].asBool();
}

Value UserTypeSystem::getInstanceField(const Value& instance, const std::string& field_name) {
    // Get a field value from an instance
    if (!isValidInstance(instance)) {
        throw std::runtime_error("Invalid instance");
    }

    auto obj = instance.asObject();
    if (obj.find(field_name) == obj.end()) {
        throw std::runtime_error("Field not found: " + field_name);
    }

    return obj[field_name];
}

bool UserTypeSystem::setInstanceField(const Value& instance, const std::string& field_name,
                                      const Value& value) {
    // Set a field value in an instance
    if (!isValidInstance(instance)) {
        return false;
    }

    // Check if the field is readonly
    auto obj = instance.asObject();
    if (obj.find("__readonly_fields") != obj.end() && obj["__readonly_fields"].isArray()) {
        auto readonly_fields = obj["__readonly_fields"].asArray();
        for (const auto& field : readonly_fields) {
            if (field.isString() && field.asString() == field_name) {
                return false;  // Cannot modify readonly field
            }
        }
    }

    // Set the field value
    obj[field_name] = value;
    return true;
}

bool UserTypeSystem::setInstanceFieldAt(const Value& instance, const std::string& field_name,
                                        const Value& value, uint64_t cycle) {
    // Set a field value in a temporal instance at a specific cycle
    if (!isInstanceTemporal(instance)) {
        return false;
    }

    // In a real implementation, this would record the field value at the specified cycle
    // For now, just update the current value
    return setInstanceField(instance, field_name, value);
}

Value UserTypeSystem::getInstanceHistory(const Value& instance) {
    // Get the history of a temporal instance
    if (!isInstanceTemporal(instance)) {
        throw std::runtime_error("Instance is not temporal");
    }

    // In a real implementation, this would return the history of the instance
    // For now, just return an empty array
    return Value(std::vector<Value>{});
}

Value UserTypeSystem::getInstanceAtCycle(const Value& instance, uint64_t cycle) {
    // Get a temporal instance state at a specific cycle
    if (!isInstanceTemporal(instance)) {
        throw std::runtime_error("Instance is not temporal");
    }

    // In a real implementation, this would return the instance state at the specified cycle
    // For now, just return the current instance
    return instance;
}

}  // namespace chronovyan