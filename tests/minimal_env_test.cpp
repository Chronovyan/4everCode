#include "environment.h"
#include "gtest/gtest.h"
#include "value.h"

using namespace chronovyan;

// Simple class to test just the Environment class
class MinimalEnvironmentTest : public ::testing::Test {
protected:
    Environment env;

    void SetUp() override {
        // Initialize environment with some resources
        env.setAethelLevel(100.0);
        env.setChrononLevel(100.0);
    }
};

// Test for basic variable operations
TEST_F(MinimalEnvironmentTest, VariableOperations) {
    // Define a variable
    env.define("test_var", Value(42.0));

    // Check that it exists
    EXPECT_TRUE(env.exists("test_var"));

    // Retrieve the value
    Value result = env.get("test_var");
    EXPECT_EQ(result.asFloat(), 42.0);

    // Assign a new value
    env.assign("test_var", Value(84.0));

    // Check the new value
    result = env.get("test_var");
    EXPECT_EQ(result.asFloat(), 84.0);
}

// Test for resource management
TEST_F(MinimalEnvironmentTest, ResourceManagement) {
    // Check initial resource levels
    EXPECT_EQ(env.getAethelLevel(), 100.0);
    EXPECT_EQ(env.getChrononLevel(), 100.0);

    // Expend resources
    env.expendAethel(25.0);
    env.expendChronon(15.0);

    // Check updated resource levels
    EXPECT_EQ(env.getAethelLevel(), 75.0);
    EXPECT_EQ(env.getChrononLevel(), 85.0);

    // Check resource availability
    EXPECT_TRUE(env.hasEnoughAethel(50.0));
    EXPECT_FALSE(env.hasEnoughAethel(100.0));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}