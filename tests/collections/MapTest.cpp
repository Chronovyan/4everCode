#include <gtest/gtest.h>

#include "chronovyan/stdlib/collections/Map.h"
#include "chronovyan/stdlib/core/Core.h"

using namespace chronovyan::stdlib;
using namespace chronovyan::stdlib::collections;

class MapTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test data
        emptyMap = Map::create();

        // Create a map with some initial values
        testMap = Map::create();
        testMap->set("one", Number::create(1));
        testMap->set("two", Number::create(2));
        testMap->set("three", Number::create(3));
    }

    MapPtr emptyMap;
    MapPtr testMap;
};

TEST_F(MapTest, TestEmptyMap) {
    EXPECT_TRUE(emptyMap->empty());
    EXPECT_EQ(emptyMap->size(), 0);
    EXPECT_FALSE(emptyMap->has("nonexistent"));
}

TEST_F(MapTest, TestSetAndGet) {
    emptyMap->set("test", Number::create(42));
    EXPECT_FALSE(emptyMap->empty());
    EXPECT_EQ(emptyMap->size(), 1);

    auto value = emptyMap->get("test");
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->type(), Type::NUMBER);
    EXPECT_EQ(std::static_pointer_cast<Number>(value)->value(), 42);
}

TEST_F(MapTest, TestHas) {
    EXPECT_TRUE(testMap->has("one"));
    EXPECT_TRUE(testMap->has("two"));
    EXPECT_TRUE(testMap->has("three"));
    EXPECT_FALSE(testMap->has("four"));
}

TEST_F(MapTest, TestRemove) {
    EXPECT_TRUE(testMap->has("one"));
    testMap->remove("one");
    EXPECT_FALSE(testMap->has("one"));

    // Removing non-existent key should not throw
    EXPECT_NO_THROW(testMap->remove("nonexistent"));
}

TEST_F(MapTest, TestClear) {
    EXPECT_FALSE(testMap->empty());
    testMap->clear();
    EXPECT_TRUE(testMap->empty());
    EXPECT_EQ(testMap->size(), 0);
}

TEST_F(MapTest, TestKeys) {
    auto keys = testMap->keys();
    EXPECT_EQ(keys.size(), 3);

    // Check that all expected keys are present
    std::set<std::string> keySet(keys.begin(), keys.end());
    EXPECT_EQ(keySet.size(), 3);
    EXPECT_NE(keySet.find("one"), keySet.end());
    EXPECT_NE(keySet.find("two"), keySet.end());
    EXPECT_NE(keySet.find("three"), keySet.end());
}

TEST_F(MapTest, TestValues) {
    auto values = testMap->values();
    EXPECT_EQ(values.size(), 3);

    // Check that all expected values are present
    std::set<double> valueSet;
    for (const auto& value : values) {
        if (value->type() == Type::NUMBER) {
            valueSet.insert(std::static_pointer_cast<Number>(value)->value());
        }
    }

    EXPECT_EQ(valueSet.size(), 3);
    EXPECT_NE(valueSet.find(1.0), valueSet.end());
    EXPECT_NE(valueSet.find(2.0), valueSet.end());
    EXPECT_NE(valueSet.find(3.0), valueSet.end());
}

TEST_F(MapTest, TestEntries) {
    auto entries = testMap->entries();
    EXPECT_EQ(entries.size(), 3);

    // Check that all expected key-value pairs are present
    std::map<std::string, double> entryMap;
    for (const auto& entry : entries) {
        entryMap[entry.first] = std::static_pointer_cast<Number>(entry.second)->value();
    }

    EXPECT_EQ(entryMap.size(), 3);
    EXPECT_EQ(entryMap["one"], 1.0);
    EXPECT_EQ(entryMap["two"], 2.0);
    EXPECT_EQ(entryMap["three"], 3.0);
}

TEST_F(MapTest, TestForEach) {
    std::map<std::string, double> result;

    testMap->forEach([&result](const std::string& key, const ValuePtr& value) {
        if (value->type() == Type::NUMBER) {
            result[key] = std::static_pointer_cast<Number>(value)->value();
        }
    });

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result["one"], 1.0);
    EXPECT_EQ(result["two"], 2.0);
    EXPECT_EQ(result["three"], 3.0);
}

TEST_F(MapTest, TestMap) {
    // Create a new map with values doubled
    auto doubled = testMap->map([](const std::string& key, const ValuePtr& value) -> ValuePtr {
        if (value->type() == Type::NUMBER) {
            double num = std::static_pointer_cast<Number>(value)->value();
            return Number::create(num * 2);
        }
        return value;
    });

    EXPECT_EQ(doubled->size(), 3);
    EXPECT_EQ(std::static_pointer_cast<Number>(doubled->get("one"))->value(), 2.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(doubled->get("two"))->value(), 4.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(doubled->get("three"))->value(), 6.0);
}

TEST_F(MapTest, TestFilter) {
    // Filter to keep only even numbers
    auto filtered = testMap->filter([](const std::string& key, const ValuePtr& value) -> bool {
        if (value->type() == Type::NUMBER) {
            double num = std::static_pointer_cast<Number>(value)->value();
            return static_cast<int>(num) % 2 == 0;
        }
        return false;
    });

    EXPECT_EQ(filtered->size(), 1);  // Only "two" has an even value
    EXPECT_TRUE(filtered->has("two"));
    EXPECT_FALSE(filtered->has("one"));
    EXPECT_FALSE(filtered->has("three"));
}

TEST_F(MapTest, TestReduce) {
    // Sum all values
    auto sum = testMap->reduce(
        [](const ValuePtr& acc, const std::string& key, const ValuePtr& value) -> ValuePtr {
            double currentSum = acc ? std::static_pointer_cast<Number>(acc)->value() : 0.0;
            if (value->type() == Type::NUMBER) {
                currentSum += std::static_pointer_cast<Number>(value)->value();
            }
            return Number::create(currentSum);
        },
        Number::create(0.0));

    EXPECT_NE(sum, nullptr);
    EXPECT_EQ(sum->type(), Type::NUMBER);
    EXPECT_EQ(std::static_pointer_cast<Number>(sum)->value(), 6.0);
}

TEST_F(MapTest, TestMerge) {
    auto otherMap = Map::create();
    otherMap->set("three", Number::create(30));
    otherMap->set("four", Number::create(4));

    auto merged = testMap->merge(otherMap);

    EXPECT_EQ(merged->size(), 4);
    EXPECT_EQ(std::static_pointer_cast<Number>(merged->get("one"))->value(), 1.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(merged->get("two"))->value(), 2.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(merged->get("three"))->value(),
              30.0);                                                                 // Overwritten
    EXPECT_EQ(std::static_pointer_cast<Number>(merged->get("four"))->value(), 4.0);  // Added
}

// Add more tests for edge cases and additional functionality
