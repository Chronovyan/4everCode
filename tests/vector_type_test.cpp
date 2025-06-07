#include <gtest/gtest.h>

#include <algorithm>
#include <memory>

#include "../include/temporal_runtime.h"
#include "../include/value.h"
#include "../include/vector_type.h"

namespace chronovyan {

class VectorTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        runtime = std::make_shared<TemporalRuntime>();
        vector = std::make_shared<ChronovyanVector>(runtime);
    }

    std::shared_ptr<TemporalRuntime> runtime;
    std::shared_ptr<ChronovyanVector> vector;
};

// Test basic vector operations
TEST_F(VectorTypeTest, BasicOperations) {
    // Test empty vector
    EXPECT_TRUE(vector->empty());
    EXPECT_EQ(vector->size(), 0);

    // Test push_back and at with integer values
    vector->push_back(Value(static_cast<int64_t>(42)));
    EXPECT_FALSE(vector->empty());
    EXPECT_EQ(vector->size(), 1);
    EXPECT_EQ(vector->at(0).asInteger(), 42);

    // Test push_back with multiple values
    vector->push_back(Value(static_cast<int64_t>(100)));
    vector->push_back(Value(static_cast<int64_t>(200)));
    EXPECT_EQ(vector->size(), 3);
    EXPECT_EQ(vector->at(1).asInteger(), 100);
    EXPECT_EQ(vector->at(2).asInteger(), 200);

    // Test pop_back
    vector->pop_back();
    EXPECT_EQ(vector->size(), 2);

    // Test insert
    vector->insert(1, Value(static_cast<int64_t>(150)));
    EXPECT_EQ(vector->size(), 3);
    EXPECT_EQ(vector->at(1).asInteger(), 150);

    // Test erase
    vector->erase(0);
    EXPECT_EQ(vector->size(), 2);
    EXPECT_EQ(vector->at(0).asInteger(), 150);

    // Test clear
    vector->clear();
    EXPECT_TRUE(vector->empty());
    EXPECT_EQ(vector->size(), 0);
}

// Test error conditions
TEST_F(VectorTypeTest, ErrorConditions) {
    // Test at with invalid index
    EXPECT_THROW(vector->at(0), std::out_of_range);

    // Test pop_back on empty vector
    EXPECT_THROW(vector->pop_back(), std::out_of_range);

    // Test erase with invalid index
    EXPECT_THROW(vector->erase(0), std::out_of_range);

    // Add some elements for further tests
    vector->push_back(Value(static_cast<int64_t>(10)));
    vector->push_back(Value(static_cast<int64_t>(20)));

    // Test slice with invalid indices
    EXPECT_THROW(vector->slice(0, 3), std::out_of_range);
    EXPECT_THROW(vector->slice(2, 1), std::out_of_range);
}

// Test advanced vector operations
TEST_F(VectorTypeTest, AdvancedOperations) {
    // Prepare vector with values
    vector->push_back(Value(static_cast<int64_t>(10)));
    vector->push_back(Value(static_cast<int64_t>(20)));
    vector->push_back(Value(static_cast<int64_t>(30)));
    vector->push_back(Value(static_cast<int64_t>(40)));

    // Test slice
    auto sliced = vector->slice(1, 3);
    EXPECT_EQ(sliced->size(), 2);
    EXPECT_EQ(sliced->at(0).asInteger(), 20);
    EXPECT_EQ(sliced->at(1).asInteger(), 30);

    // Test concat
    auto other = std::make_shared<ChronovyanVector>(runtime);
    other->push_back(Value(static_cast<int64_t>(50)));
    other->push_back(Value(static_cast<int64_t>(60)));

    auto concatenated = vector->concat(*other);
    EXPECT_EQ(concatenated->size(), 6);
    EXPECT_EQ(concatenated->at(4).asInteger(), 50);
    EXPECT_EQ(concatenated->at(5).asInteger(), 60);

    // Test map (double each value)
    auto mapped =
        vector->map([](const Value& v) { return Value(static_cast<int64_t>(v.asInteger() * 2)); });

    EXPECT_EQ(mapped->size(), 4);
    EXPECT_EQ(mapped->at(0).asInteger(), 20);
    EXPECT_EQ(mapped->at(1).asInteger(), 40);
    EXPECT_EQ(mapped->at(2).asInteger(), 60);
    EXPECT_EQ(mapped->at(3).asInteger(), 80);

    // Test filter (keep only values > 20)
    auto filtered = vector->filter([](const Value& v) { return v.asInteger() > 20; });

    EXPECT_EQ(filtered->size(), 2);
    EXPECT_EQ(filtered->at(0).asInteger(), 30);
    EXPECT_EQ(filtered->at(1).asInteger(), 40);

    // Test reduce (sum all values)
    auto sum = vector->reduce(
        [](const Value& acc, const Value& curr) {
            return Value(static_cast<int64_t>(acc.asInteger() + curr.asInteger()));
        },
        Value(static_cast<int64_t>(0)));

    EXPECT_EQ(sum.asInteger(), 100);  // 10 + 20 + 30 + 40 = 100

    // Test sort (descending order)
    vector->sort([](const Value& a, const Value& b) { return a.asInteger() > b.asInteger(); });

    EXPECT_EQ(vector->at(0).asInteger(), 40);
    EXPECT_EQ(vector->at(1).asInteger(), 30);
    EXPECT_EQ(vector->at(2).asInteger(), 20);
    EXPECT_EQ(vector->at(3).asInteger(), 10);

    // Test indexOf
    vector->clear();
    vector->push_back(Value(static_cast<int64_t>(10)));
    vector->push_back(Value(static_cast<int64_t>(20)));
    vector->push_back(Value(static_cast<int64_t>(30)));

    EXPECT_EQ(vector->indexOf(Value(static_cast<int64_t>(20))), 1);
    EXPECT_EQ(vector->indexOf(Value(static_cast<int64_t>(50))), -1);  // Not found

    // Test contains
    EXPECT_TRUE(vector->contains(Value(static_cast<int64_t>(30))));
    EXPECT_FALSE(vector->contains(Value(static_cast<int64_t>(50))));

    // Test join
    std::string joined = vector->join(", ");
    EXPECT_EQ(joined, "10, 20, 30");

    // Test toString
    std::string str = vector->toString();
    EXPECT_EQ(str, "[10, 20, 30]");
}

// Test temporal features
TEST_F(VectorTypeTest, TemporalFeatures) {
    // Get initial cycle
    int startCycle = static_cast<int>(runtime->getCycleNumber());

    // Set initial state and save checkpoint
    vector->push_back(Value(static_cast<int64_t>(10)));
    vector->push_back(Value(static_cast<int64_t>(20)));
    vector->saveHistoryCheckpoint(startCycle);

    // Change state at next cycle
    runtime->consumeChronons(1);
    int cycle1 = static_cast<int>(runtime->getCycleNumber());
    vector->push_back(Value(static_cast<int64_t>(30)));
    vector->saveHistoryCheckpoint(cycle1);

    // Change state at next cycle
    runtime->consumeChronons(1);
    int cycle2 = static_cast<int>(runtime->getCycleNumber());
    vector->at(0) = Value(static_cast<int64_t>(15));  // Update first element
    vector->saveHistoryCheckpoint(cycle2);

    // Current state should have 3 elements with [15, 20, 30]
    EXPECT_EQ(vector->size(), 3);
    EXPECT_EQ(vector->at(0).asInteger(), 15);

    // Verify we can restore from different cycles
    // First, restore from cycle1 (should have [10, 20, 30])
    EXPECT_TRUE(vector->restoreFromCycle(cycle1));
    EXPECT_EQ(vector->size(), 3);
    EXPECT_EQ(vector->at(0).asInteger(), 10);
    EXPECT_EQ(vector->at(2).asInteger(), 30);

    // Restore from startCycle (should have [10, 20])
    EXPECT_TRUE(vector->restoreFromCycle(startCycle));
    EXPECT_EQ(vector->size(), 2);
    EXPECT_EQ(vector->at(0).asInteger(), 10);
    EXPECT_EQ(vector->at(1).asInteger(), 20);

    // Restore from cycle2 (should have [15, 20, 30])
    EXPECT_TRUE(vector->restoreFromCycle(cycle2));
    EXPECT_EQ(vector->size(), 3);
    EXPECT_EQ(vector->at(0).asInteger(), 15);

    // Check that we can get values at specific cycles
    EXPECT_EQ(vector->getAtCycle(0, startCycle).asInteger(), 10);
    EXPECT_EQ(vector->getAtCycle(0, cycle2).asInteger(), 15);

    // Check historical cycles
    auto historicalCycles = vector->getHistoricalCycles();
    EXPECT_EQ(historicalCycles.size(), 3);
    EXPECT_TRUE(std::find(historicalCycles.begin(), historicalCycles.end(), startCycle) !=
                historicalCycles.end());
    EXPECT_TRUE(std::find(historicalCycles.begin(), historicalCycles.end(), cycle1) !=
                historicalCycles.end());
    EXPECT_TRUE(std::find(historicalCycles.begin(), historicalCycles.end(), cycle2) !=
                historicalCycles.end());

    // Test clearing history
    vector->clearHistory();

    // Should still have the current state after clearing history
    EXPECT_EQ(vector->size(), 3);
    EXPECT_EQ(vector->at(0).asInteger(), 15);

    // But historical cycles should be empty
    historicalCycles = vector->getHistoricalCycles();
    EXPECT_TRUE(historicalCycles.empty());

    // Trying to restore from a cleared cycle should fail
    EXPECT_FALSE(vector->restoreFromCycle(startCycle));
    EXPECT_FALSE(vector->restoreFromCycle(cycle1));
}

}  // namespace chronovyan

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}