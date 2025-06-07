#include <gtest/gtest.h>

#include <string>

#include "ast_nodes.h"
#include "value.h"

using namespace chronovyan;

class ValueTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code for all tests
    }

    void TearDown() override {
        // Common cleanup code for all tests
    }
};

// Test basic value construction and type checking
TEST_F(ValueTest, BasicConstruction) {
    // Test nil value
    Value nilValue;
    EXPECT_TRUE(nilValue.isNil());
    EXPECT_EQ(nilValue.getType(), Value::Type::NIL);

    // Test boolean values
    Value trueValue(true);
    EXPECT_TRUE(trueValue.isBoolean());
    EXPECT_TRUE(trueValue.asBoolean());

    Value falseValue(false);
    EXPECT_TRUE(falseValue.isBoolean());
    EXPECT_FALSE(falseValue.asBoolean());

    // Test integer value
    Value intValue(static_cast<int64_t>(42));
    EXPECT_TRUE(intValue.isInteger());
    EXPECT_EQ(intValue.asInteger(), 42);

    // Test float value
    Value floatValue(3.14);
    EXPECT_TRUE(floatValue.isFloat());
    EXPECT_DOUBLE_EQ(floatValue.asFloat(), 3.14);

    // Test string value
    Value stringValue(std::string("hello"));
    EXPECT_TRUE(stringValue.isString());
    EXPECT_EQ(stringValue.asString(), "hello");
}

// Test arithmetic operations
TEST_F(ValueTest, ArithmeticOperations) {
    // Test addition
    Value a(static_cast<int64_t>(5));
    Value b(static_cast<int64_t>(3));
    Value sum = add(a, b);
    EXPECT_TRUE(sum.isInteger());
    EXPECT_EQ(sum.asInteger(), 8);

    // Test subtraction
    Value diff = subtract(a, b);
    EXPECT_TRUE(diff.isInteger());
    EXPECT_EQ(diff.asInteger(), 2);

    // Test multiplication
    Value prod = multiply(a, b);
    EXPECT_TRUE(prod.isInteger());
    EXPECT_EQ(prod.asInteger(), 15);

    // Test division
    Value quot = divide(a, b);
    EXPECT_TRUE(quot.isFloat());
    EXPECT_DOUBLE_EQ(quot.asFloat(), 5.0 / 3.0);
}

// Test string operations
TEST_F(ValueTest, StringOperations) {
    Value str1(std::string("hello"));
    Value str2(std::string(" world"));

    // Test string concatenation
    Value concat = add(str1, str2);
    EXPECT_TRUE(concat.isString());
    EXPECT_EQ(concat.asString(), "hello world");

    // Test string with number
    Value num(static_cast<int64_t>(42));
    Value strNum = add(str1, num);
    EXPECT_TRUE(strNum.isString());
    EXPECT_EQ(strNum.asString(), "hello42");
}

// Test variable modifiers
TEST_F(ValueTest, VariableModifiers) {
    Value value(static_cast<int64_t>(42));

    // Test CONF modifier
    value.setModifier(VariableModifier::CONF);
    EXPECT_EQ(value.getModifier(), VariableModifier::CONF);

    // Test REB modifier
    value.setModifier(VariableModifier::REB);
    EXPECT_EQ(value.getModifier(), VariableModifier::REB);
}

// Test variable flags
TEST_F(ValueTest, VariableFlags) {
    Value value(static_cast<int64_t>(42));

    // Test adding flags
    value.addFlag(VariableFlag::STATIC);
    value.addFlag(VariableFlag::VOLATILE);

    EXPECT_TRUE(value.hasFlag(VariableFlag::STATIC));
    EXPECT_TRUE(value.hasFlag(VariableFlag::VOLATILE));
    EXPECT_FALSE(value.hasFlag(VariableFlag::ANCHOR));

    // Test removing flags
    value.removeFlag(VariableFlag::STATIC);
    EXPECT_FALSE(value.hasFlag(VariableFlag::STATIC));
    EXPECT_TRUE(value.hasFlag(VariableFlag::VOLATILE));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}