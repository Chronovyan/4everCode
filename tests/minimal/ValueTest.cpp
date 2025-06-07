#include <chronovyan/stdlib/core/Core.h>
#include <gtest/gtest.h>

using namespace chronovyan::stdlib;

// Forward declarations for test functions
void testValue(const ValuePtr& value, Type expectedType, bool expectedTruthiness);

TEST(ValueTest, BasicTest) {
    // Test default construction
    Value v1;
    EXPECT_TRUE(v1.isNil());

    // Test boolean construction
    Value v2(true);
    EXPECT_TRUE(v2.isBoolean());
    EXPECT_TRUE(v2.asBoolean());

    // Test number construction
    Value v3(42.0);
    EXPECT_TRUE(v3.isNumeric());
    EXPECT_DOUBLE_EQ(42.0, v3.asNumber());

    // Test string construction
    Value v4("test");
    EXPECT_TRUE(v4.isString());
    EXPECT_EQ("test", v4.asString());
}

TEST(ValueTest, TypeChecks) {
    Value v;

    // Default should be nil
    EXPECT_TRUE(v.isNil());
    EXPECT_FALSE(v.isBoolean());
    EXPECT_FALSE(v.isNumeric());
    EXPECT_FALSE(v.isString());

    // Test type changes
    v = true;
    EXPECT_FALSE(v.isNil());
    EXPECT_TRUE(v.isBoolean());

    v = 3.14;
    EXPECT_TRUE(v.isNumeric());
    EXPECT_FALSE(v.isString());

    v = "hello";
    EXPECT_TRUE(v.isString());
    EXPECT_FALSE(v.isNumeric());
}

TEST(ValueTest, Equality) {
    Value v1(42.0);
    Value v2(42.0);
    Value v3(3.14);

    EXPECT_TRUE(v1.equals(v2));
    EXPECT_FALSE(v1.equals(v3));

    // Test with different types
    Value v4("42");
    EXPECT_FALSE(v1.equals(v4));
}

TEST(ValueTest, Comparison) {
    Value v1(10.0);
    Value v2(20.0);

    EXPECT_TRUE(v1 < v2);
    EXPECT_FALSE(v2 < v1);
    EXPECT_FALSE(v1 < v1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
