#include <algorithm>
#include <iostream>
#include <sstream>

#include "custom_type_system.h"
#include "error_handler.h"

namespace chronovyan {

// Helper function to convert CustomTypeKind to string
std::string CustomTypeKindToString(CustomTypeKind kind) {
    switch (kind) {
        case CustomTypeKind::STRUCT:
            return "struct";
        case CustomTypeKind::ENUM:
            return "enum";
        case CustomTypeKind::UNION:
            return "union";
        case CustomTypeKind::ALIAS:
            return "alias";
        case CustomTypeKind::FUNCTION_TYPE:
            return "function";
        case CustomTypeKind::MODULE_TYPE:
            return "module";
        case CustomTypeKind::INTERFACE:
            return "interface";
        case CustomTypeKind::GENERIC_INSTANCE:
            return "generic_instance";
        default:
            return "unknown";
    }
}

// TypeExpression implementation
std::string TypeExpression::toString() const {
    std::stringstream ss;
    ss << baseName;

    if (!typeArguments.empty()) {
        ss << "<";
        for (size_t i = 0; i < typeArguments.size(); ++i) {
            if (i > 0) {
                ss << ", ";
            }
            ss << typeArguments[i].toString();
        }
        ss << ">";
    }

    if (isArray) {
        ss << "[]";
    }

    if (isNullable) {
        ss << "?";
    }

    return ss.str();
}

// TypeExpressionParser implementation
TypeExpression TypeExpressionParser::parse(const std::string& expr,
                                           const SourceLocation& location) {
    size_t pos = 0;
    skipWhitespace(expr, pos);

    if (pos >= expr.size()) {
        ErrorHandler::getInstance().reportError(location, "Empty type expression");
        return TypeExpression("Any");  // Default to Any type
    }

    return parseTypeExpression(expr, pos, location);
}

TypeExpression TypeExpressionParser::parseTypeExpression(const std::string& expr, size_t& pos,
                                                         const SourceLocation& location) {
    TypeExpression result;

    // Parse base type name
    size_t start = pos;
    while (pos < expr.size() && (std::isalnum(expr[pos]) || expr[pos] == '_')) {
        pos++;
    }

    if (start == pos) {
        ErrorHandler::getInstance().reportError(location, "Expected type name in type expression");
        return TypeExpression("Any");  // Default to Any type
    }

    result.baseName = expr.substr(start, pos - start);
    skipWhitespace(expr, pos);

    // Parse type arguments if present
    if (pos < expr.size() && expr[pos] == '<') {
        pos++;  // Skip '<'
        result.typeArguments = parseTypeArguments(expr, pos, location);
        skipWhitespace(expr, pos);
    }

    // Parse array suffix if present
    if (pos + 1 < expr.size() && expr[pos] == '[' && expr[pos + 1] == ']') {
        result.isArray = true;
        pos += 2;  // Skip '[]'
        skipWhitespace(expr, pos);
    }

    // Parse nullable suffix if present
    if (pos < expr.size() && expr[pos] == '?') {
        result.isNullable = true;
        pos++;  // Skip '?'
        skipWhitespace(expr, pos);
    }

    return result;
}

std::vector<TypeExpression> TypeExpressionParser::parseTypeArguments(
    const std::string& expr, size_t& pos, const SourceLocation& location) {
    std::vector<TypeExpression> arguments;
    skipWhitespace(expr, pos);

    while (pos < expr.size() && expr[pos] != '>') {
        arguments.push_back(parseTypeExpression(expr, pos, location));
        skipWhitespace(expr, pos);

        if (pos < expr.size() && expr[pos] == ',') {
            pos++;  // Skip ','
            skipWhitespace(expr, pos);
        } else {
            break;
        }
    }

    if (pos >= expr.size() || expr[pos] != '>') {
        ErrorHandler::getInstance().reportError(location, "Expected '>' to close type arguments");
    } else {
        pos++;  // Skip '>'
    }

    return arguments;
}

void TypeExpressionParser::skipWhitespace(const std::string& expr, size_t& pos) {
    while (pos < expr.size() && std::isspace(expr[pos])) {
        pos++;
    }
}

// CustomTypeDefinition implementation
CustomTypeDefinition::CustomTypeDefinition(const std::string& name, CustomTypeKind kind,
                                           const std::map<std::string, Value>& properties,
                                           const std::vector<std::string>& typeParameters)
    : m_name(name), m_kind(kind), m_properties(properties), m_typeParameters(typeParameters) {
    // Convert simple type parameters to TypeParameter objects
    for (const auto& param : typeParameters) {
        m_typeParametersWithConstraints.emplace_back(param);
    }
}

CustomTypeDefinition::CustomTypeDefinition(const std::string& name, CustomTypeKind kind,
                                           const std::map<std::string, Value>& properties,
                                           const std::vector<TypeParameter>& typeParameters)
    : m_name(name)
    , m_kind(kind)
    , m_properties(properties)
    , m_typeParametersWithConstraints(typeParameters) {
    // Convert TypeParameter objects to simple type parameter names
    for (const auto& param : typeParameters) {
        m_typeParameters.push_back(param.name);
    }
}

const std::string& CustomTypeDefinition::getName() const { return m_name; }

CustomTypeKind CustomTypeDefinition::getKind() const { return m_kind; }

bool CustomTypeDefinition::hasProperty(const std::string& name) const {
    return m_properties.find(name) != m_properties.end();
}

Value CustomTypeDefinition::getProperty(const std::string& name) const {
    auto it = m_properties.find(name);
    if (it != m_properties.end()) {
        return it->second;
    }
    return Value();  // Return nil if property doesn't exist
}

void CustomTypeDefinition::setProperty(const std::string& name, const Value& value) {
    m_properties[name] = value;
}

bool CustomTypeDefinition::isGeneric() const { return !m_typeParameters.empty(); }

const std::vector<std::string>& CustomTypeDefinition::getTypeParameters() const {
    return m_typeParameters;
}

const std::vector<TypeParameter>& CustomTypeDefinition::getTypeParametersWithConstraints() const {
    return m_typeParametersWithConstraints;
}

std::string CustomTypeDefinition::toString() const {
    std::stringstream ss;

    // Convert type kind to string
    std::string kindStr = CustomTypeKindToString(m_kind);

    ss << kindStr << " " << m_name;

    // Add type parameters for generic types
    if (isGeneric()) {
        ss << "<";
        for (size_t i = 0; i < m_typeParametersWithConstraints.size(); ++i) {
            if (i > 0) {
                ss << ", ";
            }
            ss << m_typeParametersWithConstraints[i].name;
            if (m_typeParametersWithConstraints[i].constraint) {
                ss << " extends " << *m_typeParametersWithConstraints[i].constraint;
            }
        }
        ss << ">";
    }

    // Add properties
    if (!m_properties.empty()) {
        ss << " {";
        bool first = true;
        for (const auto& [key, value] : m_properties) {
            if (!first) {
                ss << ", ";
            }
            first = false;
            ss << key << ": " << value.toString();
        }
        ss << "}";
    }

    return ss.str();
}

// CustomTypeInstance implementation
CustomTypeInstance::CustomTypeInstance(std::shared_ptr<CustomTypeDefinition> typeDefinition,
                                       const std::map<std::string, Value>& values)
    : m_typeDefinition(typeDefinition), m_values(values) {}

std::shared_ptr<CustomTypeDefinition> CustomTypeInstance::getTypeDefinition() const {
    return m_typeDefinition;
}

bool CustomTypeInstance::hasField(const std::string& name) const {
    return m_values.find(name) != m_values.end();
}

Value CustomTypeInstance::getField(const std::string& name) const {
    auto it = m_values.find(name);
    if (it != m_values.end()) {
        return it->second;
    }
    return Value();  // Return nil if field doesn't exist
}

void CustomTypeInstance::setField(const std::string& name, const Value& value) {
    // Check if the field exists in the type definition
    if (m_typeDefinition->hasProperty(name)) {
        m_values[name] = value;
    } else {
        std::stringstream ss;
        ss << "Field '" << name << "' does not exist in type '" << m_typeDefinition->getName()
           << "'";
        ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
    }
}

const std::map<std::string, Value>& CustomTypeInstance::getAllFields() const { return m_values; }

std::string CustomTypeInstance::toString() const {
    std::stringstream ss;

    ss << m_typeDefinition->getName() << " {";

    bool first = true;
    for (const auto& [key, value] : m_values) {
        if (!first) {
            ss << ", ";
        }
        first = false;
        ss << key << ": " << value.toString();
    }

    ss << "}";

    return ss.str();
}

// CustomTypeSystem implementation
CustomTypeSystem::CustomTypeSystem() {
    initializeTypeCheckers();
    initializeBuiltInTypes();
}

void CustomTypeSystem::initializeTypeCheckers() {
    // Initialize type checkers for different kinds of types
    m_typeCheckers[CustomTypeKind::STRUCT] = [this](const Value& value,
                                                    const CustomTypeDefinition& typeDef) -> bool {
        // For structs, check if the value is an object and has all required fields
        if (!value.isObject()) {
            return false;
        }

        const auto& obj = value.asObject();

        // Check if the type has a fields property
        if (!typeDef.hasProperty("fields") || !typeDef.getProperty("fields").isMap()) {
            // Missing or invalid fields property
            return false;
        }

        // Check each property defined in the type
        for (const auto& [propName, propValue] : typeDef.getProperty("fields").asMap()) {
            // Check if the property exists in the object
            if (!obj->hasProperty(propName)) {
                return false;
            }

            // If the property has a type, check if the value conforms to that type
            if (propValue.isMap() && propValue.asMap().find("type") != propValue.asMap().end() &&
                propValue.asMap().at("type").isString()) {
                const std::string& fieldTypeName = propValue.asMap().at("type").asString();
                if (!checkType(obj->getProperty(propName), fieldTypeName)) {
                    return false;
                }
            }
        }

        return true;
    };

    m_typeCheckers[CustomTypeKind::ENUM] = [](const Value& value,
                                              const CustomTypeDefinition& typeDef) -> bool {
        // For enums, check if the value is one of the allowed values
        if (!value.isString() && !value.isInteger()) {
            return false;
        }

        // Check if the type has a variants property
        if (!typeDef.hasProperty("variants") || !typeDef.getProperty("variants").isMap()) {
            // Missing or invalid variants property
            return false;
        }

        const auto& variants = typeDef.getProperty("variants").asMap();

        if (value.isString()) {
            const std::string& strValue = value.asString();
            // Check if the string value is one of the enum variants
            return variants.find(strValue) != variants.end();
        } else if (value.isInteger()) {
            int64_t intValue = value.asInteger();
            // Check if the integer value is one of the enum variants
            for (const auto& [variantName, variantValue] : variants) {
                if (variantValue.isInteger() && variantValue.asInteger() == intValue) {
                    return true;
                }
            }
        }

        return false;
    };

    m_typeCheckers[CustomTypeKind::UNION] = [this](const Value& value,
                                                   const CustomTypeDefinition& typeDef) -> bool {
        // For unions, check if the value conforms to any of the possible types

        // Check if the type has a types or memberTypes property
        if ((!typeDef.hasProperty("types") || !typeDef.getProperty("types").isArray()) &&
            (!typeDef.hasProperty("memberTypes") ||
             !typeDef.getProperty("memberTypes").isArray())) {
            // Missing or invalid types property
            return false;
        }

        const auto& possibleTypes = typeDef.hasProperty("types")
                                        ? typeDef.getProperty("types").asArray()
                                        : typeDef.getProperty("memberTypes").asArray();

        for (size_t i = 0; i < possibleTypes.size(); ++i) {
            if (possibleTypes.at(i).isString()) {
                const std::string& typeName = possibleTypes.at(i).asString();
                if (checkType(value, typeName)) {
                    return true;
                }
            }
        }

        return false;
    };

    m_typeCheckers[CustomTypeKind::ALIAS] = [this](const Value& value,
                                                   const CustomTypeDefinition& typeDef) -> bool {
        // For aliases, check if the value conforms to the aliased type

        // Check if the type has an aliasedType or aliased_type property
        if ((!typeDef.hasProperty("aliasedType") ||
             !typeDef.getProperty("aliasedType").isString()) &&
            (!typeDef.hasProperty("aliased_type") ||
             !typeDef.getProperty("aliased_type").isString())) {
            // Missing or invalid aliasedType property
            return false;
        }

        const std::string& aliasedTypeName = typeDef.hasProperty("aliasedType")
                                                 ? typeDef.getProperty("aliasedType").asString()
                                                 : typeDef.getProperty("aliased_type").asString();

        return checkType(value, aliasedTypeName);
    };

    m_typeCheckers[CustomTypeKind::FUNCTION_TYPE] =
        [](const Value& value, const CustomTypeDefinition& typeDef) -> bool {
        // For function types, check if the value is a function
        return value.isNativeFunction() || value.isChronovyanFunction();
    };

    m_typeCheckers[CustomTypeKind::MODULE_TYPE] = [](const Value& value,
                                                     const CustomTypeDefinition& typeDef) -> bool {
        // For module types, check if the value is an object
        return value.isObject();
    };
}

void CustomTypeSystem::initializeBuiltInTypes() {
    // Define built-in types

    // Primitive types
    auto emptyFieldsMap = std::make_shared<ChronovyanMap>();

    std::map<std::string, Value> primitiveProps = {
        {"primitive", Value(true)},
        {"fields", Value(emptyFieldsMap)}  // Add empty fields to pass validation
    };

    defineBuiltInType("nil", primitiveProps);
    defineBuiltInType("bool", primitiveProps);
    defineBuiltInType("int", primitiveProps);
    defineBuiltInType("float", primitiveProps);
    defineBuiltInType("string", primitiveProps);

    // Container types

    // Array type
    auto arrayProps = std::map<std::string, Value>{
        {"primitive", Value(false)},
        {"container", Value(true)},
        {"fields", Value(emptyFieldsMap)}  // Add empty fields to pass validation
    };
    auto arrayParams = std::vector<std::string>{"T"};
    auto arrayType = std::make_shared<CustomTypeDefinition>("Array", CustomTypeKind::STRUCT,
                                                            arrayProps, arrayParams);
    registerType(arrayType);

    // Map type
    auto mapProps = std::map<std::string, Value>{
        {"primitive", Value(false)},
        {"container", Value(true)},
        {"fields", Value(emptyFieldsMap)}  // Add empty fields to pass validation
    };
    auto mapParams = std::vector<std::string>{"K", "V"};
    auto mapType =
        std::make_shared<CustomTypeDefinition>("Map", CustomTypeKind::STRUCT, mapProps, mapParams);
    registerType(mapType);

    // Function type
    auto funcProps = std::map<std::string, Value>{{"primitive", Value(false)}};
    auto functionType = std::make_shared<CustomTypeDefinition>(
        "Function", CustomTypeKind::FUNCTION_TYPE, funcProps);
    registerType(functionType);

    // Object type
    auto objProps = std::map<std::string, Value>{
        {"primitive", Value(false)},
        {"fields", Value(emptyFieldsMap)}  // Add empty fields to pass validation
    };
    auto objectType =
        std::make_shared<CustomTypeDefinition>("Object", CustomTypeKind::STRUCT, objProps);
    registerType(objectType);

    // Any type (can be any value)
    auto typesList = std::make_shared<ChronovyanArray>();
    typesList->push(Value("nil"));
    typesList->push(Value("bool"));
    typesList->push(Value("int"));
    typesList->push(Value("float"));
    typesList->push(Value("string"));
    typesList->push(Value("Array"));
    typesList->push(Value("Map"));
    typesList->push(Value("Function"));
    typesList->push(Value("Object"));

    auto anyProps = std::map<std::string, Value>{
        {"primitive", Value(false)},
        {"memberTypes", Value(typesList)}  // Use memberTypes instead of types for validation
    };
    auto anyType = std::make_shared<CustomTypeDefinition>("Any", CustomTypeKind::UNION, anyProps);
    registerType(anyType);
}

bool CustomTypeSystem::registerType(std::shared_ptr<CustomTypeDefinition> typeDef) {
    const std::string& name = typeDef->getName();

    // Check if the type already exists
    if (m_types.find(name) != m_types.end()) {
        std::stringstream ss;
        ss << "Type '" << name << "' is already defined";
        ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
        return false;
    }

    // Validate the type definition based on its kind
    bool isValid = true;
    std::string validationError;

    switch (typeDef->getKind()) {
        case CustomTypeKind::STRUCT:
            if (!typeDef->hasProperty("fields")) {
                isValid = false;
                validationError = "Struct type '" + name + "' must have a 'fields' property";
            }
            break;

        case CustomTypeKind::ENUM:
            if (!typeDef->hasProperty("variants")) {
                isValid = false;
                validationError = "Enum type '" + name + "' must have a 'variants' property";
            }
            break;

        case CustomTypeKind::UNION:
            if (!typeDef->hasProperty("memberTypes") && !typeDef->hasProperty("types")) {
                isValid = false;
                validationError = "Union type '" + name + "' must have a 'memberTypes' property";
            }
            break;

        case CustomTypeKind::ALIAS:
            if (!typeDef->hasProperty("aliasedType") && !typeDef->hasProperty("aliased_type")) {
                isValid = false;
                validationError = "Alias type '" + name + "' must have an 'aliasedType' property";
            }
            break;

        default:
            // No specific validation for other type kinds
            break;
    }

    // Report validation error if any
    if (!isValid) {
        ErrorHandler::getInstance().reportError(SourceLocation(), validationError);
        return false;
    }

    // Register the type
    m_types[name] = typeDef;
    return true;
}

bool CustomTypeSystem::isTypeRegistered(const std::string& name) const {
    return m_types.find(name) != m_types.end();
}

std::shared_ptr<CustomTypeDefinition> CustomTypeSystem::getTypeDefinition(
    const std::string& name) const {
    auto it = m_types.find(name);
    if (it != m_types.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<CustomTypeInstance> CustomTypeSystem::createInstance(
    const std::string& typeName, const std::map<std::string, Value>& values) {
    auto typeDef = getTypeDefinition(typeName);
    if (!typeDef) {
        std::stringstream ss;
        ss << "Cannot create instance: Type '" << typeName << "' is not defined";
        ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
        return nullptr;
    }

    // Additional validation for struct types
    if (typeDef->getKind() == CustomTypeKind::STRUCT) {
        // Check that all required fields are provided
        if (typeDef->hasProperty("fields")) {
            const auto& fields = typeDef->getProperty("fields").asMap();
            for (const auto& [fieldName, fieldType] : fields) {
                // Check if field is required (no default value) and not provided
                if (values.find(fieldName) == values.end()) {
                    // This is just a warning, not an error
                    std::stringstream ss;
                    ss << "Field '" << fieldName << "' of type '" << fieldType.asString()
                       << "' is not provided when creating an instance of '" << typeName << "'";
                    ErrorHandler::getInstance().reportWarning(SourceLocation(), ss.str());
                }
            }
        }
    }

    // Create an instance of the type
    return std::make_shared<CustomTypeInstance>(typeDef, values);
}

bool CustomTypeSystem::checkType(const Value& value, const std::string& typeName) const {
    // Check if the type is registered
    auto typeDef = getTypeDefinition(typeName);
    if (!typeDef) {
        std::stringstream ss;
        ss << "Type checking failed: Type '" << typeName << "' is not defined";
        ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
        return false;
    }

    // Check primitive types first
    if (typeName == "nil") {
        if (!value.isNil()) {
            std::stringstream ss;
            ss << "Type mismatch: Expected 'nil', but got " << value.getTypeName();
            ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
            return false;
        }
        return true;
    } else if (typeName == "bool") {
        if (!value.isBoolean()) {
            std::stringstream ss;
            ss << "Type mismatch: Expected 'bool', but got " << value.getTypeName();
            ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
            return false;
        }
        return true;
    } else if (typeName == "int") {
        if (!value.isInteger()) {
            std::stringstream ss;
            ss << "Type mismatch: Expected 'int', but got " << value.getTypeName();
            ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
            return false;
        }
        return true;
    } else if (typeName == "float") {
        if (!value.isFloat()) {
            std::stringstream ss;
            ss << "Type mismatch: Expected 'float', but got " << value.getTypeName();
            ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
            return false;
        }
        return true;
    } else if (typeName == "string") {
        if (!value.isString()) {
            std::stringstream ss;
            ss << "Type mismatch: Expected 'string', but got " << value.getTypeName();
            ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
            return false;
        }
        return true;
    } else if (typeName == "Any") {
        return true;  // Any value conforms to the Any type
    }

    // Check custom types using the registered type checker
    auto checkerIt = m_typeCheckers.find(typeDef->getKind());
    if (checkerIt != m_typeCheckers.end()) {
        bool result = checkerIt->second(value, *typeDef);
        if (!result) {
            // Prepare a more detailed error message based on the type kind
            std::stringstream ss;
            ss << "Type mismatch: Value of type '" << value.getTypeName()
               << "' does not conform to type '" << typeName << "'";

            switch (typeDef->getKind()) {
                case CustomTypeKind::STRUCT:
                    ss << " (struct)";
                    if (value.isObject()) {
                        // Check missing fields for better error messages
                        const auto& fields = typeDef->getProperty("fields").asMap();
                        for (const auto& [fieldName, fieldType] : fields) {
                            if (!value.asObject()->hasProperty(fieldName)) {
                                ss << "\n  - Missing field '" << fieldName << "' of type '"
                                   << fieldType.asString() << "'";
                            }
                        }
                    } else {
                        ss << " - Expected an object value";
                    }
                    break;

                case CustomTypeKind::ENUM:
                    ss << " (enum)";
                    if (value.isString() || value.isInteger()) {
                        ss << " - Value is not a valid enum variant for '" << typeName << "'";
                    } else {
                        ss << " - Expected a string or integer value representing an enum variant";
                    }
                    break;

                case CustomTypeKind::UNION:
                    ss << " (union)";
                    ss << " - Value does not match any of the member types in the union";
                    break;

                case CustomTypeKind::ALIAS:
                    if ((typeDef->hasProperty("aliasedType") &&
                         typeDef->getProperty("aliasedType").isString()) ||
                        (typeDef->hasProperty("aliased_type") &&
                         typeDef->getProperty("aliased_type").isString())) {
                        std::string aliasedType =
                            typeDef->hasProperty("aliasedType")
                                ? typeDef->getProperty("aliasedType").asString()
                                : typeDef->getProperty("aliased_type").asString();

                        ss << " (alias for '" << aliasedType << "')";
                    } else {
                        ss << " (alias for unknown type)";
                    }
                    break;

                default:
                    break;
            }

            ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
        }
        return result;
    }

    // If no type checker is registered, assume the type check fails
    std::stringstream ss;
    ss << "No type checker available for type '" << typeName << "' of kind "
       << static_cast<int>(typeDef->getKind());
    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
    return false;
}

const std::map<std::string, std::shared_ptr<CustomTypeDefinition>>& CustomTypeSystem::getAllTypes()
    const {
    return m_types;
}

std::shared_ptr<CustomTypeDefinition> CustomTypeSystem::defineBuiltInType(
    const std::string& name, const std::map<std::string, Value>& properties) {
    auto typeDef = std::make_shared<CustomTypeDefinition>(
        name,
        CustomTypeKind::STRUCT,  // Use STRUCT as the default kind for built-in types
        properties);

    registerType(typeDef);
    return typeDef;
}

TypeExpression CustomTypeSystem::parseAndValidateType(const std::string& expression,
                                                      const SourceLocation& location) {
    // Parse the type expression
    TypeExpression typeExpr = TypeExpressionParser::parse(expression, location);

    // Validate that the base type exists
    if (!isTypeRegistered(typeExpr.baseName) && typeExpr.baseName != "nil" &&
        typeExpr.baseName != "bool" && typeExpr.baseName != "int" && typeExpr.baseName != "float" &&
        typeExpr.baseName != "string" && typeExpr.baseName != "Array" &&
        typeExpr.baseName != "Map" && typeExpr.baseName != "Function" &&
        typeExpr.baseName != "Object" && typeExpr.baseName != "Any") {
        std::stringstream ss;
        ss << "Unknown type '" << typeExpr.baseName << "' in type expression";
        ErrorHandler::getInstance().reportError(location, ss.str());
    }

    // Validate type arguments for generic types
    if (!typeExpr.typeArguments.empty()) {
        auto typeDef = getTypeDefinition(typeExpr.baseName);

        if (!typeDef) {
            std::stringstream ss;
            ss << "Cannot apply type arguments to unknown type '" << typeExpr.baseName << "'";
            ErrorHandler::getInstance().reportError(location, ss.str());
        } else if (!typeDef->isGeneric()) {
            std::stringstream ss;
            ss << "Type '" << typeExpr.baseName
               << "' is not a generic type and cannot have type arguments";
            ErrorHandler::getInstance().reportError(location, ss.str());
        } else {
            // Check if the number of type arguments matches the number of type parameters
            const auto& typeParams = typeDef->getTypeParameters();
            if (typeExpr.typeArguments.size() != typeParams.size()) {
                std::stringstream ss;
                ss << "Type '" << typeExpr.baseName << "' expects " << typeParams.size()
                   << " type arguments, but got " << typeExpr.typeArguments.size();
                ErrorHandler::getInstance().reportError(location, ss.str());
            }

            // Recursively validate each type argument
            for (auto& arg : typeExpr.typeArguments) {
                // This is a recursive call to validate nested type arguments
                arg = parseAndValidateType(arg.toString(), location);
            }
        }
    }

    return typeExpr;
}

std::string CustomTypeSystem::createGenericTypeInstance(const std::string& baseType,
                                                        const std::vector<std::string>& typeArgs,
                                                        const SourceLocation& location) {
    // Check if the base type exists and is generic
    auto typeDef = getTypeDefinition(baseType);
    if (!typeDef) {
        std::stringstream ss;
        ss << "Cannot create generic instance: Base type '" << baseType << "' is not defined";
        ErrorHandler::getInstance().reportError(location, ss.str());
        return "Any";  // Default to Any type
    }

    if (!typeDef->isGeneric()) {
        std::stringstream ss;
        ss << "Cannot create generic instance: Type '" << baseType << "' is not a generic type";
        ErrorHandler::getInstance().reportError(location, ss.str());
        return baseType;  // Return the base type itself
    }

    // Check if the number of type arguments matches the number of type parameters
    const auto& typeParams = typeDef->getTypeParameters();
    if (typeArgs.size() != typeParams.size()) {
        std::stringstream ss;
        ss << "Generic type '" << baseType << "' expects " << typeParams.size()
           << " type arguments, but got " << typeArgs.size();
        ErrorHandler::getInstance().reportError(location, ss.str());
        return "Any";  // Default to Any type
    }

    // Generate a name for the instantiated generic type
    std::stringstream instanceName;
    instanceName << baseType << "<";
    for (size_t i = 0; i < typeArgs.size(); ++i) {
        if (i > 0) {
            instanceName << ", ";
        }
        instanceName << typeArgs[i];
    }
    instanceName << ">";

    // Check if this generic instance already exists
    if (isTypeRegistered(instanceName.str())) {
        return instanceName.str();
    }

    // Create a new type definition for the generic instance
    auto props = typeDef->getProperty("fields").asMap();
    std::map<std::string, Value> instanceProps;

    // Copy properties from the base type, substituting type parameters with arguments
    for (const auto& [propName, propValue] : props) {
        // If the property is a type reference, check if it's a type parameter
        if (propValue.isString()) {
            std::string typeName = propValue.asString();
            auto it = std::find(typeParams.begin(), typeParams.end(), typeName);
            if (it != typeParams.end()) {
                // Replace type parameter with the corresponding type argument
                size_t index = std::distance(typeParams.begin(), it);
                instanceProps[propName] = Value(typeArgs[index]);
            } else {
                // Keep the original type
                instanceProps[propName] = propValue;
            }
        } else {
            // For non-type properties, keep the original value
            instanceProps[propName] = propValue;
        }
    }

    // Add references to the base type and type arguments
    instanceProps["baseType"] = Value(baseType);

    auto typeArgsList = std::make_shared<ChronovyanArray>();
    for (const auto& arg : typeArgs) {
        typeArgsList->push(Value(arg));
    }
    instanceProps["typeArguments"] = Value(typeArgsList);

    // Register the generic instance
    auto instanceDef = std::make_shared<CustomTypeDefinition>(
        instanceName.str(), CustomTypeKind::GENERIC_INSTANCE, instanceProps);

    registerType(instanceDef);
    return instanceName.str();
}

bool CustomTypeSystem::isSubtypeOf(const std::string& subType, const std::string& superType) const {
    // A type is always a subtype of itself
    if (subType == superType) {
        return true;
    }

    // Any type is a supertype of all types
    if (superType == "Any") {
        return true;
    }

    // Check if the subtype is defined
    auto subTypeDef = getTypeDefinition(subType);
    if (!subTypeDef) {
        // For primitive types, handle special cases
        if (subType == "nil" || subType == "bool" || subType == "int" || subType == "float" ||
            subType == "string") {
            // Primitive types are only subtypes of Any
            return superType == "Any";
        }
        return false;
    }

    // For alias types, check the aliased type
    if (subTypeDef->getKind() == CustomTypeKind::ALIAS) {
        // Check if the type has an aliasedType property
        if ((!subTypeDef->hasProperty("aliasedType") ||
             !subTypeDef->getProperty("aliasedType").isString()) &&
            (!subTypeDef->hasProperty("aliased_type") ||
             !subTypeDef->getProperty("aliased_type").isString())) {
            // Missing or invalid aliasedType property
            return false;
        }

        std::string aliasedType = subTypeDef->hasProperty("aliasedType")
                                      ? subTypeDef->getProperty("aliasedType").asString()
                                      : subTypeDef->getProperty("aliased_type").asString();
        return isSubtypeOf(aliasedType, superType);
    }

    // For union types, check if all member types are subtypes of the supertype
    if (subTypeDef->getKind() == CustomTypeKind::UNION) {
        const auto& memberTypes = subTypeDef->getProperty("types").asArray();
        for (size_t i = 0; i < memberTypes.size(); ++i) {
            if (!isSubtypeOf(memberTypes.at(i).asString(), superType)) {
                return false;
            }
        }
        return true;
    }

    // For generic instances, check if the base type is a subtype of the supertype
    if (subTypeDef->getKind() == CustomTypeKind::GENERIC_INSTANCE) {
        std::string baseType = subTypeDef->getProperty("baseType").asString();
        return isSubtypeOf(baseType, superType);
    }

    // TODO: Add support for interface inheritance and struct subtyping

    return false;
}

}  // namespace chronovyan