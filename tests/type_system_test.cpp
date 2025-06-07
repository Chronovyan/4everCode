#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>

#include "../include/custom_type_system.h"
#include "../include/interpreter.h"
#include "../include/value.h"

using namespace chronovyan;

// Test fixture for type system tests
class TypeSystemTest : public ::testing::Test {
protected:
    std::shared_ptr<CustomTypeSystem> typeSystem;

    void SetUp() override {
        typeSystem = std::make_shared<CustomTypeSystem>();

        // Pre-register primitive types that other definitions might refer to by name
        std::map<std::string, Value> intProps;
        intProps["primitive"] = Value(true);
        auto intTypeDef =
            std::make_shared<CustomTypeDefinition>("int", CustomTypeKind::STRUCT, intProps);
        typeSystem->registerType(intTypeDef);

        std::map<std::string, Value> stringProps;
        stringProps["primitive"] = Value(true);
        auto stringTypeDef =
            std::make_shared<CustomTypeDefinition>("string", CustomTypeKind::STRUCT, stringProps);
        typeSystem->registerType(stringTypeDef);

        std::map<std::string, Value> boolProps;
        boolProps["primitive"] = Value(true);
        auto boolTypeDef =
            std::make_shared<CustomTypeDefinition>("bool", CustomTypeKind::STRUCT, boolProps);
        typeSystem->registerType(boolTypeDef);

        // Potentially other primitives if needed by tests: float, nil, etc.
    }
};

// Test struct type definition and checking
TEST_F(TypeSystemTest, StructTypeDefinition) {
    // Create properties for the struct type
    std::map<std::string, Value> properties;

    auto fieldsMap = std::make_shared<ChronovyanMap>();

    auto nameField = std::make_shared<ChronovyanMap>();
    nameField->set("type", Value("string"));
    fieldsMap->set("name", Value(nameField));

    auto ageField = std::make_shared<ChronovyanMap>();
    ageField->set("type", Value("int"));
    fieldsMap->set("age", Value(ageField));

    properties["fields"] = Value(fieldsMap);
    properties["primitive"] = Value(false);
    properties["container"] = Value(true);

    // Create and register the type
    auto personType =
        std::make_shared<CustomTypeDefinition>("Person", CustomTypeKind::STRUCT, properties);
    bool success = typeSystem->registerType(personType);

    EXPECT_TRUE(success);
    EXPECT_TRUE(typeSystem->isTypeRegistered("Person"));

    // Create a Person instance
    std::map<std::string, Value> values = {{"name", Value("Alice")},
                                           {"age", Value(static_cast<int64_t>(30))}};

    auto person = typeSystem->createInstance("Person", values);
    ASSERT_NE(person, nullptr) << "Failed to create Person instance";

    // Check the field values
    EXPECT_TRUE(person->hasField("name"));
    EXPECT_TRUE(person->hasField("age"));

    // Use safe access with null checks
    if (person->hasField("name")) {
        EXPECT_EQ(person->getField("name").asString(), "Alice");
    }
    if (person->hasField("age")) {
        EXPECT_EQ(person->getField("age").asInteger(), 30);
    }

    // Test type checking
    Value personValue = Value(std::make_shared<ObjectValue>());
    auto personObj = personValue.asObject();
    personObj->setProperty("name", Value("Bob"));
    personObj->setProperty("age", Value(static_cast<int64_t>(25)));
    personObj->setProperty("__type", Value("Person"));

    EXPECT_TRUE(typeSystem->checkType(personValue, "Person"));

    // Test invalid type checking
    Value intValue = Value(static_cast<int64_t>(42));
    EXPECT_FALSE(typeSystem->checkType(intValue, "Person"));
}

// Test enum type definition and checking
TEST_F(TypeSystemTest, EnumTypeDefinition) {
    // Create properties for the enum type
    std::map<std::string, Value> properties;

    auto variantsMap = std::make_shared<ChronovyanMap>();
    variantsMap->set("RED", Value(static_cast<int64_t>(0)));
    variantsMap->set("GREEN", Value(static_cast<int64_t>(1)));
    variantsMap->set("BLUE", Value(static_cast<int64_t>(2)));

    properties["variants"] = Value(variantsMap);
    properties["primitive"] = Value(false);
    properties["enum"] = Value(true);

    // Create and register the type
    auto colorType =
        std::make_shared<CustomTypeDefinition>("Color", CustomTypeKind::ENUM, properties);
    bool success = typeSystem->registerType(colorType);

    EXPECT_TRUE(success);
    EXPECT_TRUE(typeSystem->isTypeRegistered("Color"));

    // Test type checking with integer values
    Value redValue = Value(static_cast<int64_t>(0));
    EXPECT_TRUE(typeSystem->checkType(redValue, "Color"));

    // Test type checking with string values
    Value stringVariant = Value("RED");
    EXPECT_TRUE(typeSystem->checkType(stringVariant, "Color"));

    // Test invalid type checking
    Value invalidValue = Value(static_cast<int64_t>(5));
    EXPECT_FALSE(typeSystem->checkType(invalidValue, "Color"));
}

// Test union type definition and checking
TEST_F(TypeSystemTest, UnionTypeDefinition) {
    // Create properties for the union type
    std::map<std::string, Value> properties;

    auto typesList = std::make_shared<ChronovyanArray>();
    typesList->push(Value("int"));
    typesList->push(Value("string"));

    properties["memberTypes"] = Value(typesList);
    properties["primitive"] = Value(false);
    properties["union"] = Value(true);

    // Create and register the type
    auto numOrStrType =
        std::make_shared<CustomTypeDefinition>("NumOrStr", CustomTypeKind::UNION, properties);
    bool success = typeSystem->registerType(numOrStrType);

    EXPECT_TRUE(success);
    EXPECT_TRUE(typeSystem->isTypeRegistered("NumOrStr"));

    // Test type checking
    Value intValue = Value(static_cast<int64_t>(42));
    EXPECT_TRUE(typeSystem->checkType(intValue, "NumOrStr"));

    Value stringValue = Value("Hello");
    EXPECT_TRUE(typeSystem->checkType(stringValue, "NumOrStr"));

    // Test invalid type checking
    Value boolValue = Value(true);
    EXPECT_FALSE(typeSystem->checkType(boolValue, "NumOrStr"));
}

// Test alias type definition and checking
TEST_F(TypeSystemTest, AliasTypeDefinition) {
    // Create properties for the alias type
    std::map<std::string, Value> properties;

    properties["aliased_type"] = Value("int");
    properties["primitive"] = Value(false);
    properties["alias"] = Value(true);

    // Create and register the type
    auto idType = std::make_shared<CustomTypeDefinition>("ID", CustomTypeKind::ALIAS, properties);
    bool success = typeSystem->registerType(idType);

    EXPECT_TRUE(success);
    EXPECT_TRUE(typeSystem->isTypeRegistered("ID"));

    // Test type checking
    Value intValue = Value(static_cast<int64_t>(42));
    EXPECT_TRUE(typeSystem->checkType(intValue, "ID"));

    // Test invalid type checking
    Value stringValue = Value("Hello");
    EXPECT_FALSE(typeSystem->checkType(stringValue, "ID"));
}

// Test generic type definition and instantiation
TEST_F(TypeSystemTest, GenericTypeDefinition) {
    // Create properties for a generic container type
    std::map<std::string, Value> properties;

    auto fieldsMap = std::make_shared<ChronovyanMap>();

    auto valueField = std::make_shared<ChronovyanMap>();
    valueField->set("type", Value("T"));
    fieldsMap->set("value", Value(valueField));

    auto nextField = std::make_shared<ChronovyanMap>();
    nextField->set("type", Value("Box<T>"));
    fieldsMap->set("next", Value(nextField));

    properties["fields"] = Value(fieldsMap);
    properties["primitive"] = Value(false);
    properties["container"] = Value(true);

    // Create and register the generic Box type
    std::vector<std::string> typeParams = {"T"};
    auto boxType = std::make_shared<CustomTypeDefinition>("Box", CustomTypeKind::STRUCT, properties,
                                                          typeParams);
    bool success = typeSystem->registerType(boxType);

    EXPECT_TRUE(success);
    EXPECT_TRUE(typeSystem->isTypeRegistered("Box"));
    EXPECT_TRUE(boxType->isGeneric());

    // Create a generic type instance
    std::string stringBoxType =
        typeSystem->createGenericTypeInstance("Box", {"string"}, SourceLocation());
    EXPECT_EQ(stringBoxType, "Box<string>");
    EXPECT_TRUE(typeSystem->isTypeRegistered("Box<string>"));

    // Test type properties for the instantiated generic
    auto stringBoxDef = typeSystem->getTypeDefinition("Box<string>");
    EXPECT_NE(stringBoxDef, nullptr);
    EXPECT_EQ(stringBoxDef->getKind(), CustomTypeKind::GENERIC_INSTANCE);
    EXPECT_EQ(stringBoxDef->getProperty("baseType").asString(), "Box");

    auto typeArgs = stringBoxDef->getProperty("typeArguments").asArray();
    EXPECT_EQ(typeArgs.size(), 1);
    EXPECT_EQ(typeArgs.at(0).asString(), "string");

    // Test subtyping with generic types
    EXPECT_TRUE(typeSystem->isSubtypeOf("Box<string>", "Any"));
    EXPECT_FALSE(typeSystem->isSubtypeOf("Box<string>", "Box<int>"));
}

// Test type expression parsing
TEST_F(TypeSystemTest, TypeExpressionParsing) {
    // Parse a simple type
    TypeExpression intType = TypeExpressionParser::parse("int");
    EXPECT_EQ(intType.baseName, "int");
    EXPECT_TRUE(intType.typeArguments.empty());
    EXPECT_FALSE(intType.isArray);
    EXPECT_FALSE(intType.isNullable);

    // Parse a generic type
    TypeExpression mapType = TypeExpressionParser::parse("Map<string, int>");
    EXPECT_EQ(mapType.baseName, "Map");
    EXPECT_EQ(mapType.typeArguments.size(), 2);
    EXPECT_EQ(mapType.typeArguments[0].baseName, "string");
    EXPECT_EQ(mapType.typeArguments[1].baseName, "int");

    // Parse an array type
    TypeExpression arrayType = TypeExpressionParser::parse("int[]");
    EXPECT_EQ(arrayType.baseName, "int");
    EXPECT_TRUE(arrayType.isArray);

    // Parse a nullable type
    TypeExpression nullableType = TypeExpressionParser::parse("string?");
    EXPECT_EQ(nullableType.baseName, "string");
    EXPECT_TRUE(nullableType.isNullable);

    // Parse a complex type
    TypeExpression complexType = TypeExpressionParser::parse("Map<string, Array<int>[]>?");
    EXPECT_EQ(complexType.baseName, "Map");
    EXPECT_EQ(complexType.typeArguments.size(), 2);
    EXPECT_EQ(complexType.typeArguments[0].baseName, "string");
    EXPECT_EQ(complexType.typeArguments[1].baseName, "Array");
    EXPECT_TRUE(complexType.typeArguments[1].isArray);
    EXPECT_TRUE(complexType.isNullable);
}

// Test parsing and validating types
TEST_F(TypeSystemTest, TypeValidation) {
    // First, define some types for validation

    // Define Array<T> generic type (assuming CustomTypeSystem recognizes "Array" as a generic base)
    // std::map<std::string, Value> arrayProps;
    // auto arrayFields = std::make_shared<ChronovyanMap>();
    // auto elementsField = std::make_shared<ChronovyanMap>();
    // elementsField->set("type", Value("T[]")); // Or just "T" if Array<T> implies element type T
    // arrayFields->set("elements", Value(elementsField));
    // arrayProps["fields"] = Value(arrayFields);
    // std::vector<std::string> arrayTypeParams = {"T"};
    // auto arrayTypeDef = std::make_shared<CustomTypeDefinition>("Array", CustomTypeKind::STRUCT,
    // arrayProps, arrayTypeParams); typeSystem->registerType(arrayTypeDef); // Do not re-register
    // if "Array" is built-in/known

    // Define Map<K, V> generic type (assuming CustomTypeSystem recognizes "Map" as a generic base)
    // std::map<std::string, Value> mapProps;
    // auto mapFields = std::make_shared<ChronovyanMap>();
    // auto keysField = std::make_shared<ChronovyanMap>();
    // keysField->set("type", Value("K[]")); // Or just "K"
    // mapFields->set("keys", Value(keysField));
    // auto valuesField = std::make_shared<ChronovyanMap>();
    // valuesField->set("type", Value("V[]")); // Or just "V"
    // mapFields->set("values", Value(valuesField));
    // mapProps["fields"] = Value(mapFields);
    // std::vector<std::string> mapTypeParams = {"K", "V"};
    // auto mapTypeDef = std::make_shared<CustomTypeDefinition>("Map", CustomTypeKind::STRUCT,
    // mapProps, mapTypeParams); typeSystem->registerType(mapTypeDef); // Do not re-register if
    // "Map" is built-in/known

    // Now test type validation

    // Valid simple type
    TypeExpression intType = typeSystem->parseAndValidateType("int");
    EXPECT_EQ(intType.baseName, "int");

    // Valid generic type
    TypeExpression arrayOfInt = typeSystem->parseAndValidateType("Array<int>");
    EXPECT_EQ(arrayOfInt.baseName, "Array");
    EXPECT_EQ(arrayOfInt.typeArguments.size(), 1);
    EXPECT_EQ(arrayOfInt.typeArguments[0].baseName, "int");

    // Valid complex type
    TypeExpression complexMapType = typeSystem->parseAndValidateType("Map<string, Array<int>>");
    EXPECT_EQ(complexMapType.baseName, "Map");
    EXPECT_EQ(complexMapType.typeArguments.size(), 2);
    EXPECT_EQ(complexMapType.typeArguments[0].baseName, "string");
    EXPECT_EQ(complexMapType.typeArguments[1].baseName, "Array");

    // Create generic instances
    std::string arrayOfIntType = typeSystem->createGenericTypeInstance("Array", {"int"});
    EXPECT_EQ(arrayOfIntType, "Array<int>");
    EXPECT_TRUE(typeSystem->isTypeRegistered("Array<int>"));

    std::string mapOfStringInt = typeSystem->createGenericTypeInstance("Map", {"string", "int"});
    EXPECT_EQ(mapOfStringInt, "Map<string, int>");
    EXPECT_TRUE(typeSystem->isTypeRegistered("Map<string, int>"));
}

// Test type checking function
TEST_F(TypeSystemTest, TypeOfFunction) {
    // Create a TYPE_OF function
    auto typeOfFunc = [](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
        if (args.empty()) {
            return Value("nil");
        }

        const Value& value = args[0];

        // Return the type name based on the value type
        if (value.isNil()) {
            return Value("nil");
        } else if (value.isBoolean()) {
            return Value("bool");
        } else if (value.isInteger()) {
            return Value("int");
        } else if (value.isFloat()) {
            return Value("float");
        } else if (value.isString()) {
            return Value("string");
        } else if (value.isArray()) {
            return Value("Array");
        } else if (value.isMap()) {
            return Value("Map");
        } else if (value.isNativeFunction() || value.isChronovyanFunction()) {
            return Value("Function");
        } else if (value.isObject()) {
            // Check if the object has a type definition
            auto obj = value.asObject();
            if (obj->hasProperty("__type")) {
                return obj->getProperty("__type");
            } else {
                return Value("Object");
            }
        } else {
            return Value("unknown");
        }
    };

    // Test each value type directly instead of using a map

    // Test nil value
    {
        Value nilValue;
        std::vector<Value> args = {nilValue};
        Interpreter interpreter;
        Value result = typeOfFunc(interpreter, args);
        EXPECT_EQ(result.asString(), "nil");
    }

    // Test boolean value
    {
        Value boolValue(true);
        std::vector<Value> args = {boolValue};
        Interpreter interpreter;
        Value result = typeOfFunc(interpreter, args);
        EXPECT_EQ(result.asString(), "bool");
    }

    // Test integer value
    {
        Value intValue(static_cast<int64_t>(42));
        std::vector<Value> args = {intValue};
        Interpreter interpreter;
        Value result = typeOfFunc(interpreter, args);
        EXPECT_EQ(result.asString(), "int");
    }

    // Test float value
    {
        Value floatValue(3.14);
        std::vector<Value> args = {floatValue};
        Interpreter interpreter;
        Value result = typeOfFunc(interpreter, args);
        EXPECT_EQ(result.asString(), "float");
    }

    // Test string value
    {
        Value stringValue("Hello");
        std::vector<Value> args = {stringValue};
        Interpreter interpreter;
        Value result = typeOfFunc(interpreter, args);
        EXPECT_EQ(result.asString(), "string");
    }

    // Test array value
    {
        Value arrayValue(std::make_shared<ChronovyanArray>());
        std::vector<Value> args = {arrayValue};
        Interpreter interpreter;
        Value result = typeOfFunc(interpreter, args);
        EXPECT_EQ(result.asString(), "Array");
    }

    // Test map value
    {
        Value mapValue(std::make_shared<ChronovyanMap>());
        std::vector<Value> args = {mapValue};
        Interpreter interpreter;
        Value result = typeOfFunc(interpreter, args);
        EXPECT_EQ(result.asString(), "Map");
    }

    // Test function value
    {
        Value funcValue(std::make_shared<NativeFunction>(typeOfFunc, 1));
        std::vector<Value> args = {funcValue};
        Interpreter interpreter;
        Value result = typeOfFunc(interpreter, args);
        EXPECT_EQ(result.asString(), "Function");
    }

    // Test object value
    {
        Value personValue = Value(std::make_shared<ObjectValue>());
        auto personObj = personValue.asObject();
        personObj->setProperty("name", Value("Alice"));
        personObj->setProperty("age", Value(static_cast<int64_t>(30)));
        personObj->setProperty("__type", Value("Person"));

        std::vector<Value> args = {personValue};
        Interpreter interpreter;
        Value result = typeOfFunc(interpreter, args);
        EXPECT_EQ(result.asString(), "Person");
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}