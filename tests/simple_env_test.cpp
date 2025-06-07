#include <memory>

#include "environment.h"
#include "gtest/gtest.h"
#include "value.h"

using namespace chronovyan;

// Simple test for Environment class functionality
class SimpleEnvironmentTest : public ::testing::Test {
protected:
    std::shared_ptr<Environment> env;

    void SetUp() override {
        env = std::make_shared<Environment>();
        env->setAethelLevel(100.0);
        env->setChrononLevel(100.0);
    }
};

// Test for defining and retrieving variables
TEST_F(SimpleEnvironmentTest, DefineAndRetrieve) {
    // Define a variable
    env->define("test_var", Value(42.0));

    // Retrieve the variable
    Value result = env->get("test_var");

    // Check the value
    EXPECT_EQ(result.asFloat(), 42.0);
}

// Test for assigning to existing variables
TEST_F(SimpleEnvironmentTest, Assignment) {
    // Define a variable
    env->define("test_var", Value(42.0));

    // Assign a new value
    env->assign("test_var", Value(84.0));

    // Check the new value
    Value result = env->get("test_var");
    EXPECT_EQ(result.asFloat(), 84.0);
}

// Test for managing resource levels
TEST_F(SimpleEnvironmentTest, ResourceLevels) {
    // Check initial values
    EXPECT_EQ(env->getAethelLevel(), 100.0);
    EXPECT_EQ(env->getChrononLevel(), 100.0);

    // Change the levels
    env->setAethelLevel(50.0);
    env->setChrononLevel(75.0);

    // Check the new values
    EXPECT_EQ(env->getAethelLevel(), 50.0);
    EXPECT_EQ(env->getChrononLevel(), 75.0);
}

// Test for resource expenditure
TEST_F(SimpleEnvironmentTest, ResourceExpenditure) {
    // Expend resources
    env->expendAethel(25.0);
    env->expendChronon(15.0);

    // Check the new levels
    EXPECT_EQ(env->getAethelLevel(), 75.0);
    EXPECT_EQ(env->getChrononLevel(), 85.0);
}

// Test for resource availability checking
TEST_F(SimpleEnvironmentTest, ResourceAvailability) {
    // Check availability
    EXPECT_TRUE(env->hasEnoughAethel(50.0));
    EXPECT_TRUE(env->hasEnoughChronon(75.0));

    // Check unavailability
    EXPECT_FALSE(env->hasEnoughAethel(150.0));
    EXPECT_FALSE(env->hasEnoughChronon(125.0));
}

// Test for enclosing environments
TEST_F(SimpleEnvironmentTest, EnclosingEnvironments) {
    // Create an enclosing environment
    auto enclosedEnv = std::make_shared<Environment>(env);

    // Define a variable in the enclosing environment
    env->define("outer_var", Value(42.0));

    // Define a variable in the enclosed environment
    enclosedEnv->define("inner_var", Value(84.0));

    // Check that the enclosed environment can access the outer variable
    Value outerResult = enclosedEnv->get("outer_var");
    EXPECT_EQ(outerResult.asFloat(), 42.0);

    // Check that the enclosed environment can access its own variable
    Value innerResult = enclosedEnv->get("inner_var");
    EXPECT_EQ(innerResult.asFloat(), 84.0);

    // Check that the enclosing environment cannot access the inner variable
    EXPECT_THROW(env->get("inner_var"), std::runtime_error);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}