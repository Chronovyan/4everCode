#include <gtest/gtest.h>

#include <cmath>
#include <functional>
#include <memory>

#include "../include/temporal_runtime.h"
#include "../include/temporal_sequence.h"

namespace chronovyan {
namespace testing {

class TemporalSequenceTest : public ::testing::Test {
protected:
    void SetUp() override {
        runtime = std::make_shared<TemporalRuntime>();
        sequence = std::make_shared<TemporalSequence>(runtime);
    }

    std::shared_ptr<TemporalRuntime> runtime;
    std::shared_ptr<TemporalSequence> sequence;
};

// Basic operations tests
TEST_F(TemporalSequenceTest, BasicOperations) {
    // Test empty sequence
    EXPECT_TRUE(sequence->empty());
    EXPECT_EQ(sequence->size(), 0);

    // Test adding points
    sequence->set(1, Value(static_cast<int64_t>(100)));
    EXPECT_FALSE(sequence->empty());
    EXPECT_EQ(sequence->size(), 1);

    sequence->set(5, Value(static_cast<int64_t>(500)));
    sequence->set(3, Value(static_cast<int64_t>(300)));
    EXPECT_EQ(sequence->size(), 3);

    // Test accessing values
    EXPECT_EQ(sequence->at(1).asInteger(), 100);
    EXPECT_EQ(sequence->at(3).asInteger(), 300);
    EXPECT_EQ(sequence->at(5).asInteger(), 500);

    // Test hasCycle
    EXPECT_TRUE(sequence->hasCycle(1));
    EXPECT_TRUE(sequence->hasCycle(3));
    EXPECT_TRUE(sequence->hasCycle(5));
    EXPECT_FALSE(sequence->hasCycle(2));

    // Test updating values
    sequence->set(3, Value(static_cast<int64_t>(350)));
    EXPECT_EQ(sequence->at(3).asInteger(), 350);

    // Test removing values
    bool removed = sequence->remove(3);
    EXPECT_TRUE(removed);
    EXPECT_FALSE(sequence->hasCycle(3));
    EXPECT_EQ(sequence->size(), 2);

    // Test removing non-existent cycle
    removed = sequence->remove(10);
    EXPECT_FALSE(removed);
    EXPECT_EQ(sequence->size(), 2);

    // Test getting all cycles
    std::vector<int> cycles = sequence->getCycles();
    EXPECT_EQ(cycles.size(), 2);
    EXPECT_EQ(cycles[0], 1);
    EXPECT_EQ(cycles[1], 5);

    // Test clearing
    sequence->clear();
    EXPECT_TRUE(sequence->empty());
    EXPECT_EQ(sequence->size(), 0);
}

// Test for cycle range and boundary functions
TEST_F(TemporalSequenceTest, CycleRangeFunctions) {
    // Empty sequence
    EXPECT_EQ(sequence->getEarliestCycle(), -1);
    EXPECT_EQ(sequence->getLatestCycle(), -1);

    // Add points
    sequence->set(10, Value(static_cast<int64_t>(1000)));
    EXPECT_EQ(sequence->getEarliestCycle(), 10);
    EXPECT_EQ(sequence->getLatestCycle(), 10);

    sequence->set(5, Value(static_cast<int64_t>(500)));
    EXPECT_EQ(sequence->getEarliestCycle(), 5);
    EXPECT_EQ(sequence->getLatestCycle(), 10);

    sequence->set(15, Value(static_cast<int64_t>(1500)));
    EXPECT_EQ(sequence->getEarliestCycle(), 5);
    EXPECT_EQ(sequence->getLatestCycle(), 15);

    // Test slice
    auto sliced = sequence->slice(7, 12);
    EXPECT_EQ(sliced->size(), 1);
    EXPECT_TRUE(sliced->hasCycle(10));

    // Test out of bounds access
    EXPECT_THROW(sequence->at(20), std::out_of_range);
}

// Test interpolation
TEST_F(TemporalSequenceTest, Interpolation) {
    // Add numeric points
    sequence->set(10, Value(static_cast<int64_t>(100)));
    sequence->set(20, Value(static_cast<int64_t>(200)));

    // Step interpolation
    Value step = sequence->interpolate(15, "step");
    EXPECT_EQ(step.asInteger(), 100);

    // Linear interpolation
    Value linear = sequence->interpolate(15, "linear");
    EXPECT_EQ(linear.asInteger(), 150);

    // Spline interpolation (currently same as linear)
    Value spline = sequence->interpolate(15, "spline");
    EXPECT_EQ(spline.asInteger(), 150);

    // Default method (step)
    Value def = sequence->interpolate(15, "unknown");
    EXPECT_EQ(def.asInteger(), 100);

    // Float values
    sequence->clear();
    sequence->set(10, Value(10.0));
    sequence->set(20, Value(20.0));

    Value linearFloat = sequence->interpolate(15, "linear");
    EXPECT_NEAR(linearFloat.asFloat(), 15.0, 0.001);

    // Non-numeric values
    sequence->clear();
    sequence->set(10, Value(std::string("hello")));
    sequence->set(20, Value(std::string("world")));

    Value nonNumeric = sequence->interpolate(15, "linear");
    EXPECT_EQ(nonNumeric.toString(), "hello");

    // Edge cases
    sequence->clear();
    sequence->set(10, Value(static_cast<int64_t>(100)));
    sequence->set(20, Value(static_cast<int64_t>(200)));

    // Before earliest
    Value before = sequence->interpolate(5, "linear");
    EXPECT_EQ(before.asInteger(), 100);

    // After latest
    Value after = sequence->interpolate(25, "linear");
    EXPECT_EQ(after.asInteger(), 200);

    // Empty sequence
    sequence->clear();
    Value empty = sequence->interpolate(10, "linear");
    EXPECT_TRUE(empty.isNil());
}

// Test resampling
TEST_F(TemporalSequenceTest, Resampling) {
    // Create a sequence with points at irregular intervals
    sequence->set(5, Value(static_cast<int64_t>(50)));
    sequence->set(10, Value(static_cast<int64_t>(100)));
    sequence->set(20, Value(static_cast<int64_t>(200)));

    // Resample with interval of 5
    auto resampled = sequence->resample(5, "linear");
    EXPECT_EQ(resampled->size(), 4);  // Points at 5, 10, 15, 20
    EXPECT_TRUE(resampled->hasCycle(5));
    EXPECT_TRUE(resampled->hasCycle(10));
    EXPECT_TRUE(resampled->hasCycle(15));
    EXPECT_TRUE(resampled->hasCycle(20));

    EXPECT_EQ(resampled->at(5).asInteger(), 50);
    EXPECT_EQ(resampled->at(10).asInteger(), 100);
    EXPECT_EQ(resampled->at(15).asInteger(), 150);  // Interpolated
    EXPECT_EQ(resampled->at(20).asInteger(), 200);

    // Empty sequence
    sequence->clear();
    auto emptyResampled = sequence->resample(5, "linear");
    EXPECT_TRUE(emptyResampled->empty());
}

// Test functional operations
TEST_F(TemporalSequenceTest, FunctionalOperations) {
    // Set up sequence
    sequence->set(1, Value(static_cast<int64_t>(10)));
    sequence->set(2, Value(static_cast<int64_t>(20)));
    sequence->set(3, Value(static_cast<int64_t>(30)));
    sequence->set(4, Value(static_cast<int64_t>(40)));
    sequence->set(5, Value(static_cast<int64_t>(50)));

    // Test map - double each value
    auto doubled =
        sequence->map([](int cycle, const Value& val) { return Value(val.asInteger() * 2); });

    EXPECT_EQ(doubled->size(), 5);
    EXPECT_EQ(doubled->at(1).asInteger(), 20);
    EXPECT_EQ(doubled->at(5).asInteger(), 100);

    // Test filter - only even numbers
    auto evens =
        sequence->filter([](int cycle, const Value& val) { return val.asInteger() % 20 == 0; });

    EXPECT_EQ(evens->size(), 2);
    EXPECT_TRUE(evens->hasCycle(2));
    EXPECT_TRUE(evens->hasCycle(4));

    // Test reduce - sum all values
    Value sum = sequence->reduce(
        [](const Value& acc, int cycle, const Value& val) {
            return Value(static_cast<int64_t>(acc.asInteger() + val.asInteger()));
        },
        Value(static_cast<int64_t>(0)));

    EXPECT_EQ(sum.asInteger(), 150);  // 10 + 20 + 30 + 40 + 50
}

// Test merging sequences
TEST_F(TemporalSequenceTest, MergingSequences) {
    // First sequence: 1->10, 3->30, 5->50
    sequence->set(1, Value(static_cast<int64_t>(10)));
    sequence->set(3, Value(static_cast<int64_t>(30)));
    sequence->set(5, Value(static_cast<int64_t>(50)));

    // Second sequence: 2->20, 3->35, 6->60
    auto sequence2 = std::make_shared<TemporalSequence>(runtime);
    sequence2->set(2, Value(static_cast<int64_t>(20)));
    sequence2->set(3, Value(static_cast<int64_t>(35)));
    sequence2->set(6, Value(static_cast<int64_t>(60)));

    // Test merge with "replace" strategy
    auto merged1 = sequence->merge(*sequence2, "replace");
    EXPECT_EQ(merged1->size(), 5);              // 1, 2, 3, 5, 6
    EXPECT_EQ(merged1->at(3).asInteger(), 35);  // Second sequence value takes precedence

    // Test merge with "keep" strategy
    auto merged2 = sequence->merge(*sequence2, "keep");
    EXPECT_EQ(merged2->size(), 5);              // 1, 2, 3, 5, 6
    EXPECT_EQ(merged2->at(3).asInteger(), 30);  // First sequence value is kept

    // Test merge with "combine" strategy
    auto merged3 = sequence->merge(*sequence2, "combine");
    EXPECT_EQ(merged3->size(), 5);              // 1, 2, 3, 5, 6
    EXPECT_EQ(merged3->at(3).asInteger(), 65);  // 30 + 35 = 65
}

// Test statistics
TEST_F(TemporalSequenceTest, Statistics) {
    // Set up sequence
    sequence->set(1, Value(10.0));
    sequence->set(2, Value(20.0));
    sequence->set(3, Value(30.0));
    sequence->set(4, Value(40.0));
    sequence->set(5, Value(50.0));

    auto stats = sequence->statistics();

    EXPECT_EQ(stats["min_cycle"].asInteger(), 1);
    EXPECT_EQ(stats["max_cycle"].asInteger(), 5);
    EXPECT_EQ(stats["cycle_range"].asInteger(), 4);
    EXPECT_EQ(stats["point_count"].asInteger(), 5);

    EXPECT_EQ(stats["min_value"].asFloat(), 10.0);
    EXPECT_EQ(stats["max_value"].asFloat(), 50.0);
    EXPECT_EQ(stats["mean"].asFloat(), 30.0);
    EXPECT_NEAR(stats["std_dev"].asFloat(), 14.142, 0.001);

    // Empty sequence
    sequence->clear();
    auto emptyStats = sequence->statistics();
    EXPECT_TRUE(emptyStats.empty());
}

// Test pattern detection and projection
TEST_F(TemporalSequenceTest, PatternDetectionAndProjection) {
    // Set up a sequence with a repeating pattern
    sequence->set(1, Value(static_cast<int64_t>(10)));
    sequence->set(2, Value(static_cast<int64_t>(20)));
    sequence->set(3, Value(static_cast<int64_t>(30)));
    sequence->set(4, Value(static_cast<int64_t>(20)));
    sequence->set(5, Value(static_cast<int64_t>(10)));

    // Pattern detection currently just returns the full sequence as a pattern
    auto patterns = sequence->detectPatterns();
    EXPECT_EQ(patterns.size(), 1);

    // Projection should repeat the pattern
    auto projected = sequence->project(1);
    EXPECT_GT(projected->size(), sequence->size());

    // Empty sequence
    sequence->clear();
    auto emptyPatterns = sequence->detectPatterns();
    EXPECT_TRUE(emptyPatterns.empty());

    auto emptyProjected = sequence->project(1);
    EXPECT_TRUE(emptyProjected->empty());
}

// Test string representation
TEST_F(TemporalSequenceTest, StringRepresentation) {
    sequence->set(1, Value(static_cast<int64_t>(10)));
    sequence->set(2, Value(static_cast<int64_t>(20)));

    std::string str = sequence->toString();
    EXPECT_NE(str.find("TemporalSequence"), std::string::npos);
    EXPECT_NE(str.find("(1:10)"), std::string::npos);
    EXPECT_NE(str.find("(2:20)"), std::string::npos);
}

// Test runtime integration
TEST_F(TemporalSequenceTest, RuntimeIntegration) {
    // Get initial chronons
    int64_t initialChronons = runtime->getChronons();

    // Perform operations that definitely consume chronons
    for (int i = 0; i < 10; i++) {
        sequence->set(i, Value(static_cast<int64_t>(i * 10)));
    }

    // Resource tracking should have consumed chronons
    EXPECT_GT(initialChronons, runtime->getChronons());

    // Change the runtime
    auto newRuntime = std::make_shared<TemporalRuntime>();
    int64_t newInitialChronons = newRuntime->getChronons();
    sequence->setTemporalRuntime(newRuntime);
    EXPECT_EQ(sequence->getTemporalRuntime(), newRuntime);

    // Test operations with new runtime
    sequence->clear();
    EXPECT_GT(newInitialChronons, newRuntime->getChronons());
}

// Edge cases and error handling
TEST_F(TemporalSequenceTest, EdgeCasesAndErrorHandling) {
    // Slice with invalid range
    EXPECT_THROW(sequence->slice(10, 5), std::invalid_argument);

    // Create with vector of points
    std::vector<TemporalSequence::TimePoint> points = {{5, Value(static_cast<int64_t>(50))},
                                                       {3, Value(static_cast<int64_t>(30))},
                                                       {1, Value(static_cast<int64_t>(10))}};

    auto customSequence = std::make_shared<TemporalSequence>(points, runtime);
    EXPECT_EQ(customSequence->size(), 3);

    // Points should be sorted
    EXPECT_EQ(customSequence->getCycles()[0], 1);
    EXPECT_EQ(customSequence->getCycles()[1], 3);
    EXPECT_EQ(customSequence->getCycles()[2], 5);
}

}  // namespace testing
}  // namespace chronovyan