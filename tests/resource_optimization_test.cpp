#include <memory>

#include "environment.h"
#include "gtest/gtest.h"
#include "value.h"

using namespace chronovyan;

// Simplified test case focusing just on Environment resource operations
class ResourceOptimizationTest : public ::testing::Test {
protected:
    std::shared_ptr<Environment> env;

    void SetUp() override {
        env = std::make_shared<Environment>();
        env->setAethelLevel(100.0);
        env->setChrononLevel(100.0);
    }
};

// Test for resource level management
TEST_F(ResourceOptimizationTest, ResourceLevelManagement) {
    // Check initial values
    EXPECT_EQ(env->getAethelLevel(), 100.0);
    EXPECT_EQ(env->getChrononLevel(), 100.0);

    // Test setting new values
    env->setAethelLevel(50.0);
    env->setChrononLevel(75.0);

    EXPECT_EQ(env->getAethelLevel(), 50.0);
    EXPECT_EQ(env->getChrononLevel(), 75.0);
}

// Test for resource availability checking
TEST_F(ResourceOptimizationTest, ResourceAvailabilityCheck) {
    // Test resource availability
    EXPECT_TRUE(env->hasEnoughAethel(50.0));
    EXPECT_TRUE(env->hasEnoughChronon(50.0));

    EXPECT_FALSE(env->hasEnoughAethel(150.0));
    EXPECT_FALSE(env->hasEnoughChronon(150.0));
}

// Test for resource expenditure
TEST_F(ResourceOptimizationTest, ResourceExpenditure) {
    // Expend resources
    env->expendAethel(25.0);
    env->expendChronon(15.0);

    // Verify resources were properly expended
    EXPECT_EQ(env->getAethelLevel(), 75.0);
    EXPECT_EQ(env->getChrononLevel(), 85.0);

    // Expend more resources
    env->expendAethel(25.0);
    env->expendChronon(35.0);

    // Verify updated levels
    EXPECT_EQ(env->getAethelLevel(), 50.0);
    EXPECT_EQ(env->getChrononLevel(), 50.0);
}

// Test for resource insufficiency
TEST_F(ResourceOptimizationTest, ResourceInsufficiency) {
    // Set initial resources to low values
    env->setAethelLevel(20.0);
    env->setChrononLevel(10.0);

    // Test resource insufficiency
    EXPECT_FALSE(env->hasEnoughAethel(25.0));
    EXPECT_FALSE(env->hasEnoughChronon(15.0));

    // Should still be able to expend resources that are available
    EXPECT_TRUE(env->hasEnoughAethel(15.0));
    EXPECT_TRUE(env->hasEnoughChronon(5.0));
}

// Test for variable-based resource management
TEST_F(ResourceOptimizationTest, VariableBasedResourceManagement) {
    // Set a variable representing a resource amount
    env->define("aethel_cost", Value(30.0));
    env->define("chronon_cost", Value(20.0));

    // Get the values and verify
    Value aethelCost = env->get("aethel_cost");
    Value chrononCost = env->get("chronon_cost");

    EXPECT_EQ(aethelCost.asFloat(), 30.0);
    EXPECT_EQ(chrononCost.asFloat(), 20.0);

    // Expend resources based on variable values
    env->expendAethel(aethelCost.asFloat());
    env->expendChronon(chrononCost.asFloat());

    // Verify resources were properly expended
    EXPECT_EQ(env->getAethelLevel(), 70.0);
    EXPECT_EQ(env->getChrononLevel(), 80.0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}