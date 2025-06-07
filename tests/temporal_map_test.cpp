#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <vector>

#include "../include/temporal_map.h"
#include "../include/temporal_runtime.h"

namespace chronovyan {
namespace {

class TemporalMapTest : public ::testing::Test {
protected:
    void SetUp() override { runtime = std::make_shared<TemporalRuntime>(); }

    std::shared_ptr<TemporalRuntime> runtime;
};

// Tests basic construction and access
TEST_F(TemporalMapTest, BasicConstruction) {
    // Create an empty map
    TemporalMap map(runtime);

    // Check initial state
    EXPECT_EQ(map.size(), 0);
    EXPECT_TRUE(map.empty());

    // Add some key-value pairs
    map.set(Value(1), Value("one"));
    map.set(Value(2), Value("two"));
    map.set(Value(3), Value("three"));

    // Check updated state
    EXPECT_EQ(map.size(), 3);
    EXPECT_FALSE(map.empty());
    EXPECT_TRUE(map.contains(Value(1)));
    EXPECT_TRUE(map.contains(Value(2)));
    EXPECT_TRUE(map.contains(Value(3)));
    EXPECT_FALSE(map.contains(Value(4)));

    // Check values
    EXPECT_EQ(map.at(Value(1)).asString(), "one");
    EXPECT_EQ(map.at(Value(2)).asString(), "two");
    EXPECT_EQ(map.at(Value(3)).asString(), "three");

    // Check exception for non-existent key
    EXPECT_THROW(map.at(Value(4)), std::out_of_range);
}

// Tests construction with initial elements
TEST_F(TemporalMapTest, ConstructionWithElements) {
    // Create a map with initial elements
    std::map<Value, Value> initialElements = {
        {Value(1), Value("one")}, {Value(2), Value("two")}, {Value(3), Value("three")}};

    TemporalMap map(initialElements, runtime);

    // Check state
    EXPECT_EQ(map.size(), 3);
    EXPECT_FALSE(map.empty());
    EXPECT_TRUE(map.contains(Value(1)));
    EXPECT_TRUE(map.contains(Value(2)));
    EXPECT_TRUE(map.contains(Value(3)));

    // Check values
    EXPECT_EQ(map.at(Value(1)).asString(), "one");
    EXPECT_EQ(map.at(Value(2)).asString(), "two");
    EXPECT_EQ(map.at(Value(3)).asString(), "three");
}

// Tests removing elements
TEST_F(TemporalMapTest, RemoveElements) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value("one"));
    map.set(Value(2), Value("two"));
    map.set(Value(3), Value("three"));

    // Remove an element
    EXPECT_TRUE(map.remove(Value(2)));

    // Check updated state
    EXPECT_EQ(map.size(), 2);
    EXPECT_TRUE(map.contains(Value(1)));
    EXPECT_FALSE(map.contains(Value(2)));
    EXPECT_TRUE(map.contains(Value(3)));

    // Try to remove a non-existent element
    EXPECT_FALSE(map.remove(Value(4)));

    // Remove all elements
    EXPECT_TRUE(map.remove(Value(1)));
    EXPECT_TRUE(map.remove(Value(3)));

    // Check final state
    EXPECT_EQ(map.size(), 0);
    EXPECT_TRUE(map.empty());
}

// Tests keys and values methods
TEST_F(TemporalMapTest, KeysAndValues) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value("one"));
    map.set(Value(2), Value("two"));
    map.set(Value(3), Value("three"));

    // Get keys
    std::vector<Value> keys = map.keys();

    // Check keys
    EXPECT_EQ(keys.size(), 3);
    EXPECT_TRUE(std::find_if(keys.begin(), keys.end(),
                             [](const Value& v) { return v.asInteger() == 1; }) != keys.end());
    EXPECT_TRUE(std::find_if(keys.begin(), keys.end(),
                             [](const Value& v) { return v.asInteger() == 2; }) != keys.end());
    EXPECT_TRUE(std::find_if(keys.begin(), keys.end(),
                             [](const Value& v) { return v.asInteger() == 3; }) != keys.end());

    // Get values
    std::vector<Value> values = map.values();

    // Check values
    EXPECT_EQ(values.size(), 3);
    EXPECT_TRUE(std::find_if(values.begin(), values.end(),
                             [](const Value& v) { return v.asString() == "one"; }) != values.end());
    EXPECT_TRUE(std::find_if(values.begin(), values.end(),
                             [](const Value& v) { return v.asString() == "two"; }) != values.end());
    EXPECT_TRUE(std::find_if(values.begin(), values.end(), [](const Value& v) {
                    return v.asString() == "three";
                }) != values.end());
}

// Tests forEach method
TEST_F(TemporalMapTest, ForEach) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value("one"));
    map.set(Value(2), Value("two"));
    map.set(Value(3), Value("three"));

    // Use forEach to sum the keys
    int keySum = 0;
    std::string valueConcat;

    map.forEach([&](const Value& key, const Value& value) {
        keySum += key.asInteger();
        valueConcat += value.asString();
    });

    // Check results
    EXPECT_EQ(keySum, 6);  // 1 + 2 + 3

    // Note: The order of concatenation is not guaranteed due to map iteration order
    EXPECT_EQ(valueConcat.length(), 11);  // "one" + "two" + "three"
    EXPECT_TRUE(valueConcat.find("one") != std::string::npos);
    EXPECT_TRUE(valueConcat.find("two") != std::string::npos);
    EXPECT_TRUE(valueConcat.find("three") != std::string::npos);
}

// Tests mapValues method
TEST_F(TemporalMapTest, MapValues) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value(10));
    map.set(Value(2), Value(20));
    map.set(Value(3), Value(30));

    // Apply a transformation (double each value)
    auto transformed = map.mapValues([](const Value& key, const Value& value) {
        return Value(static_cast<int64_t>(value.asInteger() * 2));
    });

    // Check original map is unchanged
    EXPECT_EQ(map.at(Value(1)).asInteger(), 10);
    EXPECT_EQ(map.at(Value(2)).asInteger(), 20);
    EXPECT_EQ(map.at(Value(3)).asInteger(), 30);

    // Check transformed map
    EXPECT_EQ(transformed->at(Value(1)).asInteger(), 20);
    EXPECT_EQ(transformed->at(Value(2)).asInteger(), 40);
    EXPECT_EQ(transformed->at(Value(3)).asInteger(), 60);
}

// Tests filter method
TEST_F(TemporalMapTest, Filter) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value(10));
    map.set(Value(2), Value(20));
    map.set(Value(3), Value(30));
    map.set(Value(4), Value(40));

    // Filter to keep only even keys
    auto filtered =
        map.filter([](const Value& key, const Value& value) { return key.asInteger() % 2 == 0; });

    // Check filtered map
    EXPECT_EQ(filtered->size(), 2);
    EXPECT_FALSE(filtered->contains(Value(1)));
    EXPECT_TRUE(filtered->contains(Value(2)));
    EXPECT_FALSE(filtered->contains(Value(3)));
    EXPECT_TRUE(filtered->contains(Value(4)));
    EXPECT_EQ(filtered->at(Value(2)).asInteger(), 20);
    EXPECT_EQ(filtered->at(Value(4)).asInteger(), 40);
}

// Tests merge method
TEST_F(TemporalMapTest, Merge) {
    // Create two maps
    TemporalMap map1(runtime);
    map1.set(Value(1), Value("one"));
    map1.set(Value(2), Value("two"));

    TemporalMap map2(runtime);
    map2.set(Value(2), Value("TWO"));
    map2.set(Value(3), Value("THREE"));

    // Merge without conflict resolution (map2 values take precedence)
    auto merged1 = map1.merge(map2);

    // Check merged map
    EXPECT_EQ(merged1->size(), 3);
    EXPECT_EQ(merged1->at(Value(1)).asString(), "one");
    EXPECT_EQ(merged1->at(Value(2)).asString(), "TWO");  // Overwritten by map2
    EXPECT_EQ(merged1->at(Value(3)).asString(), "THREE");

    // Merge with conflict resolution (concatenate strings)
    auto merged2 = map1.merge(map2, [](const Value& key, const Value& val1, const Value& val2) {
        return Value(val1.asString() + "+" + val2.asString());
    });

    // Check merged map with custom conflict resolution
    EXPECT_EQ(merged2->size(), 3);
    EXPECT_EQ(merged2->at(Value(1)).asString(), "one");
    EXPECT_EQ(merged2->at(Value(2)).asString(), "two+TWO");  // Combined using resolver
    EXPECT_EQ(merged2->at(Value(3)).asString(), "THREE");
}

// Tests string representation
TEST_F(TemporalMapTest, StringRepresentation) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value("one"));
    map.set(Value(2), Value("two"));

    // Get string representation
    std::string str = map.toString();

    // Check representation (exact format may vary)
    EXPECT_TRUE(str.find("1") != std::string::npos);
    EXPECT_TRUE(str.find("one") != std::string::npos);
    EXPECT_TRUE(str.find("2") != std::string::npos);
    EXPECT_TRUE(str.find("two") != std::string::npos);
}

// Tests temporal features - saving and restoring checkpoints
TEST_F(TemporalMapTest, SaveAndRestoreCheckpoints) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value("one"));
    map.set(Value(2), Value("two"));

    // Save the current state at cycle 1
    map.saveHistoryCheckpoint(1);

    // Modify the map
    map.set(Value(2), Value("TWO"));
    map.set(Value(3), Value("three"));
    map.remove(Value(1));

    // Save the current state at cycle 2
    map.saveHistoryCheckpoint(2);

    // Check current state
    EXPECT_EQ(map.size(), 2);
    EXPECT_FALSE(map.contains(Value(1)));
    EXPECT_TRUE(map.contains(Value(2)));
    EXPECT_TRUE(map.contains(Value(3)));

    // Restore from cycle 1
    EXPECT_TRUE(map.restoreFromCycle(1));

    // Check restored state
    EXPECT_EQ(map.size(), 2);
    EXPECT_TRUE(map.contains(Value(1)));
    EXPECT_TRUE(map.contains(Value(2)));
    EXPECT_FALSE(map.contains(Value(3)));
    EXPECT_EQ(map.at(Value(1)).asString(), "one");
    EXPECT_EQ(map.at(Value(2)).asString(), "two");

    // Restore from cycle 2
    EXPECT_TRUE(map.restoreFromCycle(2));

    // Check restored state
    EXPECT_EQ(map.size(), 2);
    EXPECT_FALSE(map.contains(Value(1)));
    EXPECT_TRUE(map.contains(Value(2)));
    EXPECT_TRUE(map.contains(Value(3)));
    EXPECT_EQ(map.at(Value(2)).asString(), "TWO");
    EXPECT_EQ(map.at(Value(3)).asString(), "three");

    // Try to restore from a non-existent cycle
    EXPECT_FALSE(map.restoreFromCycle(3));

    // Check historical cycles
    std::vector<int> cycles = map.getHistoricalCycles();
    EXPECT_EQ(cycles.size(), 2);
    EXPECT_TRUE(std::find(cycles.begin(), cycles.end(), 1) != cycles.end());
    EXPECT_TRUE(std::find(cycles.begin(), cycles.end(), 2) != cycles.end());

    // Clear history
    map.clearHistory();
    EXPECT_TRUE(map.getHistoricalCycles().empty());
}

// Tests accessing values at specific cycles
TEST_F(TemporalMapTest, AccessAtCycle) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value("one"));
    map.set(Value(2), Value("two"));

    // Save the current state at cycle 1
    map.saveHistoryCheckpoint(1);

    // Modify the map
    map.set(Value(1), Value("ONE"));
    map.set(Value(3), Value("three"));

    // Save the current state at cycle 2
    map.saveHistoryCheckpoint(2);

    // Access values at different cycles
    EXPECT_EQ(map.getAtCycle(Value(1), 1).asString(), "one");
    EXPECT_EQ(map.getAtCycle(Value(2), 1).asString(), "two");
    EXPECT_THROW(map.getAtCycle(Value(3), 1), std::out_of_range);

    EXPECT_EQ(map.getAtCycle(Value(1), 2).asString(), "ONE");
    EXPECT_EQ(map.getAtCycle(Value(2), 2).asString(), "two");
    EXPECT_EQ(map.getAtCycle(Value(3), 2).asString(), "three");

    // Check containment at different cycles
    EXPECT_TRUE(map.containedAtCycle(Value(1), 1));
    EXPECT_TRUE(map.containedAtCycle(Value(2), 1));
    EXPECT_FALSE(map.containedAtCycle(Value(3), 1));

    EXPECT_TRUE(map.containedAtCycle(Value(1), 2));
    EXPECT_TRUE(map.containedAtCycle(Value(2), 2));
    EXPECT_TRUE(map.containedAtCycle(Value(3), 2));

    // Get keys at different cycles
    std::vector<Value> keysAtCycle1 = map.keysAtCycle(1);
    EXPECT_EQ(keysAtCycle1.size(), 2);
    EXPECT_TRUE(std::find_if(keysAtCycle1.begin(), keysAtCycle1.end(), [](const Value& v) {
                    return v.asInteger() == 1;
                }) != keysAtCycle1.end());
    EXPECT_TRUE(std::find_if(keysAtCycle1.begin(), keysAtCycle1.end(), [](const Value& v) {
                    return v.asInteger() == 2;
                }) != keysAtCycle1.end());

    std::vector<Value> keysAtCycle2 = map.keysAtCycle(2);
    EXPECT_EQ(keysAtCycle2.size(), 3);
    EXPECT_TRUE(std::find_if(keysAtCycle2.begin(), keysAtCycle2.end(), [](const Value& v) {
                    return v.asInteger() == 1;
                }) != keysAtCycle2.end());
    EXPECT_TRUE(std::find_if(keysAtCycle2.begin(), keysAtCycle2.end(), [](const Value& v) {
                    return v.asInteger() == 2;
                }) != keysAtCycle2.end());
    EXPECT_TRUE(std::find_if(keysAtCycle2.begin(), keysAtCycle2.end(), [](const Value& v) {
                    return v.asInteger() == 3;
                }) != keysAtCycle2.end());
}

// Tests tracking key history
TEST_F(TemporalMapTest, TrackKeyHistory) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value(10));

    // Save checkpoints at different cycles
    map.saveHistoryCheckpoint(1);

    map.set(Value(1), Value(20));
    map.saveHistoryCheckpoint(2);

    map.set(Value(1), Value(30));
    map.saveHistoryCheckpoint(3);

    map.set(Value(1), Value(40));
    map.saveHistoryCheckpoint(4);

    // Track history of key 1 from cycle 1 to 4
    auto history = map.trackKeyHistory(Value(1), 1, 4);

    // Check history
    EXPECT_EQ(history.size(), 4);
    EXPECT_EQ(history[1].asInteger(), 10);
    EXPECT_EQ(history[2].asInteger(), 20);
    EXPECT_EQ(history[3].asInteger(), 30);
    EXPECT_EQ(history[4].asInteger(), 40);

    // Track history with missing cycles
    auto partialHistory = map.trackKeyHistory(Value(1), 2, 3);
    EXPECT_EQ(partialHistory.size(), 2);
    EXPECT_EQ(partialHistory[2].asInteger(), 20);
    EXPECT_EQ(partialHistory[3].asInteger(), 30);

    // Track history of a key that doesn't exist at some cycles
    map.set(Value(2), Value(100));
    map.saveHistoryCheckpoint(5);

    auto sparseHistory = map.trackKeyHistory(Value(2), 1, 5);
    EXPECT_EQ(sparseHistory.size(), 1);
    EXPECT_EQ(sparseHistory[5].asInteger(), 100);
}

// Tests finding frequently changed keys
TEST_F(TemporalMapTest, FrequentlyChangedKeys) {
    // Create a map with elements
    TemporalMap map(runtime);
    map.set(Value(1), Value(10));
    map.set(Value(2), Value(20));
    map.set(Value(3), Value(30));

    // Save checkpoint
    map.saveHistoryCheckpoint(1);

    // Change key 1 multiple times
    map.set(Value(1), Value(11));
    map.saveHistoryCheckpoint(2);

    map.set(Value(1), Value(12));
    map.saveHistoryCheckpoint(3);

    // Change key 2 once
    map.set(Value(2), Value(21));
    map.saveHistoryCheckpoint(4);

    // Key 3 remains unchanged

    // Find frequently changed keys
    auto changedKeys = map.getFrequentlyChangedKeys(1, 4);

    // Check results
    EXPECT_EQ(changedKeys.size(), 3);
    EXPECT_EQ(changedKeys[Value(1)], 2);  // Changed twice (10->11->12)
    EXPECT_EQ(changedKeys[Value(2)], 1);  // Changed once (20->21)
    EXPECT_EQ(changedKeys[Value(3)], 0);  // Never changed

    // Test with limit
    auto limitedChangedKeys = map.getFrequentlyChangedKeys(1, 4, 1);
    EXPECT_EQ(limitedChangedKeys.size(), 1);
    EXPECT_EQ(limitedChangedKeys[Value(1)], 2);  // Only the most frequently changed key
}

// Tests pattern detection
TEST_F(TemporalMapTest, PatternDetection) {
    // Create a map with elements
    TemporalMap map(runtime);

    // Test increasing pattern
    map.set(Value("increasing"), Value(10));
    map.saveHistoryCheckpoint(1);

    map.set(Value("increasing"), Value(20));
    map.saveHistoryCheckpoint(2);

    map.set(Value("increasing"), Value(30));
    map.saveHistoryCheckpoint(3);

    // Test decreasing pattern
    map.set(Value("decreasing"), Value(30));
    map.saveHistoryCheckpoint(4);

    map.set(Value("decreasing"), Value(20));
    map.saveHistoryCheckpoint(5);

    map.set(Value("decreasing"), Value(10));
    map.saveHistoryCheckpoint(6);

    // Test cyclic pattern
    map.set(Value("cyclic"), Value(10));
    map.saveHistoryCheckpoint(7);

    map.set(Value("cyclic"), Value(20));
    map.saveHistoryCheckpoint(8);

    map.set(Value("cyclic"), Value(10));
    map.saveHistoryCheckpoint(9);

    map.set(Value("cyclic"), Value(20));
    map.saveHistoryCheckpoint(10);

    // Test stable pattern
    map.set(Value("stable"), Value("constant"));
    map.saveHistoryCheckpoint(11);

    map.set(Value("stable"), Value("constant"));
    map.saveHistoryCheckpoint(12);

    map.set(Value("stable"), Value("constant"));
    map.saveHistoryCheckpoint(13);

    // Detect patterns
    std::string increasingPattern = map.detectKeyPattern(Value("increasing"), {1, 2, 3});
    std::string decreasingPattern = map.detectKeyPattern(Value("decreasing"), {4, 5, 6});
    std::string cyclicPattern = map.detectKeyPattern(Value("cyclic"), {7, 8, 9, 10});
    std::string stablePattern = map.detectKeyPattern(Value("stable"), {11, 12, 13});

    // Check detected patterns
    EXPECT_EQ(increasingPattern, "increasing");
    EXPECT_EQ(decreasingPattern, "decreasing");
    EXPECT_EQ(cyclicPattern, "cyclic");
    EXPECT_EQ(stablePattern, "stable");

    // Test insufficient data
    std::string insufficientPattern = map.detectKeyPattern(Value("nonexistent"), {1, 2});
    EXPECT_EQ(insufficientPattern, "insufficient_data");
}

// Tests future value prediction
TEST_F(TemporalMapTest, FutureValuePrediction) {
    // Create a map with elements
    TemporalMap map(runtime);

    // Linear pattern for key 1
    map.set(Value(1), Value(10));
    map.saveHistoryCheckpoint(1);

    map.set(Value(1), Value(20));
    map.saveHistoryCheckpoint(2);

    map.set(Value(1), Value(30));
    map.saveHistoryCheckpoint(3);

    // Non-numeric values for key 2
    map.set(Value(2), Value("first"));
    map.saveHistoryCheckpoint(4);

    map.set(Value(2), Value("second"));
    map.saveHistoryCheckpoint(5);

    map.set(Value(2), Value("third"));
    map.saveHistoryCheckpoint(6);

    // Predict future values
    Value predictedNumeric = map.predictFutureValue(Value(1), 4, {1, 2, 3});
    Value predictedNonNumeric = map.predictFutureValue(Value(2), 7, {4, 5, 6});

    // Check predictions
    EXPECT_TRUE(predictedNumeric.isInteger());
    EXPECT_EQ(predictedNumeric.asInteger(), 40);  // Linearly extrapolated

    EXPECT_TRUE(predictedNonNumeric.isString());
    EXPECT_EQ(predictedNonNumeric.asString(), "third");  // Most recent value

    // Test with insufficient data
    Value insufficientPrediction = map.predictFutureValue(Value(3), 1, {});
    EXPECT_TRUE(insufficientPrediction.isNil());
}

// Tests resource tracking
TEST_F(TemporalMapTest, ResourceTracking) {
    // Create a map with runtime
    TemporalMap map(runtime);

    // Initial chronons should be 0
    EXPECT_EQ(runtime->getChrononsUsed(), 1);  // 1 for initialization

    // Perform operations and check chronons usage
    map.set(Value(1), Value(10));
    int chrononsAfterSet = runtime->getChrononsUsed();
    EXPECT_GT(chrononsAfterSet, 1);

    map.at(Value(1));
    int chrononsAfterAt = runtime->getChrononsUsed();
    EXPECT_EQ(chrononsAfterAt, chrononsAfterSet);  // at doesn't track resources

    map.saveHistoryCheckpoint(1);
    int chrononsAfterSave = runtime->getChrononsUsed();
    EXPECT_GT(chrononsAfterSave, chrononsAfterAt);

    // Test setting and getting runtime
    auto newRuntime = std::make_shared<TemporalRuntime>();
    map.setTemporalRuntime(newRuntime);
    EXPECT_EQ(map.getTemporalRuntime(), newRuntime);
}

}  // namespace
}  // namespace chronovyan