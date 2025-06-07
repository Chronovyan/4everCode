#include <gtest/gtest.h>

#include <algorithm>
#include <memory>

#include "../include/map_type.h"
#include "../include/temporal_runtime.h"
#include "../include/value.h"

namespace chronovyan {

class MapTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        runtime = std::make_shared<TemporalRuntime>();
        map = std::make_shared<TemporalMap>(runtime);
    }

    std::shared_ptr<TemporalRuntime> runtime;
    std::shared_ptr<TemporalMap> map;
};

// Test basic map operations
TEST_F(MapTypeTest, BasicOperations) {
    // Test empty map
    EXPECT_TRUE(map->empty());
    EXPECT_EQ(map->size(), 0);

    // Test set and get with integer values
    map->set("key1", Value(static_cast<int64_t>(42)));
    EXPECT_FALSE(map->empty());
    EXPECT_EQ(map->size(), 1);
    EXPECT_TRUE(map->contains("key1"));
    EXPECT_EQ(map->get("key1").asInt(), 42);

    // Test with string values
    std::string testString = "hello";
    map->set("key2", Value(testString));
    EXPECT_TRUE(map->contains("key2"));
    EXPECT_TRUE(map->get("key2").isString());
    EXPECT_EQ(map->get("key2").asString(), "hello");

    // Test updating a value
    map->set("key1", Value(static_cast<int64_t>(100)));
    EXPECT_EQ(map->get("key1").asInt(), 100);

    // Test removing a value
    EXPECT_TRUE(map->remove("key1"));
    EXPECT_FALSE(map->contains("key1"));
    EXPECT_EQ(map->size(), 1);  // key2 is still there

    // Test clear
    map->clear();
    EXPECT_TRUE(map->empty());
    EXPECT_EQ(map->size(), 0);
}

// Test temporal features
TEST_F(MapTypeTest, TemporalFeatures) {
    // Get initial cycle
    int64_t startCycle = runtime->getCycleNumber();

    // Set initial value and save checkpoint
    map->set("key1", Value(static_cast<int64_t>(10)));
    map->saveHistoryCheckpoint(startCycle);

    // Change value at next cycle
    runtime->consumeChronons(1);
    int64_t cycle1 = runtime->getCycleNumber();
    map->set("key1", Value(static_cast<int64_t>(20)));
    map->saveHistoryCheckpoint(cycle1);

    // Change value at next cycle
    runtime->consumeChronons(1);
    int64_t cycle2 = runtime->getCycleNumber();
    map->set("key1", Value(static_cast<int64_t>(30)));
    map->saveHistoryCheckpoint(cycle2);

    // Current value should be 30
    EXPECT_EQ(map->get("key1").asInt(), 30);

    // Verify we can restore from different cycles
    // First, restore from cycle1 (should have value 20)
    EXPECT_TRUE(map->restoreFromCycle(cycle1));
    EXPECT_EQ(map->get("key1").asInt(), 20);

    // Restore from startCycle (should have value 10)
    EXPECT_TRUE(map->restoreFromCycle(startCycle));
    EXPECT_EQ(map->get("key1").asInt(), 10);

    // Restore from cycle2 (should have value 30)
    EXPECT_TRUE(map->restoreFromCycle(cycle2));
    EXPECT_EQ(map->get("key1").asInt(), 30);

    // Check that we can get values at specific cycles
    EXPECT_EQ(map->getAtCycle("key1", startCycle).asInt(), 10);
    EXPECT_EQ(map->getAtCycle("key1", cycle1).asInt(), 20);
    EXPECT_EQ(map->getAtCycle("key1", cycle2).asInt(), 30);

    // Test clearing history
    map->clearHistory();

    // Should still have the current value after clearing history
    EXPECT_EQ(map->get("key1").asInt(), 30);

    // But historical cycles should be empty
    auto historicalCycles = map->getHistoricalCycles();
    EXPECT_TRUE(historicalCycles.empty());

    // Trying to restore from a cleared cycle should fail
    EXPECT_FALSE(map->restoreFromCycle(startCycle));
    EXPECT_FALSE(map->restoreFromCycle(cycle1));
}

}  // namespace chronovyan

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}