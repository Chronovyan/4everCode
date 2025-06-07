#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>

#include "../include/error_handler.h"
#include "../include/user_type_system.h"
#include "enum_variant.h"
#include "struct_field.h"

namespace chronovyan {

// Define the InstanceData class
class InstanceData {
private:
    std::shared_ptr<UserDefinedType> m_type;
    std::map<std::string, Value> m_fields;

public:
    InstanceData(std::shared_ptr<UserDefinedType> type);
    std::shared_ptr<UserDefinedType> getType() const;
    void setField(const std::string& fieldName, const Value& value);
    Value getField(const std::string& fieldName) const;
    bool hasField(const std::string& fieldName) const;
};

// ===== FieldDefinition Implementation =====

FieldDefinition::FieldDefinition(const std::string& name, const std::string& type,
                                 VariableModifier modifier)
    : m_name(name)
    , m_type(type)
    , m_modifier(modifier)
    , m_hasDefaultValue(false)
    , m_required(true) {}

FieldDefinition::FieldDefinition(const std::string& name, const std::string& type,
                                 const Value& defaultValue, VariableModifier modifier,
                                 bool required)
    : m_name(name)
    , m_type(type)
    , m_defaultValue(defaultValue)
    , m_modifier(modifier)
    , m_hasDefaultValue(true)
    , m_required(required) {}

const std::string& FieldDefinition::getName() const { return m_name; }

const std::string& FieldDefinition::getType() const { return m_type; }

const Value& FieldDefinition::getDefaultValue() const { return m_defaultValue; }

VariableModifier FieldDefinition::getModifier() const { return m_modifier; }

bool FieldDefinition::isRequired() const { return m_required; }

bool FieldDefinition::hasDefaultValue() const { return m_hasDefaultValue; }

// ===== EnumValueDefinition Implementation =====

EnumValueDefinition::EnumValueDefinition(const std::string& name, const Value& value)
    : m_name(name), m_value(value), m_hasExplicitValue(!value.isNil()) {}

const std::string& EnumValueDefinition::getName() const { return m_name; }

bool EnumValueDefinition::hasExplicitValue() const { return m_hasExplicitValue; }

const Value& EnumValueDefinition::getExplicitValue() const { return m_value; }

// ===== MethodDefinition Implementation =====

MethodDefinition::MethodDefinition(
    const std::string& name, const std::vector<std::string>& paramTypes,
    const std::string& returnType,
    std::function<Value(const std::vector<Value>&, std::shared_ptr<ObjectValue>)> implementation,
    bool isStatic)
    : m_name(name)
    , m_paramTypes(paramTypes)
    , m_returnType(returnType)
    , m_implementation(implementation)
    , m_isStatic(isStatic) {}

const std::string& MethodDefinition::getName() const { return m_name; }

const std::vector<std::string>& MethodDefinition::getParamTypes() const { return m_paramTypes; }

const std::string& MethodDefinition::getReturnType() const { return m_returnType; }

const std::function<Value(const std::vector<Value>&, std::shared_ptr<ObjectValue>)>&
MethodDefinition::getImplementation() const {
    return m_implementation;
}

bool MethodDefinition::isStatic() const { return m_isStatic; }

// ===== TypeBuilder Implementation =====

TypeBuilder::TypeBuilder(UserTypeSystem& typeSystem, const std::string& name)
    : m_typeSystem(&typeSystem)
    , m_name(name)
    , m_isUnion(false)
    , m_temporalEnabled(false)
    , m_predictiveEnabled(false) {}

TypeBuilder& TypeBuilder::addTypeParameter(const std::string& paramName) {
    m_typeParameters.push_back(paramName);
    return *this;
}

TypeBuilder& TypeBuilder::asUnion() {
    m_isUnion = true;
    return *this;
}

TypeBuilder& TypeBuilder::withTemporalSupport(bool predictive) {
    m_temporalEnabled = true;
    m_predictiveEnabled = predictive;
    return *this;
}

TypeBuilder& TypeBuilder::addField(const std::string& fieldName, const std::string& typeName) {
    m_fields.emplace_back(fieldName, typeName);
    return *this;
}

TypeBuilder& TypeBuilder::addVariant(const std::string& variantName) {
    m_variants.emplace_back(variantName);
    return *this;
}

TypeBuilder& TypeBuilder::addVariantWithValue(const std::string& variantName,
                                              std::unique_ptr<ExprNode> value) {
    m_variants.emplace_back(variantName, std::move(value));
    return *this;
}

std::shared_ptr<UserDefinedType> TypeBuilder::register_() {
    if (m_isUnion && !m_typeParameters.empty()) {
        // Union type with type parameters
        return registerUnionType();
    } else if (!m_typeParameters.empty()) {
        // Generic type
        return registerAliasType();
    } else if (!m_variants.empty()) {
        // Enum type
        return registerEnumType();
    } else {
        // Struct type
        return registerStructType();
    }
}

std::shared_ptr<UserDefinedType> TypeBuilder::registerStructType() {
    // Create a new struct type
    auto type = std::make_shared<UserDefinedType>(m_name, UserDefinedType::Kind::STRUCT);

    // Add fields
    for (const auto& field : m_fields) {
        type->addField(field);
    }

    // Set temporal support
    if (m_temporalEnabled) {
        type->setTemporalSupport(m_predictiveEnabled);
    }

    // Register the type
    m_typeSystem->registerType(m_name, type);

    return type;
}

std::shared_ptr<UserDefinedType> TypeBuilder::registerEnumType() {
    // Create a new enum type
    auto type = std::make_shared<UserDefinedType>(m_name, UserDefinedType::Kind::ENUM);

    // Add variants
    for (const auto& variant : m_variants) {
        type->addVariant(variant);
    }

    // Set temporal support
    if (m_temporalEnabled) {
        type->setTemporalSupport(m_predictiveEnabled);
    }

    // Register the type
    m_typeSystem->registerType(m_name, type);

    return type;
}

std::shared_ptr<UserDefinedType> TypeBuilder::registerUnionType() {
    // Create a new union type
    auto type = std::make_shared<UserDefinedType>(m_name, UserDefinedType::Kind::UNION);

    // Add type parameters
    for (const auto& param : m_typeParameters) {
        type->addTypeParameter(param);
    }

    // Set temporal support
    if (m_temporalEnabled) {
        type->setTemporalSupport(m_predictiveEnabled);
    }

    // Register the type
    m_typeSystem->registerType(m_name, type);

    return type;
}

std::shared_ptr<UserDefinedType> TypeBuilder::registerAliasType() {
    // Create a new alias type
    auto type = std::make_shared<UserDefinedType>(m_name, UserDefinedType::Kind::ALIAS);
    type->setAliasTarget(m_name);

    // Register the type
    m_typeSystem->registerType(m_name, type);

    return type;
}

// ===== UserTypeSystem Implementation =====

UserTypeSystem::UserTypeSystem()
    : m_typeSystem(std::make_shared<CustomTypeSystem>()), m_runtime(nullptr), m_instanceCount(0) {}

UserTypeSystem::UserTypeSystem(std::shared_ptr<CustomTypeSystem> typeSystem,
                               std::shared_ptr<TemporalRuntime> runtime)
    : m_typeSystem(typeSystem), m_runtime(runtime), m_instanceCount(0) {}

TypeBuilder UserTypeSystem::createTypeBuilder(const std::string& name, UserTypeKind kind) {
    return TypeBuilder(*this, name);
}

TypeBuilder UserTypeSystem::defineStruct(const std::string& name) {
    return createTypeBuilder(name, UserTypeKind::STRUCT);
}

TypeBuilder UserTypeSystem::defineEnum(const std::string& name) {
    return createTypeBuilder(name, UserTypeKind::ENUM);
}

TypeBuilder UserTypeSystem::defineUnion(const std::string& name) {
    return createTypeBuilder(name, UserTypeKind::UNION);
}

TypeBuilder UserTypeSystem::defineAlias(const std::string& name) {
    return createTypeBuilder(name, UserTypeKind::ALIAS);
}

std::shared_ptr<Value> UserTypeSystem::createInstance(const std::string& typeName,
                                                      const std::map<std::string, Value>& values) {
    auto typePtr = getType(typeName);
    if (!typePtr) {
        return nullptr;
    }

    // Create a new instance...
    // Implementation goes here
    return std::make_shared<Value>(Value());
}

bool UserTypeSystem::isOfType(const Value& value, const std::string& typeName) {
    // Check if the value is of the specified type
    return false;
}

bool UserTypeSystem::isTypeRegistered(const std::string& name) {
    return m_types.find(name) != m_types.end();
}

std::shared_ptr<UserDefinedType> UserTypeSystem::getType(const std::string& name) const {
    auto it = m_types.find(name);
    if (it == m_types.end()) {
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<UserDefinedType> UserTypeSystem::registerType(
    const std::string& name, std::shared_ptr<UserDefinedType> type) {
    if (!type) {
        // Error handling for null type
        return nullptr;
    }
    m_types[name] = type;
    return type;
}

bool UserTypeSystem::isValidInstance(const Value& instance) const {
    // Implementation
    return false;
}

Value UserTypeSystem::getInstanceField(const Value& instance, const std::string& fieldName) const {
    // Implementation
    return Value();
}

bool UserTypeSystem::isInstanceTemporal(const Value& instance) const {
    // Implementation
    return false;
}

bool UserTypeSystem::setInstanceFieldAt(const Value& instance, const std::string& fieldName,
                                        const Value& value, uint64_t cycle) {
    // Implementation
    return false;
}

bool UserTypeSystem::setInstanceField(const Value& instance, const std::string& fieldName,
                                      const Value& value) {
    // Implementation
    return false;
}

Value UserTypeSystem::getInstanceHistory(const Value& instance) const {
    // Implementation
    return Value();
}

Value UserTypeSystem::getInstanceAtCycle(const Value& instance, uint64_t cycle) const {
    // Implementation
    return Value();
}

bool UserTypeSystem::isTypeTemporal(const std::string& typeName) const {
    // Implementation of isTypeTemporal method
    auto type = getType(typeName);
    if (!type) {
        return false;
    }
    return type->hasTemporalSupport();
}

// ===== UserDefinedType Implementation =====

UserDefinedType::UserDefinedType(const std::string& name, Kind kind)
    : m_name(name), m_kind(kind), m_temporalSupport(false), m_predictiveSupport(false) {}

const std::string& UserDefinedType::getName() const { return m_name; }

UserDefinedType::Kind UserDefinedType::getKind() const { return m_kind; }

void UserDefinedType::addField(const StructField& field) { m_fields.push_back(field); }

void UserDefinedType::addVariant(const EnumVariant& variant) { m_variants.push_back(variant); }

void UserDefinedType::addTypeParameter(const std::string& param) {
    m_typeParameters.push_back(param);
}

void UserDefinedType::setAliasTarget(const std::string& target) { m_aliasTarget = target; }

void UserDefinedType::setTemporalSupport(bool predictive) {
    m_temporalSupport = true;
    m_predictiveSupport = predictive;
}

bool UserDefinedType::hasTemporalSupport() const { return m_temporalSupport; }

bool UserDefinedType::hasPredictiveSupport() const { return m_predictiveSupport; }

const std::vector<StructField>& UserDefinedType::getFields() const { return m_fields; }

const std::vector<EnumVariant>& UserDefinedType::getVariants() const { return m_variants; }

const std::vector<std::string>& UserDefinedType::getTypeParameters() const {
    return m_typeParameters;
}

const std::string& UserDefinedType::getAliasTarget() const { return m_aliasTarget; }

// InstanceData Implementation
InstanceData::InstanceData(std::shared_ptr<UserDefinedType> type) : m_type(type) {}

std::shared_ptr<UserDefinedType> InstanceData::getType() const { return m_type; }

void InstanceData::setField(const std::string& fieldName, const Value& value) {
    m_fields[fieldName] = value;
}

Value InstanceData::getField(const std::string& fieldName) const {
    auto it = m_fields.find(fieldName);
    if (it != m_fields.end()) {
        return it->second;
    }
    return Value();  // Return nil value if field not found
}

bool InstanceData::hasField(const std::string& fieldName) const {
    return m_fields.find(fieldName) != m_fields.end();
}

}  // namespace chronovyan